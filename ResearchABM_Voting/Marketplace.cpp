// Marketplace.cpp
// a class which messages agents, controls prices, calculates stats

#include "stdafx.h"
#include "Marketplace.h"
#include "CSVWriter.h"

using namespace std;

#define MIN_PRICE 0.1f
#define BUY_CALC_INTERVAL 1


bool cmp_wealth(const agentRank& a, const agentRank& b)
{
	return a.salary > b.salary; // highest comes first
}

bool cmp_popsize(const agentRank& a, const agentRank& b)
{
	return a.populationSize > b.populationSize; // highest comes first
}

// Consumption - the consumption rate of an agent - between 0 and 1. If there is enough money and available stock, the agent will consume 1/4 (0.25 units) of each stock.
// Stocks - how much product there is
// Prices - the price of a product - goes up if there is a shortage, down if there is a surplus
// Producer Count	- how many agents are employed in a specific role. Agents will switch to a random role if they are in the lowest salary bracket and are 'unhappy', whether they switch is set by a probability
//					- satisfaction increases if they earn more than other agents, decreases in opposite
// Salaries - selling price of product agent produces * amount produced (which is a factor of consumption). Agents always receive *some* salary (0.1) which can equate to a minimum wage, or benefits. 
Marketplace::Marketplace()
{
	m_linesWritten = 0;
	m_stocks.clear();
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		m_stocks.push_back(1);
		m_averageStocks.push_back(1);
	}

	m_prices.clear();
	m_systemSellingPrices.clear();
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		m_prices.push_back(1);
		m_systemSellingPrices.push_back(1);
		m_systemBuyingPrices.push_back(1);
	}


	m_wealthRank.clear();
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		agentRank r;
		r.type = i;
		r.wealth = 0;
		m_wealthRank.push_back(r);
	}

	m_inShock = false;
}

void Marketplace::setPrice(PRODUCT_TYPE type, const float price)
{
	m_prices[type] = max(price, MIN_PRICE);
}

void Marketplace::update(vector<Agent*> vecAgents)
{
	// agents - produce new stocks
	for (Agent* a : vecAgents) {
		a->resetLastSalary();
		a->produce();
	}

	// gather total stocks of each type
	calculateStocksReadyToSell(vecAgents);

	
	calculateAverageMoney(vecAgents);

	// gather the agent selling prices for their type
	calculateAgentSellingPrices(vecAgents);


	// now create an auction and allow agents to buy off each other
	// create two lists of all agents, a seller and buyer list 
	// for each type...
	//		1. order the seller list into low to high for that type
	//		2. order the buyer list into low to high for that type
	//		2.1 current buyer is the highest buyer
	//		3. perform a match with lowest seller and highest buyer 
	//			- lowest seller must have enough stock & 
	//			- must be selling at a lower price the buyer is prepared to pay &
	//			- buyer must have enough money (no partial portions)
	//		4. perform a transaction between seller and buyer, update stock levels on agent and here, record the price (price is midpoint between two offers)
	//		4.2 seller gets the proceeeds / reduces stock, buyer get stock / reduces money
	//		4.3 pop the buyer off the buying vector
	//		5. go to 2.1


	// randomise the type loop each time otherwise the later types might consistently not sell if agents run out of money
	vecInt vTypesRandomised;
	for (unsigned int type = 0; type < PRODUCT_TYPE_COUNT; type++) {
		vTypesRandomised.push_back(type);
	}
	std::random_shuffle(vTypesRandomised.begin(), vTypesRandomised.end());

	for (unsigned int type : vTypesRandomised) {

		vector<Agent*> sellers;
		vector<Agent*> buyers;
		
		getBuyersNotOfType(type, vecAgents, buyers);
		getSellersOfType(type, vecAgents, sellers);

		// sort the sellers and buyers into buying & selling prices - in each case lowest price is first in list
		std::sort(buyers.begin(), buyers.end(), buyersorter(type));
		std::sort(sellers.begin(), sellers.end(), sellersorter(type));

		float averageSellingPrice = 0;
		float transactionCount = 0;

		while (buyers.size() > 0) {
			// the highest bidder for this type is the last one
			Agent* buyer = buyers[buyers.size()-1];
			
			// find a seller with enough stock
			Agent* seller = nullptr;
			for (Agent* a : sellers) {
				if (a->storesToSell() >= 1) {
					seller = a;
					break;
				}
			}

			if (seller == nullptr) {
				break; // there are no sellers left with enough stock
			}

			// final selling price is the midpoint between the two
			float finalSellingPrice = buyer->buyingPrice(type) - seller->sellingPrice();
			finalSellingPrice /= 2;
			finalSellingPrice += seller->sellingPrice();

			// seller and buyers will not trade when prices are too high or buyer doesn't have enough money
			// all subsequent sellers will be more expensive, so round is over for this buyer
			if (seller->sellingPrice() > buyer->buyingPrice(type) || finalSellingPrice > buyer->money()) {
				
				buyers.pop_back(); // buyer is not satisfied but will not get a better deal
				continue;
			}
			
			seller->sell(finalSellingPrice);
			buyer->buy(type, finalSellingPrice);

#ifdef SELF_ADAPTIVE_PRICING // no point doing this if the system sets prices
			
			// re-calculate selling prices after each purchase
			seller->calculateSellingPrice(m_prices);
			std::sort(sellers.begin(), sellers.end(), sellersorter(type));
#endif 

			// record the price
			averageSellingPrice += finalSellingPrice;
			transactionCount++;

			// remove the buyer as they are now satisfied
			buyers.pop_back();
		}

		// record the price
		if(transactionCount == 0)
			setPrice(type, 0);
		else
			setPrice(type, averageSellingPrice / transactionCount);
	}

	calculateAverageFood(vecAgents);
	// gather the agent buying prices for each type
	calculateAgentBuyingPrices(vecAgents);


	// Agents consume stocks
	for (Agent* a : vecAgents) {
		a->consume();
	}

	calculateAggregateStatistics(vecAgents);

	for (Agent* a : vecAgents)
	{
		a->updateSatisfaction(this);
	}
}

void Marketplace::getSellersOfType(const unsigned int type, vector<Agent*> vecAgentsIn, vector<Agent*>& vecAgentsOut)
{
	for (Agent* a : vecAgentsIn) {
		if (a->produceType() == type)
			vecAgentsOut.push_back(a);
	}
}

void Marketplace::getBuyersNotOfType(const unsigned int type, vector<Agent*> vecAgentsIn, vector<Agent*>& vecAgentsOut)
{
	for (Agent* a : vecAgentsIn) {
		if (a->produceType() != type)
			vecAgentsOut.push_back(a);
	}
}

PRODUCT_TYPE Marketplace::getHighestWealthRank()
{
	vecRank vr = m_wealthRank;
	std::sort(vr.begin(), vr.end(), cmp_wealth);

	agentRank r = vr[0];
	return r.type;
}

int	Marketplace::getWealthRank(PRODUCT_TYPE type)
{
	vecRank vr = m_wealthRank;
	std::sort(vr.begin(), vr.end(), cmp_wealth);

	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		agentRank r = vr[i];
		if (r.type == type) {
			return i + 1;
		}
	}

	return 0;
}

int	Marketplace::getWealthRankForSelf(const float salary)
{
	vecRank vr = m_wealthRank;
	std::sort(vr.begin(), vr.end(), cmp_wealth);

	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		agentRank r = vr[i];
		if (r.salary < salary) {
			return i + 1;
		}
	}

	// salary is lower than all
	return 10;
}



float Marketplace::totalStoresForSellers(vector<Agent*> vecAgents)
{
	float t = 0;
	for (Agent* a : vecAgents) {
		t += a->storesToSell();
	}
	return t;
}



// CALCULATE METHODS ----------------------------------------------------------

void Marketplace::calculateStocksReadyToSell(vector<Agent*> vecAgents)
{
	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++)
		m_stocks[i] = 0;

	for (Agent* a : vecAgents) {
		int type = a->produceType();
		m_stocks[type] += a->storesToSell();
	}

	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++)
		m_averageStocks[i] = m_stocks[i] / 100; // 100 is the general num of people of that type
	
}

void Marketplace::calculateAverageFood(vector<Agent*> vecAgents)
{
	m_averageFood.clear();
	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++)
		m_averageFood.push_back(0);

	for (Agent* a : vecAgents) {
		for (int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
			m_averageFood[i] += a->storesOfType(i);
		}
	}

	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++)
		m_averageFood[i] /= vecAgents.size(); 
}

void Marketplace::calculateAverageMoney(vector<Agent*> vecAgents)
{
	m_averageMoney.clear();
	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++)
		m_averageMoney.push_back(0);

	for (Agent* a : vecAgents) {
		int type = a->produceType();
		m_averageMoney[type] += a->money();
	}

	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++)
		m_averageMoney[i] /= 100; // 100 is the general num of people of that type
}

#define STORES_SELLING_DISCOUNT (PRODUCT_TYPE_COUNT * 2)

void Marketplace::calculateSystemSellingPrice()
{
	float desiredReserves = STORES_SELLING_DISCOUNT; // of stores above this value then sell for a discount - see Steiglitz algorithm

	// see notes / spreadsheet and Steiglitz for explanations of these
	float b00 = 15;		// Bid function for food reserves parameter
	float b01 = 10;		// Bid function for gold reserves parameter
	float b0inf = 1;	// Bid function for infinity
	float moneyForEachType = 1; // not relevant so set to one
	float y = log((b0inf - b00) / (b0inf - b01));

	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++)
	{
		float fu = m_averageStocks[i] / desiredReserves;
		float gu = moneyForEachType / (m_prices[i] / desiredReserves);
		float b0gu = b0inf - (b0inf - b00) * exp(-y * gu);
		float bfugu = powf(b0gu, 1 - fu);
		m_systemSellingPrices[i] = bfugu;

	}
}

void Marketplace::calculateSystemBuyingPrice()
{
	float BASELINE_RESERVES = 2;

	float desiredReserves = BASELINE_RESERVES;
	float b00 = 4;
	float b01 = 8;
	float b0inf = 10;

	float y = log((b0inf - b00) / (b0inf - b01));
	
	for (int type = 0; type < PRODUCT_TYPE_COUNT; type++) {
		float fu = m_averageFood[type] / desiredReserves;
		float moneyForEachType = m_averageMoney[type] / PRODUCT_TYPE_COUNT;
		float gu = moneyForEachType / (m_prices[type] / desiredReserves);
		float b0gu = b0inf - (b0inf - b00) * exp(-y * gu);
		float bfugu = powf(b0gu, 1 - fu);
		m_systemBuyingPrices[type] = min(bfugu, m_averageMoney[type]);

		// test - avoid the situation where the buying price is always below the selling price (and thus never selling any)
		m_systemBuyingPrices[type] = max(m_systemBuyingPrices[type], 1.0f);
	}
}

void Marketplace::calculateAgentSellingPrices(vector<Agent*> vecAgents) 
{
#ifndef SELF_ADAPTIVE_PRICING
	calculateSystemSellingPrice();
#endif

	for (Agent* a : vecAgents) {
#ifdef SELF_ADAPTIVE_PRICING
		a->calculateSellingPrice(m_prices);
#else
		a->setSellingPrices(m_systemSellingPrices);
#endif
	}
}

void Marketplace::calculateAgentBuyingPrices(vector<Agent*> vecAgents)
{
	
#ifndef SELF_ADAPTIVE_PRICING
	calculateSystemBuyingPrice();
#endif

	for (Agent* a : vecAgents) {
#ifdef SELF_ADAPTIVE_PRICING
		a->calculateBuyingPrice(m_prices);
#else
		a->setBuyingPrices(m_systemBuyingPrices);
#endif
	}
	 
}

// gini 0 = high equality, 1 = low equality
float Marketplace::calculateEquality(vector<float> values)
{
	int r, c, n;
	float t = 0;

	float average = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
	if (average == 0)
		return 0;

	n = (int)values.size();
	for (r = 1; r < n; r++) {
		for (c = 0; c <= r - 1; c++) {
			t = t + fabs(values[r] - values[c]);
		}
	}

	float gini = t / ((float)n * average * (float)n);
	return gini;
}

// note this does not sort by wealth
void Marketplace::calculateAggregateStatistics(vector<Agent*> vecAgents)
{
	m_wealthRank.clear();

	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		agentRank r;
		r.populationSize = 0;
		r.salary = 0;
		r.type = i;
		r.wealth = 0;
		r.satisfaction = 0;
		r.consumption = 0;

		r.salary_median = 0;

		r.salary_min = 0;
		r.wealth_min = 0;
		r.satisfaction_min = 0;
		r.consumption_min = 0;

		r.salary_max = 0;
		r.wealth_max = 0;
		r.satisfaction_max = 0;
		r.consumption_max = 0;
		r.sales = 0;
		r.jobMovement = 0;
		r.sellMovement = 0;

		m_wealthRank.push_back(r);
	}

	vecFloat salaries[PRODUCT_TYPE_COUNT];
	vecFloat wealth[PRODUCT_TYPE_COUNT];
	vecFloat consumption[PRODUCT_TYPE_COUNT];
	vecFloat satisfaction[PRODUCT_TYPE_COUNT];
	vecFloat overallSalaries;
	vecFloat overallWealth;
	vecFloat overallConsumption;
	vecFloat overallSatisfaction;
	overallSalaries.clear();
	overallWealth.clear();
	overallConsumption.clear();
	overallSatisfaction.clear();

	for (Agent* a : vecAgents) {
		int produceType = a->produceType();

		m_wealthRank[produceType].populationSize++;

		m_wealthRank[produceType].salary += a->lastSalary();
		m_wealthRank[produceType].wealth += a->money();
		m_wealthRank[produceType].consumption += a->consumption();
		m_wealthRank[produceType].satisfaction += a->satisfaction();
		m_wealthRank[produceType].sellMovement += a->sellingPriceDiff();

		salaries[produceType].push_back(a->lastSalary());
		wealth[produceType].push_back(a->money());
		consumption[produceType].push_back(a->consumption());
		satisfaction[produceType].push_back(a->satisfaction());

		overallSalaries.push_back(a->lastSalary());
		overallWealth.push_back(a->money());
		overallConsumption.push_back(a->consumption());
		overallSatisfaction.push_back(a->satisfaction());

		m_wealthRank[produceType].salary_min = min(a->lastSalary(), m_wealthRank[produceType].salary_min);
		m_wealthRank[produceType].wealth_min = min(a->money(), m_wealthRank[produceType].wealth_min);
		m_wealthRank[produceType].satisfaction_min = min(a->satisfaction(), m_wealthRank[produceType].satisfaction_min);
		m_wealthRank[produceType].consumption_min = min(a->consumption(), m_wealthRank[produceType].consumption_min);

		m_wealthRank[produceType].salary_max = max(a->lastSalary(), m_wealthRank[produceType].salary_max);
		m_wealthRank[produceType].wealth_max = max(a->money(), m_wealthRank[produceType].wealth_max);
		m_wealthRank[produceType].satisfaction_max = max(a->satisfaction(), m_wealthRank[produceType].satisfaction_max);
		m_wealthRank[produceType].consumption_max = max(a->consumption(), m_wealthRank[produceType].consumption_max);

		m_wealthRank[produceType].sales += a->sales();
	}

	for (agentRank r : m_wealthRank) {
		if (r.populationSize == 0) {
			r.salary = 0;
			r.wealth = 0;
			r.satisfaction = 0;
			r.consumption = 0;
		}
		else {
			r.wealth /= r.populationSize;		// how much money they have in the bank (avg)
			r.satisfaction /= r.populationSize;	// a value over time indicating how much an agent earns compared to others
			r.consumption /= r.populationSize;		// a value over time indicating how much an agent has ate (avg)

			r.salary /= r.populationSize;
			r.sellMovement /= r.populationSize;
			r.jobMovement /= r.populationSize;

			r.salary_equality = calculateEquality(salaries[r.type]);
			r.wealth_equality = calculateEquality(wealth[r.type]);
			r.consumption_equality = calculateEquality(consumption[r.type]);
			r.satisfaction_equality = calculateEquality(satisfaction[r.type]);
		}

		m_wealthRank[r.type] = r;
	}

	std::sort(overallWealth.begin(), overallWealth.end());
	std::sort(overallSalaries.begin(), overallSalaries.end());
	std::sort(overallSatisfaction.begin(), overallSatisfaction.end());
	std::sort(overallConsumption.begin(), overallConsumption.end());

	m_equality.clear();
	{
		m_equality.push_back(calculateEquality(overallWealth));
		m_equality.push_back(calculateEquality(overallSalaries));
		m_equality.push_back(calculateEquality(overallSatisfaction));
		m_equality.push_back(calculateEquality(overallConsumption));
	}
}




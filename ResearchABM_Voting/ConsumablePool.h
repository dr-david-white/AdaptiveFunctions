#pragma once

#include "Agent.h"
#include "CSVWriter.h"

// names for output files
#define SALES_CSV "sales.csv"
#define STOCKS_CSV "stocks.csv"
#define WEALTH_CSV "wealth.csv"
#define PRICES_CSV "prices.csv"
#define CONSUMPTION_CSV "consumption.csv"
#define SALARIES_CSV "salaries.csv"
#define PRODUCERCOUNT_CSV "producercount.csv"
#define HAPPINESS_CSV "satisfaction.csv"
#define EQUALITY_CSV "equality.csv"
#define EQUALITY_PRODUCERS_CSV "equality_producers.csv"
#define POPULATION_CSV "population.csv"
#define SELLPRICECHANGE_CSV "sellpricechange.csv"

#define DIFFERENCEPRICENEEDS_SELLING_CSV "differencepriceneeds_selling.csv"
#define DIFFERENCEPRICENEEDS_BUYING_CSV "differencepriceneeds_buying.csv"

#define SHOCK_CSV "shock.csv"

struct agentRank
{
	int populationSize;
	float wealth;
	float salary;
	float salary_median;
	float satisfaction;
	float consumption;

	float wealth_equality;
	float salary_equality;
	float satisfaction_equality;
	float consumption_equality;

	float wealth_min;
	float salary_min;
	float satisfaction_min;
	float consumption_min;

	float wealth_max;
	float salary_max;
	float satisfaction_max;
	float consumption_max;

	float sales; // sales this cycle
	float jobMovement;
	float sellMovement;
	PRODUCT_TYPE type;
};


typedef  std::vector<agentRank> vecRank;

class ConsumablePool
{
	ConsumablePool();
	// Stop the compiler generating methods of copy the object
	ConsumablePool(ConsumablePool const& copy);            // Not Implemented
	ConsumablePool& operator=(ConsumablePool const& copy); // Not Implemented

public:
	static ConsumablePool& getInstance()
	{
		// The only instance
		static ConsumablePool instance;
		return instance;
	}

	float	getPrice(PRODUCT_TYPE type);
	void	setPrice(PRODUCT_TYPE type, const float price);

	void	update(vector<Agent*> vecAgents);
	void	calculateAggregateStatistics(vector<Agent*> vecAgents);
	PRODUCT_TYPE getHighestWealthRank();
	int		getWealthRank(PRODUCT_TYPE type);
	int		getWealthRankForSelf(const float salary);

	void	printPriceNeedsSelling(const unsigned int rowsWritten);
	void	printSatisfaction(const unsigned int rowsWritten);
	void	printConsumption(const unsigned int rowsWritten);
	void	printEquality(const unsigned int rowsWritten, PRODUCT_TYPE type);
	void	printOverallEquality(const unsigned int rowsWritten);
	void	printStocks(const unsigned int rowsWritten);
	void	printWealth(const unsigned int rowsWritten);
	void	printPrices(const unsigned int rowsWritten);
	void	printSalary(const unsigned int rowsWritten);
	void	printSales(const unsigned int rowsWritten);
	void	printFinalFormula(string name, const unsigned int rowsWritten);
	void	printPopulation(vector<Agent*> vecAgents);
	void	printSellMovement(const unsigned int rowsWritten);
	void	printShockPeriod(const unsigned int rowsWritten);
	
	void	printProducers(vector<Agent*> vecAgents);
	void	setInShock(bool inShock) { m_inShock = inShock; }

	std::streampos	initialiseCSV(string name);
	std::streampos	initialiseCSV(string name, string title);
	std::streampos	initialiseEqualityCSV(string name);

	void	calculateStocksReadyToSell(vector<Agent*> vecAgents);
	
	void	calculateAgentSellingPrices(vector<Agent*> vecAgents);
	void	calculateAgentBuyingPrices(vector<Agent*> vecAgents);

	void	calculateSystemSellingPrice();
	void	calculateSystemBuyingPrice();

	void	calculateAverageFood(vector<Agent*> vecAgents);
	void	calculateAverageMoney(vector<Agent*> vecAgents);
	
	std::string getNameFromType(const unsigned int type);
	void	getSellersOfType(const unsigned int type, vector<Agent*> vecAgents, vector<Agent*>& vecAgentsOut);
	void	getBuyersNotOfType(const unsigned int type, vector<Agent*> vecAgents, vector<Agent*>& vecAgentsOut);
	
	float 	calculateEquality(vector<float> values);
	float	totalStoresForSellers(vector<Agent*> vecAgents);

protected:
	void	printRowFormula(CSVWriter& csv, const unsigned int rowsWritten);

private:
	vecFloat m_stocks;
	vecFloat m_averageStocks;

	vecFloat m_averageFood;
	vecFloat m_averageMoney;

	vecFloat m_prices;
	vecFloat m_priceNeedsSelling; // the difference in the system buying price and the agent buying price 
	//vecFloat m_priceNeedsBuying; // as above for selling

	vecFloat m_systemSellingPrices;
	vecFloat m_systemBuyingPrices;

	vecRank m_wealthRank;

	vecFloat m_overallSalaries;
	vecFloat m_overallWealth;
	vecFloat m_overallConsumption;
	vecFloat m_overallSatisfaction;

	unsigned int m_linesWritten;
	bool		m_inShock;
};


class buyersorter {
	int type;
public:
	buyersorter(int _type) : type(_type) {}

	bool SortFunction(const Agent* obj1, const Agent* obj2, const int type) {
		return obj1->buyingPrice(type) < obj2->buyingPrice(type);
	}

	bool operator()(const Agent* o1, const Agent* o2) {
		return SortFunction(o1, o2, type);
	}
};

class sellersorter {
	int type;
public:
	sellersorter(int _type) : type(_type) {}

	bool SortFunction(const Agent* obj1, const Agent* obj2, const int type) {
		return obj1->sellingPrice() < obj2->sellingPrice();
	}

	bool operator()(const Agent* o1, const Agent* o2) {
		return SortFunction(o1, o2, type);
	}
};


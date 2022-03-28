// agent.cpp
// the agent

#include "stdafx.h"
#include "Agent.h"
#include "ConsumablePool.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <math.h>  

using namespace std;


#define STORES_SELLING_DISCOUNT (PRODUCT_TYPE_COUNT * 2)
#define MAX_CONSUMPTION_OF_PRODUCT 0.9f
#define MAX_STOCK 60


Agent::Agent(bool influencer, int id) : m_id(id)
{
	m_satisfaction = (rand() % 100) / 100.0f;
	m_consumption = (rand() % 100) / 100.0f;

	m_money = 100;
	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		m_storesToConsume.push_back(0);
		m_buyingPrices.push_back(0);
	}

	m_lastSalary = 0;
	m_producerType = -1; // needs to be not an actual one or the random selection won't work
	m_storesToSell = 0;
	m_sellingPrice = 1;

	m_sales = 0;

	chooseRandomType();

	// set productivity random between 0.8 and 1
	int r = rand() % 100;
	float productivity = (float)r;
	productivity /= 100;
	productivity *= 0.2;
	productivity += 0.8;

	setProductivity(min(1.0f, productivity));
}

Agent::~Agent()
{
}

void Agent::chooseSpecificType(PRODUCT_TYPE type)
{
	m_producerType = type;
}

void Agent::chooseRandomType() {
	PRODUCT_TYPE current = m_producerType;
	while (current == m_producerType) {
		int n = rand() % PRODUCT_TYPE_COUNT;
		m_producerType = n;
	}
}

float Agent::produce()
{
	m_sales = 0;

	// the agent produces in direct proportion to their productivity
	m_storesToSell += m_productivity * m_rawMaterialAvailablity;

	if (m_storesToSell > MAX_STOCK)
		m_storesToSell = MAX_STOCK;

	return m_storesToSell;
}

float Agent::storesOfType(int type) {
	float totalStores = 0;
	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		totalStores = m_storesToConsume[type];
	}
	return totalStores;
}

void Agent::consume()
{
	//float totalStoresV = totalStores();
	float totalConsumed = 0;

	for (int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		float productConsumed = m_storesToConsume[i];
		m_storesToConsume[i] = max(0.0f, m_storesToConsume[i]-0.9f);
		productConsumed -= m_storesToConsume[i]; // so, 0.9 or some value smaller
		totalConsumed += productConsumed;
	}

	// consumption is the amount consumed. Perfect consumption is total consumption of 0.9 units of each product
	// this value is weighted to produce a value between 0 and 1 - 1 being perfect consumption
	totalConsumed /= PRODUCT_TYPE_COUNT;
	totalConsumed /= MAX_CONSUMPTION_OF_PRODUCT;

	m_consumption = totalConsumed;

	m_consumption = min(m_consumption, 1.0f);
	m_consumption = max(m_consumption, 0.0f);
}

void Agent::buy(int type, float price)
{
	m_storesToConsume[type] += 1;
	m_money -= price;
}

void Agent::sell(float price)
{
	m_sales += 1;
	m_storesToSell -= 1;
	m_money += price;
	m_lastSalary += price;
}

float Agent::consumption()
{
	return m_consumption;
}

void Agent::updateSatisfaction()
{
	float delta = 0.1f;
	ConsumablePool& pool = ConsumablePool::getInstance();

	// produce a rank value between 0 and 1 (1 = ranked top, 0 = bottom)
	int rank = pool.getWealthRankForSelf(m_lastSalary); // 1 = top 10 = bottom
	float satisfactionInRankStanding = (float)rank;

	// https://journals.sagepub.com/doi/10.1177/0956797610362671

	float nl = 1.75;
	float i = (PRODUCT_TYPE_COUNT - rank);
	float n = PRODUCT_TYPE_COUNT;

	float numerator = (i - 1) - (nl * (n - i));
	float denominator = 2 * ((i - 1) + (nl * (n - i)));
	float SR = 0.5 + (numerator / denominator);

	m_satisfaction = SR;
}

float randomValue(float min, float max) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

void Agent::setBuyingPrices(const vecFloat& prices)
{
	for (int type = 0; type < PRODUCT_TYPE_COUNT; type++) {
		m_buyingPrices[type] = prices[type];
	}
}

void Agent::setSellingPrices(const vecFloat& prices)
{
	float lastSellingPrice = m_sellingPrice;
	m_sellingPrice = prices[m_producerType];
	m_sellingPriceDiff = fabs(m_sellingPrice - lastSellingPrice);
}

float Agent::calculateSellingPrice(const vecFloat& prices, bool bVirtual)
{
	float desiredReserves = STORES_SELLING_DISCOUNT; // of stores above this value then sell for a discount - see Steiglitz algorithm

	// see notes / spreadsheet and Steiglitz for explanations of these
	float b00 = 15;		// Bid function for food reserves parameter
	float b01 = 10;		// Bid function for gold reserves parameter
	float b0inf = 1;	// Bid function for infinity

	float moneyForEachType = 1; // not relevant so set to one

	// for the self adaptive selling price, the consumption of the agent affects the buying price 'around' the baseline reserves. 
	// low consumption is likely to lead to a higher selling price, vice versa. 
	float maxReserves = STORES_SELLING_DISCOUNT * 1.5f;
	float minReserves = STORES_SELLING_DISCOUNT * 0.5f;
	float diffReserves = maxReserves - minReserves;
	desiredReserves = minReserves + ((1 - m_consumption) * diffReserves);

	float lastSellingPrice = m_sellingPrice;

	float y = log((b0inf - b00) / (b0inf - b01));

	float fu = m_storesToSell / desiredReserves;
	float gu = moneyForEachType / (prices[m_producerType] / desiredReserves);
	float b0gu = b0inf - (b0inf - b00) * exp(-y * gu);

	float bfugu = powf(b0gu, 1 - fu);

	if (!bVirtual) {
		m_sellingPrice = bfugu;
		m_sellingPriceDiff = fabs(m_sellingPrice - lastSellingPrice);
		return 0;
	}
	else
	{
		return bfugu;
	}
}

void Agent::calculateBuyingPrice(const vecFloat& prices)
{
	// see notes / spreadsheet and Steiglitz for explanations of these	
	float BASELINE_RESERVES = 2;

	float desiredReserves = BASELINE_RESERVES;
	float b00 = 4;
	float b01 = 8;
	float b0inf = 10; 
	 
	// for the self adaptive buying price, the consumption of the agent affects the buying price 'around' the baseline reserves. 
	// high consumption = lower buying price / vice versa
	float maxReserves = BASELINE_RESERVES * 1.5f;
	float minReserves = BASELINE_RESERVES * 0.5f;
	float diffReserves = maxReserves - minReserves;
	desiredReserves = minReserves + ((1 - m_consumption) * diffReserves);

	float y = log((b0inf - b00) / (b0inf - b01));
	float moneyForEachType = m_money / PRODUCT_TYPE_COUNT;
	for (int type = 0; type < PRODUCT_TYPE_COUNT; type++) {
		float fu = m_storesToConsume[type] / desiredReserves;
		float gu = moneyForEachType / (prices[type] / desiredReserves);
		float b0gu = b0inf - (b0inf - b00) * exp(-y * gu);
		float bfugu = powf(b0gu, 1 - fu);

		m_buyingPrices[type] = min(bfugu, m_money);
	}
}

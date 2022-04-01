#pragma once

#include "Agent.h"
#include "CSVWriter.h"





class Marketplace
{
public:

	Marketplace();

public:

	void	setPrice(PRODUCT_TYPE type, const float price);

	void	update(vector<Agent*> vecAgents);
	void	calculateAggregateStatistics(vector<Agent*> vecAgents);
	
	void	setInShock(bool inShock) { m_inShock = inShock; }
	bool	isInShock() { return m_inShock; }

	int			getWealthRankForSelf(const float salary);
	vecRank		getAgentRankings() { return m_wealthRank; }
	vecFloat	getOverallEquality() { return m_equality; }
	vecFloat	getStocks() { return m_stocks; }
	vecFloat	getPrices() { return m_prices; }

private:
	PRODUCT_TYPE getHighestWealthRank();
	int		getWealthRank(PRODUCT_TYPE type);

	float 	calculateEquality(vector<float> values);
	float	totalStoresForSellers(vector<Agent*> vecAgents);

	void	calculateStocksReadyToSell(vector<Agent*> vecAgents);

	void	calculateAgentSellingPrices(vector<Agent*> vecAgents);
	void	calculateAgentBuyingPrices(vector<Agent*> vecAgents);

	void	calculateSystemSellingPrice();
	void	calculateSystemBuyingPrice();

	void	calculateAverageFood(vector<Agent*> vecAgents);
	void	calculateAverageMoney(vector<Agent*> vecAgents);

	void	getSellersOfType(const unsigned int type, vector<Agent*> vecAgents, vector<Agent*>& vecAgentsOut);
	void	getBuyersNotOfType(const unsigned int type, vector<Agent*> vecAgents, vector<Agent*>& vecAgentsOut);


private:
	vecFloat m_stocks;
	vecFloat m_averageStocks;

	vecFloat m_averageFood;
	vecFloat m_averageMoney;

	vecFloat m_prices;

	vecFloat m_systemSellingPrices;
	vecFloat m_systemBuyingPrices;

	vecRank	m_wealthRank;
	vecFloat m_equality;

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


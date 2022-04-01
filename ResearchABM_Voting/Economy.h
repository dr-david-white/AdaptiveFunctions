#pragma once

#include "Marketplace.h"
class Agent;

class Economy
{
public:
	Economy();
	~Economy();

	void update(const unsigned int day);
	Marketplace* getMarketplace() { return &m_pool; }

private:
	void shocks(vector<Agent*>& vecAgents, const int& rowsWritten);
	void setRawMaterialAvailability(bool disruption);

private:
	vector<Agent*> m_vecAgents;
	Marketplace m_pool;
	bool m_bInShockPeriod;
	int m_startShockPeriod;
	int m_endShockPeriod;
	float m_rawMaterialAvailablity[10]; // the levels of reduced raw materials during a disruption for each profession
};


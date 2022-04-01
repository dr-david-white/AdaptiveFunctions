// This class represents the environment in which agents live
// and so controls the raw materials and disruptions to them

#include "stdafx.h"
#include "Economy.h"
#include "Agent.h"
#include "Marketplace.h"


#define MIN_SHOCK_DATE 500 // the date of the first disruption / usually 500? - set this to a large number to avoid any shocks
#define LEN_SHOCK_PERIOD_INTERVAL 250000 // the time between shocks - the recovery time (usually 250, large number = just once)
#define LEN_SHOCK_PERIOD 125  // the length of time a disruption occurs for (125, 250 eg)
#define MAX_RAW_MATERIAL_AVAILABILITY 4 // the amount of raw materials given to agents


Economy::Economy()
{
	m_bInShockPeriod;
	m_startShockPeriod = MIN_SHOCK_DATE;
	m_endShockPeriod = m_startShockPeriod + LEN_SHOCK_PERIOD;
	setRawMaterialAvailability(false);

	for (unsigned int i = 0; i < POPULATION_SIZE; i++)
	{
		Agent* agent = new Agent(false, i);
		m_vecAgents.push_back(agent);
		agent->setRawMaterialAvailablity(MAX_RAW_MATERIAL_AVAILABILITY);
	}
}

Economy::~Economy()
{

}

void Economy::update(const unsigned int day)
{
	// Marketplace - calculate the production, auction, prices and consumption for this step
	m_pool.update(m_vecAgents);

	shocks(m_vecAgents, day);
}

void Economy::shocks(vector<Agent*>& vecAgents, const int& rowsWritten)
{

	// PRE SHOCK PERIOD
	if (rowsWritten < m_startShockPeriod) {
		if (rowsWritten % PRINT_INTERVAL == 0) {
			cout << "PRE SHOCK" << endl;
		}

		setRawMaterialAvailability(false);
	}

	m_pool.setInShock(false);
	if (rowsWritten >= m_startShockPeriod && rowsWritten <= m_endShockPeriod)
	{
		// SHOCK PERIOD

		setRawMaterialAvailability(true);
		m_bInShockPeriod = true;
		m_pool.setInShock(true);
		if (rowsWritten % PRINT_INTERVAL == 0) {
			cout << "SHOCK" << endl;
		}

	}

	for (Agent* agent : vecAgents)
	{
		agent->setRawMaterialAvailablity(m_rawMaterialAvailablity[agent->produceType()]);
	}

	// POST SHOCK PERIOD
	if (rowsWritten > m_endShockPeriod) {
		cout << "POST SHOCK" << endl;

		if (m_bInShockPeriod) {
			m_startShockPeriod += LEN_SHOCK_PERIOD_INTERVAL + LEN_SHOCK_PERIOD;
			m_endShockPeriod = m_startShockPeriod + LEN_SHOCK_PERIOD;
			m_bInShockPeriod = false;

			for (Agent* agent : vecAgents)
			{
				agent->setRawMaterialAvailablity(MAX_RAW_MATERIAL_AVAILABILITY);
			}
		}
	}
}

void Economy::setRawMaterialAvailability(bool disruption)
{
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++)
	{
		if (disruption)
		{
			float r = (float)(rand() % 100);
			r /= 100;
			r *= float(MAX_RAW_MATERIAL_AVAILABILITY - 1);

			m_rawMaterialAvailablity[i] = 1 + r;
		}
		else
		{
			m_rawMaterialAvailablity[i] = MAX_RAW_MATERIAL_AVAILABILITY;
		}
	}
}

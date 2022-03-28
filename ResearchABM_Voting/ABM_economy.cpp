// ABM_economy.cpp : Defines the entry point for the console application.
// This class represents the environment in which agents live
// and so controls the raw materials and disruptions to them
//

#include "stdafx.h"
#include <vector>
#include "Agent.h"
#include "ConsumablePool.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <iomanip>  

using namespace std;

#define STABILISATION_PERIOD 150

#define STATISTICS_INTERVAL 1
#define PRINT_INTERVAL 50

#define LIFECYCLES 1050 // the length of the simulation in days
#define MIN_SHOCK_DATE 500 // the date of the first disruption / usually 500? - set this to a large number to avoid any shocks

#define LEN_SHOCK_PERIOD_INTERVAL 250000 // the time between shocks - the recovery time (250, eg, large number = just once)
#define LEN_SHOCK_PERIOD 125  // the length of time a disruption occurs for (125, 250 eg)

#define MAX_RAW_MATERIAL_AVAILABILITY 4 // the amount of raw materials given to agents

static bool bInShockPeriod = false;
static int startShockPeriod = 0;
static int endShockPeriod = 0;


vector<Agent*> vecAgents;
typedef vector<Agent*>::iterator agentitor;
float rawMaterialAvailablity[10]; // the levels of reduced raw materials during a disruption for each profession


void setRawMaterialAvailability(bool disruption);
void shocks(vector<Agent*>& vecAgents, const int& rowsWritten);

int main()
{
	startShockPeriod = MIN_SHOCK_DATE;
	endShockPeriod = startShockPeriod + LEN_SHOCK_PERIOD;

	setRawMaterialAvailability(false);

	srand((unsigned int)time((time_t)NULL));

	for (unsigned int i = 0; i < POPULATION_SIZE; i++)
	{	
		Agent* agent = new Agent(false, i);
		vecAgents.push_back(agent);
		agent->setRawMaterialAvailablity(MAX_RAW_MATERIAL_AVAILABILITY);
	}

	ConsumablePool& pool = ConsumablePool::getInstance();
	unsigned int rowsWritten = 0;

	for (unsigned int i = 0; i < LIFECYCLES; i++)
	{
		// consumablepool - calculate the production, action, prices and consumption for this step
		pool.update(vecAgents);

		// process each day
		if (i % STATISTICS_INTERVAL == 0)
		{
			pool.calculateAggregateStatistics(vecAgents);

			// print the data to the output files
			if (i > STABILISATION_PERIOD) {
				pool.printStocks(rowsWritten);
				pool.printPrices(rowsWritten);
				pool.printSalary(rowsWritten);
				pool.printConsumption(rowsWritten);
				pool.printWealth(rowsWritten);
				pool.printOverallEquality(rowsWritten);
				pool.printSales(rowsWritten);

				pool.printSatisfaction(rowsWritten);

				pool.printProducers(vecAgents);
				pool.printPopulation(vecAgents);

				pool.printSellMovement(rowsWritten);

				pool.printShockPeriod(rowsWritten);

				pool.printPriceNeedsSelling(rowsWritten);

				rowsWritten++;
			}
		}

		if (i % PRINT_INTERVAL == 0) {
			cout << endl;
			cout << "rows written: " << i << endl;
		}

		for (Agent* agent : vecAgents)
		{
			agent->updateSatisfaction();
		}

		shocks(vecAgents, i);
	}

	// print summary formula
	pool.printFinalFormula(STOCKS_CSV, rowsWritten);
	pool.printFinalFormula(WEALTH_CSV, rowsWritten);
	pool.printFinalFormula(PRICES_CSV, rowsWritten);
	pool.printFinalFormula(CONSUMPTION_CSV, rowsWritten);
	pool.printFinalFormula(SALARIES_CSV, rowsWritten);
	pool.printFinalFormula(HAPPINESS_CSV, rowsWritten);
	pool.printFinalFormula(SALES_CSV, rowsWritten);
	
    return 0;
}

void shocks(vector<Agent*>& vecAgents, const int& rowsWritten)
{
	ConsumablePool& pool = ConsumablePool::getInstance();

	// PRE SHOCK PERIOD
	if (rowsWritten < startShockPeriod) {
		if (rowsWritten % PRINT_INTERVAL == 0) {
			cout << "PRE SHOCK" << endl;
		}

		setRawMaterialAvailability(false);
	}

	pool.setInShock(false);
	if (rowsWritten >= startShockPeriod && rowsWritten <= endShockPeriod)
	{
		// SHOCK PERIOD

		setRawMaterialAvailability(true);
		bInShockPeriod = true;
		pool.setInShock(true);
		if (rowsWritten % PRINT_INTERVAL == 0) {
			cout << "SHOCK" << endl;
		}

	}

	for (Agent* agent : vecAgents)
	{
		agent->setRawMaterialAvailablity(rawMaterialAvailablity[agent->produceType()]);
	}

	// POST SHOCK PERIOD
	if (rowsWritten > endShockPeriod) {
		cout << "POST SHOCK" << endl;

		if (bInShockPeriod) {
			startShockPeriod += LEN_SHOCK_PERIOD_INTERVAL + LEN_SHOCK_PERIOD;
			endShockPeriod = startShockPeriod + LEN_SHOCK_PERIOD;
			bInShockPeriod = false;

			for (Agent* agent : vecAgents)
			{
				agent->setRawMaterialAvailablity(MAX_RAW_MATERIAL_AVAILABILITY);
			}
		}
	}
}

void setRawMaterialAvailability(bool disruption)
{
	for (unsigned int i = 0; i < 10; i++)
	{
		if (disruption)
		{
			float r = (rand() % 100);
			r /= 100;
			r *= float(MAX_RAW_MATERIAL_AVAILABILITY - 1);

			rawMaterialAvailablity[i] = 1 + r; // will add up to MAX_RAW_MATERIAL_AVAILABILITY
		}
		else
		{
			rawMaterialAvailablity[i] = MAX_RAW_MATERIAL_AVAILABILITY;
		}
	}
}




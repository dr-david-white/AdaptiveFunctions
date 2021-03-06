// ABM_economy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Agent.h"
#include "Economy.h"
#include "Marketplace.h"
#include "DataOutput.h"

using namespace std;

#define STABILISATION_PERIOD 150
#define LIFECYCLES 1050 // the length of the simulation in days


Economy g_economy;

int main()
{
	srand((unsigned int)time((time_t)NULL));

	DataOutput dataOutput;

	unsigned int rowsWritten = 0;
	for (unsigned int i = 0; i < LIFECYCLES; i++)
	{
		g_economy.update(i);
		// print the data to the output files
		if (i > STABILISATION_PERIOD) {
			dataOutput.printStocks(rowsWritten, g_economy.getMarketplace()->getStocks());
			dataOutput.printPrices(rowsWritten, g_economy.getMarketplace()->getPrices());
			dataOutput.printSalary(rowsWritten, g_economy.getMarketplace()->getAgentRankings());
			dataOutput.printConsumption(rowsWritten, g_economy.getMarketplace()->getAgentRankings());
			dataOutput.printWealth(rowsWritten, g_economy.getMarketplace()->getAgentRankings());
			dataOutput.printOverallEquality(g_economy.getMarketplace()->getOverallEquality());
			dataOutput.printSales(rowsWritten, g_economy.getMarketplace()->getAgentRankings());
			dataOutput.printSatisfaction(rowsWritten, g_economy.getMarketplace()->getAgentRankings());
			dataOutput.printSellMovement(rowsWritten, g_economy.getMarketplace()->getAgentRankings());
			dataOutput.printShockPeriod(rowsWritten, g_economy.getMarketplace()->isInShock());

			rowsWritten++;
		}

		if (i % PRINT_INTERVAL == 0) {
			cout << endl;
			cout << "rows written: " << i << endl;
		}
	}

	// print summary formula
	dataOutput.printFinalFormula(STOCKS_CSV, rowsWritten);
	dataOutput.printFinalFormula(WEALTH_CSV, rowsWritten);
	dataOutput.printFinalFormula(PRICES_CSV, rowsWritten);
	dataOutput.printFinalFormula(CONSUMPTION_CSV, rowsWritten);
	dataOutput.printFinalFormula(SALARIES_CSV, rowsWritten);
	dataOutput.printFinalFormula(HAPPINESS_CSV, rowsWritten);
	dataOutput.printFinalFormula(SALES_CSV, rowsWritten);
	
    return 0;
}





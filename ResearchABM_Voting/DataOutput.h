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

class DataOutput
{
public:
	DataOutput();
	~DataOutput();
	
	std::streampos	initialiseCSV(string name);
	std::streampos	initialiseCSV(string name, string title);
	std::streampos	initialiseEqualityCSV(string name);

	std::string getNameFromType(const unsigned int type);

	// print methods
	void printSales(const unsigned int rowsWritten, vecRank agentRankings);

	void	printFinalFormula(string name, const unsigned int rowsWritten);
	void	printSatisfaction(const unsigned int rowsWritten, vecRank agentRankings);
	void	printConsumption(const unsigned int rowsWritten, vecRank agentRankings);
	void	printOverallEquality(vecFloat equality);
	void	printStocks(const unsigned int rowsWritten, vecFloat stocks);
	void	printWealth(const unsigned int rowsWritten, vecRank agentRankings);
	void	printPrices(const unsigned int rowsWritten, vecFloat prices);
	void	printSalary(const unsigned int rowsWritten, vecRank agentRankings);


	void	printSellMovement(const unsigned int rowsWritten, vecRank agentRankings);
	void	printShockPeriod(const unsigned int rowsWritten, bool inShock);

protected:
	void	printRowFormula(CSVWriter& csv, const unsigned int rowsWritten);
};


#include "stdafx.h"
#include "DataOutput.h"



DataOutput::DataOutput()
{
	// clear the current csv file
	initialiseCSV(DIFFERENCEPRICENEEDS_SELLING_CSV);
	initialiseCSV(SALES_CSV);
	initialiseEqualityCSV(EQUALITY_CSV);
	initialiseCSV(SELLPRICECHANGE_CSV);
	initialiseCSV(SHOCK_CSV, "Shock");

	CSVWriter csv(",");
	csv.writeToFile(POPULATION_CSV);
}

DataOutput::~DataOutput()
{

}

std::streampos DataOutput::initialiseCSV(string name)
{
	CSVWriter csv(",");
	csv.newRow();
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		csv << getNameFromType(i);
	}
	return csv.writeToFile(name);
}

std::streampos DataOutput::initialiseCSV(string name, string title)
{
	CSVWriter csv(",");
	csv.newRow();
	csv << title;
	return csv.writeToFile(name);
}

std::streampos DataOutput::initialiseEqualityCSV(string name)
{
	CSVWriter csv(",");
	csv.newRow() << "Wealth" << "Salary" << "Satisfaction" << "Consumption";
	return csv.writeToFile(name);
}

std::string DataOutput::getNameFromType(const unsigned int type)
{
	switch (type)
	{
	case 0:
		return "Baker";
	case 1:
		return "Barista";
	case 2:
		return "Brewer";
	case 3:
		return "Butcher";
	case 4:
		return "Farmer";
	case 5:
		return "Farrier";
	case 6:
		return "Fisherman";
	case 7:
		return "Grocer";
	case 8:
		return "Miller";
	case 9:
		return "Vinter";
	default:
		return to_string(type);
	}

	return to_string(type); // not needed but stops compiler warnings
}


// PRINT METHODS ----------------------------------------------------------------------------

void DataOutput::printOverallEquality(vecFloat equality)
{
	CSVWriter csv(",");
	csv.newRow();

	csv << equality[0]; // Wealth
	csv << equality[1]; // Salaries
	csv << equality[2]; // Satisfaction
	csv << equality[3]; // Consumption

	csv.writeToFile(EQUALITY_CSV, true);
}

void DataOutput::printSalary(const unsigned int rowsWritten, vecRank agentRankings)
{
	CSVWriter csv(",");
	csv.newRow();
	for (const agentRank& r : agentRankings) {
		csv << r.salary;
	}

	printRowFormula(csv, rowsWritten);

	csv.writeToFile(SALARIES_CSV, true);
}

void DataOutput::printSellMovement(const unsigned int rowsWritten, vecRank agentRankings)
{
	CSVWriter csv(",");
	csv.newRow();
	for (const agentRank& r : agentRankings) {
		csv << r.sellMovement;
	}
	printRowFormula(csv, rowsWritten);
	csv.writeToFile(SELLPRICECHANGE_CSV, true);
}

void DataOutput::printSatisfaction(const unsigned int rowsWritten, vecRank agentRankings)
{
	CSVWriter csv(",");
	csv.newRow();
	for (const agentRank& r : agentRankings) {
		csv << r.satisfaction;
	}
	printRowFormula(csv, rowsWritten);
	csv.writeToFile(HAPPINESS_CSV, true);
}

void DataOutput::printWealth(const unsigned int rowsWritten, vecRank agentRankings)
{
	CSVWriter csv(",");
	csv.newRow();
	for (const agentRank& r : agentRankings)
	{
		csv << r.wealth;
	}
	printRowFormula(csv, rowsWritten);
	csv.writeToFile(WEALTH_CSV, true);
}

void DataOutput::printStocks(const unsigned int rowsWritten, vecFloat stocks)
{
	CSVWriter csv(",");

	csv.newRow();
	for (float p : stocks) {
		csv << p;
	}
	printRowFormula(csv, rowsWritten);
	csv.writeToFile(STOCKS_CSV, true);
}

void DataOutput::printPrices(const unsigned int rowsWritten, vecFloat prices)
{
	CSVWriter csv(",");

	csv.newRow();
	for (float p : prices) {
		csv << p;
	}
	printRowFormula(csv, rowsWritten);

	csv.writeToFile(PRICES_CSV, true);
}

void DataOutput::printShockPeriod(const unsigned int rowsWritten, bool inShock)
{
	CSVWriter csv(",");
	csv.newRow();
	if (inShock)
		csv << 1;
	else
		csv << 0;

	csv.writeToFile(SHOCK_CSV, true);
}

void DataOutput::printConsumption(const unsigned int rowsWritten, vecRank agentRankings)
{
	CSVWriter csv(",");
	csv.newRow();
	for (agentRank r : agentRankings)
	{
		csv << r.consumption;
		if (r.consumption == 0)
		{
			int i = 0;
		}
	}
	printRowFormula(csv, rowsWritten);
	csv.writeToFile(CONSUMPTION_CSV, true);
}


void DataOutput::printSales(const unsigned int rowsWritten, vecRank agentRankings)
{
	CSVWriter csv(",");
	csv.newRow();
	for (agentRank r : agentRankings)
	{
		csv << r.sales;
	}

	printRowFormula(csv, rowsWritten);

	csv.writeToFile(SALES_CSV, true);
}

void DataOutput::printRowFormula(CSVWriter& csv, const unsigned int rowsWritten)
{
	char chFrom = 'A';
	char chTo = 'A' + PRODUCT_TYPE_COUNT - 1;
	unsigned int row = rowsWritten + 2;

	std::ostringstream stringStream;
	stringStream << "=AVERAGE(" << chFrom << row << ":" << chTo << row << ")";
	csv << stringStream.str();
}

void DataOutput::printFinalFormula(string name, const unsigned int rowsWritten)
{
	CSVWriter csv(",");
	csv.newRow();
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		char ch = 'A' + i;
		std::ostringstream stringStream;
		stringStream << "=SUM(" << ch << "2:" << ch << rowsWritten + 1 << ")" << "/" << rowsWritten;
		csv << stringStream.str();
	}
	printRowFormula(csv, rowsWritten);


	csv.newRow();
	for (unsigned int i = 0; i < PRODUCT_TYPE_COUNT; i++) {
		char ch = 'A' + i;
		std::ostringstream stringStream;
		stringStream << "=STDEV(" << ch << "2:" << ch << rowsWritten + 1 << ")";
		csv << stringStream.str();
	}
	printRowFormula(csv, rowsWritten + 1);

	csv.writeToFile(name, true);
}



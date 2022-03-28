#pragma once

#define PRODUCT_TYPE int
#define PRODUCT_TYPE_COUNT 10
#define POPULATION_SIZE 1000

class Agent
{
public:
	Agent(bool influencer, int id);
	~Agent();

public:
	float			produce();
	void			consume();
	float			satisfaction() { return m_satisfaction; }
	void			updateSatisfaction();
	float			consumption();
	float			money() { return m_money; }
	PRODUCT_TYPE	produceType() { return m_producerType; }
	void			chooseRandomType();
	void			chooseSpecificType(PRODUCT_TYPE type);
	float			lastSalary() { return m_lastSalary; }
	void			resetLastSalary() { m_lastSalary = 0; }
	vecFloat		storesToConsume() { return m_storesToConsume; }

	float			storesToSell() { return m_storesToSell; }
	void			setStoresToSell(const float f) { m_storesToSell = 0; }

	float			calculateSellingPrice(const vecFloat& prices, bool bVirtual = false);
	void			calculateBuyingPrice(const vecFloat& prices);
	
	float			sellingPrice() const { return m_sellingPrice; }
	float			sellingPriceDiff() const { return m_sellingPriceDiff; }
	void			setBuyingPrices(const vecFloat& prices);
	void			setSellingPrices(const vecFloat& prices);
	
	vecFloat		buyingPrices() { return m_buyingPrices; }
	float			buyingPrice(int type) const { return m_buyingPrices[type]; }

	float			productivity() { return m_productivity; } 
	void			setProductivity(const float productivity) { m_productivity = productivity; }
	float			rawMaterialAvailablity() { return m_rawMaterialAvailablity; }
	void			setRawMaterialAvailablity(const float rawMaterialAvailablity) { m_rawMaterialAvailablity = rawMaterialAvailablity; }

	void			buy(int type, float price);
	void			sell(float price);
	int				id() { return m_id; }
	float			sales() { return m_sales; }
	void			addMoney(const float& money) { m_money += money; }


	float			storesOfType(int type);

protected:

private:
	PRODUCT_TYPE	m_producerType;
	float			m_money;
	float			m_consumption; // a value between 0 and 1, represents how much they consumed last cycle
	float			m_productivity; // a value between 0.6 and 1, represents how much an agent can produce
	float			m_rawMaterialAvailablity;
	float			m_satisfaction; // a value between 0 and 1, represents how much they earn compared to other careers
	vecFloat		m_storesToConsume; // kgs of food to consume of each type
	vecFloat		m_buyingPrices;
	float			m_storesToSell; // kgs of food to sell

	float			m_lastSalary;
	int				m_id;
	float			m_sellingPrice;
	float			m_sellingPriceDiff;
	float			m_sales;
};


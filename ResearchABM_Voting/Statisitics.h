#pragma once
class Statisitics
{
private:
	Statisitics();
	// Stop the compiler generating methods of copy the object
	Statisitics(Statisitics const& copy);            // Not Implemented
	Statisitics& operator=(Statisitics const& copy); // Not Implemented

public:
	static Statisitics& getInstance()
	{
		// The only instance
		static Statisitics instance;
		return instance;
	}

};


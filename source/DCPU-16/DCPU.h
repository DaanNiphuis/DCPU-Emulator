// Author: Daan Niphuis

#ifndef DCPU_H
#define DCPU_H

#include <stdint.h>

class DCPU
{
public:
	DCPU();
	~DCPU();

	void loadProgram(const char* fileName);
	void start();

private:
	union Word
	{
		inline Word(): i(0) {}
		inline Word(uint16_t i): i(i) {}
		
		uint16_t i;
		struct 
		{
			char lo;
			char hi;
		};
		struct 
		{
			uint16_t o : 4;
			uint16_t a : 6;
			uint16_t b : 6;
		};
	};

	Word& next();
	template <typename T> T getValue(uint16_t location);
	static Word size(const Word& instruction);	

	Word registers[8]; // A, B, C, X, Y, Z, I, J
	Word pc; // Program counter
	Word sp; // Stack pointer
	Word o;  // overflow
	Word ram[0x10000];
	Word buf;
};

#endif
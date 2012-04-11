// Author: Daan Niphuis

#ifndef DCPU_H
#define DCPU_H

#include <stdint.h>

class DCPU
{
public:
	DCPU();
	~DCPU();

	// Copies program from disk into memory.
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
			char hi;
			char lo;
		};
		struct 
		{
			uint16_t o : 4;
			uint16_t a : 6;
			uint16_t b : 6;
		};
	};

	// Reads the word at the location of the program counter. And then increases the pc by one.
	Word& next();
	// Convert the location to the actual value. For example 0x0 is the value in register A
 	Word& getValue(uint16_t location);
 	// Skip the next instruction
	void skip();
	// Returns true if the value reads the next instruction.
	static bool readsNext(uint16_t location);
	
	Word registers[8]; // A, B, C, X, Y, Z, I, J
	Word pc; // Program counter
	Word sp; // Stack pointer
	Word o;  // overflow
	Word ram[0x10000];
	Word buf; // Used for returning literal values.
};

#endif
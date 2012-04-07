// Author: Daan Niphuis

#include "DCPU.h"

#include <iostream>
#include <string>

void main()
{
	// Read program into ram
	std::cout << ">";
	std::string programName;
	std::cin >> programName;
	
	DCPU cpu;
	cpu.loadProgram(programName.c_str());
	cpu.start();	
}
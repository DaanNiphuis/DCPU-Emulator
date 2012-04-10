// Author: Daan Niphuis

#include "DCPU.h"

#include <fstream>

const uint16_t NON_BASIC = 0x0;
const uint16_t SET = 0x1;
const uint16_t ADD = 0x2;
const uint16_t SUB = 0x3;
const uint16_t MUL = 0x4;
const uint16_t DIV = 0x5;
const uint16_t MOD = 0x6;
const uint16_t SHL = 0x7;
const uint16_t SHR = 0x8;
const uint16_t AND = 0x9;
const uint16_t BOR = 0xA;
const uint16_t XOR = 0xB;
const uint16_t IFE = 0xC;
const uint16_t IFN = 0xD;
const uint16_t IFG = 0xE;
const uint16_t IFB = 0xF;
const uint16_t JSR = 0x1;

const uint16_t A = 0x0;
const uint16_t B = 0x1;
const uint16_t C = 0x2;
const uint16_t X = 0x3;
const uint16_t Y = 0x4;
const uint16_t Z = 0x5;
const uint16_t I = 0x6;
const uint16_t J = 0x7;
const uint16_t VAL_A = 0x8;
const uint16_t VAL_B = 0x9;
const uint16_t VAL_C = 0xA;
const uint16_t VAL_X = 0xB;
const uint16_t VAL_Y = 0xC;
const uint16_t VAL_Z = 0xD;
const uint16_t VAL_I = 0xE;
const uint16_t VAL_J = 0xF;
const uint16_t VAL_NEXT_A = 0x10;
const uint16_t VAL_NEXT_B = 0x11;
const uint16_t VAL_NEXT_C = 0x12;
const uint16_t VAL_NEXT_X = 0x13;
const uint16_t VAL_NEXT_Y = 0x14;
const uint16_t VAL_NEXT_Z = 0x15;
const uint16_t VAL_NEXT_I = 0x16;
const uint16_t VAL_NEXT_J = 0x17;
const uint16_t POP = 0x18;
const uint16_t PEEK = 0x19;
const uint16_t PUSH = 0x1A;
const uint16_t SP = 0x1B;
const uint16_t PC = 0x1C;
const uint16_t O = 0x1D;
const uint16_t VAL_NEXT = 0x1E;
const uint16_t NEXT = 0x1F;
// 0x20-0x3f: literal value 0x00-0x1f (literal)

DCPU::DCPU()
{
}

DCPU::~DCPU()
{
}

void DCPU::loadProgram(const char* fileName)
{
	std::ifstream file(fileName, std::ios_base::binary);
	
	unsigned int i = 0;
	while (file.good())
	{
		file.get(ram[i].hi);
		file.get(ram[i].lo);
		++i;
	}
	file.close();
}

void DCPU::start()
{
	for (;;)
	{
		const Word& instruction = next();

		uint16_t& a = getValue(instruction.a).i;
		const uint16_t b = getValue(instruction.b).i;

		switch (instruction.o)
		{
		case NON_BASIC: 
			switch (instruction.a)
			{
			case JSR:
				ram[--sp.i] = pc.i;
				pc = b;
				break;
			}
			break;
		case SET: a = b; break;
		case ADD: 
			a += b;
			o = int32_t(a) + int32_t(b) > 0xFFFF ? 1 : 0;
			break;
		case SUB: 
			a -= b;
			o = int32_t(a) - int32_t(b) < 0 ? 1 : 0;
			break;
		case MUL:
			a *= b;
			o = ((uint32_t(a) * uint32_t(b))>>16)&0xFFFF;
			break;
		case DIV:
			a /= b;
			o = ((uint32_t(a)<<16) / uint32_t(b))&0xFFFF;
			break;
		case MOD:
			if (b == 0)
				a = 0;
			else
				a %= b;
		case SHL:
			a <<= b;
			o = ((uint32_t(a)<<uint32_t(b))>>16)&0xFFFF;
			break;
		case SHR:
			a >>= b;
			o = ((uint32_t(a)<<16)>>uint32_t(b))&0xFFFF;
			break;
		case AND: a &= b; break;
		case BOR: a |= b; break;
		case XOR: a ^= b; break;
		case IFE: if (a != b)     pc.i += size(next()); break;
		case IFN: if (a == b)     pc.i += size(next()); break;
		case IFG: if (a <= b)     pc.i += size(next()); break;
		case IFB: if ((a & b)==0) pc.i += size(next()); break;
		}
	}
}

DCPU::Word& DCPU::next()
{
	return ram[pc.i++];
}

DCPU::Word& DCPU::getValue(uint16_t location)
{
	switch (location)
	{
	case A:
	case B:
	case C:
	case X:
	case Y:
	case Z:
	case I:
	case J:	return registers[location];
	case VAL_A:
	case VAL_B:
	case VAL_C:
	case VAL_X:
	case VAL_Y:
	case VAL_Z:
	case VAL_I:
	case VAL_J: return ram[registers[location-VAL_A].i];
	case VAL_NEXT_A:
	case VAL_NEXT_B:
	case VAL_NEXT_C:
	case VAL_NEXT_X:
	case VAL_NEXT_Y:
	case VAL_NEXT_Z:
	case VAL_NEXT_I:
	case VAL_NEXT_J: return ram[registers[location-VAL_NEXT_A].i + next().i];
	case POP:		return ram[sp.i++];
	case PEEK:		return ram[sp.i];
	case PUSH:		return ram[--sp.i];
	case SP:		return sp;
	case PC:		return pc;
	case O:			return o;
	case VAL_NEXT:	return ram[next().i];
	case NEXT:		return next();
	default: 
		buf = location - 0x20; 
		return buf;
	}
}

uint16_t DCPU::size(const Word& instruction)
{
	uint16_t size = 0;
	if (usesNext(instruction.a))
		++size;
	if (usesNext(instruction.b))
		++size;
	return size;
}

bool DCPU::usesNext(uint16_t value)
{
	return (value >= VAL_NEXT_A && value <= VAL_NEXT_J) || value == VAL_NEXT || value == NEXT;
}

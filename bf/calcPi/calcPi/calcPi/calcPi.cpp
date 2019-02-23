#include <stdio.h>
#include <stdlib.h>

#include "stdafx.h"

#include <stdint.h>
#include <string.h>

class Memory {
public:
	Memory()
	{
		memset(m_mem, 0, sizeof(m_mem));
		memPtr = 0;
	}

	Memory& operator += (uint16_t ptr)
	{
		memPtr += ptr;
		return *this;
	}

	Memory& operator -= (uint16_t ptr)
	{
		memPtr -= ptr;
		return *this;
	}

	uint16_t& operator [] (uint16_t ptr)
	{
		return this->m_mem[ptr];
	}

	Memory& operator = (uint16_t ptr)
	{
		memPtr = ptr;
		return *this;
	}

	/*uint16_t * operator + (uint16_t ptr)
	{
		uint16_t currentMem = memPtr + ptr;
		return m_mem + currentMem;
	}

	uint16_t * operator - (uint16_t ptr)
	{
		uint16_t currentMem = memPtr - ptr;
		return m_mem + currentMem;
	}*/

	uint16_t & operator * ()
	{
		return m_mem[memPtr];
	}

	Memory& operator ++ ()
	{
		memPtr++;
		return *this;
	}

	Memory& operator -- ()
	{
		memPtr--;
		return *this;
	}


private:
	uint16_t memPtr;

	uint16_t m_mem[65536];
};

/*



*(mem+dst) += *mem
*/
void addMov(Memory &mem, uint16_t dstPos)
{
	while (*mem)
	{
		mem += dstPos;
		(*mem)++;
		mem -= dstPos;
		(*mem)--;	
	}
}

/*

*(mem+srcNew) = *mem
*(mem+dst) += *mem
*/

void addCpy(Memory &mem, uint16_t dstPos, uint16_t srcNew)
{
	while (*mem)
	{
		mem += dstPos;
		(*mem)++;
		mem -= dstPos;
		(*mem)--;
		mem += srcNew;
		(*mem)++;
		mem -= srcNew;
	}
}


/*

*mem -= *(mem + srcPos)

*/

void subMov(Memory &mem, uint16_t srcPos)
{
	mem += srcPos;
	while (*mem)
	{
		mem -= srcPos;
		(*mem)--;
		if (!*mem)
		{
			return;
		}
		mem += srcPos;
		(*mem)--;
	}
	mem -= srcPos;
}

/*

*(mem+subNew) = *(mem + srcPos)
*mem -= *(mem + srcPos)

*/
void subCpy(Memory &mem, uint16_t srcPos, uint16_t subNew)
{
	mem += srcPos;
	while (*mem)
	{
		(*mem)--;
		mem -= srcPos;
		mem += subNew;
		(*mem)++;
		mem -= subNew;
		(*mem)--;
		if (!*mem)
		{
			return;
		}

		mem += srcPos;

	}
	mem -= srcPos;
}

void nullCell(Memory &mem)
{
	while (*mem)
	{
		(*mem)--;
	}
}

/*mem = mem + src * mul*/
void mulConst(Memory &mem, uint16_t srcPos, uint16_t mulConst)
{
	mem += srcPos;
	while (*mem)
	{
		(*mem)--;
		mem -= srcPos;
		(*mem) += mulConst;
		mem += srcPos;
	}
	mem -= srcPos;
}


/*

*(mem+resPos) = *mem / *(mem+delPos)
*mem = *mem % *(mem+delPos)

*/
void divide(Memory &mem, uint16_t delPos, uint16_t resPos, uint16_t tmpPos)
{
	mem += resPos;
	nullCell(mem);
	mem -= resPos;
	while (*mem)
	{		
		mem += delPos;
		//addMov(mem, tmpPos);
		while (*mem)
		{
			mem += tmpPos;
			(*mem)++;
			mem -= tmpPos;
			(*mem)--;
		}
		mem -= delPos;
		subCpy(mem, tmpPos + delPos, delPos);
		mem += delPos;
		mem += tmpPos;
		if (*mem)
		{
			//addMov(mem, -(tmpPos+ delPos));
			while (*mem)
			{
				mem -= (tmpPos + delPos);
				(*mem)++;
				mem += (tmpPos + delPos);
				(*mem)--;
			}
			mem -= (delPos + tmpPos);
			return;
		}
		mem -= (delPos + tmpPos);
		mem += resPos;
		(*mem)++;
		mem -= resPos;
	}
}


void calcPi()
{

	Memory mem;

	mem[0] = 355;
	mem[1] = 113;

	mem += 1;
	addCpy(mem, 1, 2);
	mem -= 1;
	divide(mem, 2, 1, 4);

	mem += 1;
	fprintf(stderr, "%d.", *mem);
	mem +=1;
	
	mem -= 2;//mem == -1
	*mem = 6;
while (*mem)
	{
		(*mem)--;
		mem += 1;//mem == 1
		//nullCell(mem);
		while (*mem)
		{
			(*mem)--;
		}
		mem += 1;

		//mulConst(mem, 1, 10);
		while (*mem)
		{
			(*mem)--;
			mem -= 1;
			(*mem) += 10;
			mem += 1;
		}
		mem -= 1;

		mem += 2;//mem == 3
		addCpy(mem, 1, 2);

		mem -= 2;//mem == 1
		divide(mem, 4, 5, 6);

		mem += 4;

		//addMov(mem, -3);
		while (*mem)
		{
			mem -= 3;
			(*mem)++;
			mem += 3;
			(*mem)--;
		}

		mem += 1;//mem == 6
		fprintf(stderr, "%d", *mem);// mem ==6

		mem += 1;//mem == 7
		
		//nullCell(mem);
		while (*mem)
		{
			(*mem)--;
		}

		mem -= 3; // mem == 4
		//addMov(mem, -1);
		while (*mem)
		{
			mem -= 1;
			(*mem)++;
			mem += 1;
			(*mem)--;
		}
		
		mem -= 4;// mem == 0
	}

	
	fprintf(stderr, "\n");

}


void testAddMov(uint16_t a, uint16_t b)
{
	Memory Mem;

	Mem[5] = a;
	Mem[6] = b;
	Mem = 5;

	addMov(Mem, 1);
	++Mem;
	fprintf(stderr, "addMov(%d+%d) %s, wait %d, got %d\n", a, b, (*Mem != (a + b)) ? "incorrect" : "correct", a + b, *Mem);
}


void testAddCpy(uint16_t a, uint16_t b)
{
	Memory Mem;

	Mem[5] = a;
	Mem[6] = b;
	Mem = 5;
	addCpy(Mem, 1, 2);

	uint16_t res = *(++Mem);
	uint16_t cpy = *(++Mem);
	fprintf(stderr, "addCpy(%d+%d) %s, wait %d, got %d - %d\n", a, b, 
		((res != (a + b)) || (cpy != a)) ? "incorrect" : "correct", a + b, res, cpy);
}

void testSubMov(uint16_t a, uint16_t b)
{
	Memory Mem;

	Mem[5] = a;
	Mem[6] = b;
	Mem = 5;

	subMov(Mem, 1);
	fprintf(stderr, "subMov(%d-%d) %s, wait %d, got %d\n", a, b, (Mem[5] != (a - b)) ? "incorrect" : "correct", a - b, Mem[5]);
}

void testSubCpy(uint16_t a, uint16_t b)
{
	Memory Mem;

	Mem[5] = a;
	Mem[6] = b;
	Mem = 5;

	subCpy(Mem, 1, 2);
	fprintf(stderr, "subCpy(%d-%d) %s, wait %d, got %d - %d\n", a, b, 
		((Mem[5] != (a - b)) || (Mem[7] != b)) ? "incorrect" : "correct", a - b, Mem[5], Mem[7]);
}

void testMulConst(uint16_t a, uint16_t b)
{
	Memory Mem;

	Mem[5] = a;
	Mem = 6;

	mulConst(Mem, -1,  b);
	fprintf(stderr, "mulConst(%d*%d) %s, wait %d, got %d\n", a, b, (Mem[6] != (a * b)) ? "incorrect" : "correct", a * b, Mem[6]);
}


void testDiv(uint16_t a, uint16_t b)
{
	Memory Mem;

	Mem[5] = a;
	Mem[6] = b;
	Mem = 5;
	
	divide(Mem, 6 - 5, 7 - 5, 8-5);
	
	fprintf(stderr, "divide(%d/%d) %s, wait %d (%d), got %d(%d)\n", a, b, 
		((Mem[7] != (a / b)) || (Mem[6] != (a % b)))? "incorrect" : "correct", a /b, a % b, Mem[7], Mem[6]);
}

int main(void) {

	testAddMov(355,113);
	testAddCpy(355, 113);
	testSubMov(16, 113);
	testSubCpy(16, 113);
	testSubMov(355, 113);
	testSubCpy(355, 113);
	testMulConst(355,113);
	testDiv(355, 113);
	calcPi();
	
	return 0;
}

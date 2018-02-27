/* 
* ExternalRam.cpp
*
* Created: 29.05.2017 22:42:28
* Author: radiolok
*/


#include "ExternalRam.h"

uint8_t ramLockAccess = 0;

uint8_t ramInit(void)
{

	DDRE |= (1 << PE2) | (1 << PE3);
	PORTE |= ((1 << PE2) | (1 << PE3));
	DDRG |= (1 << PG1);
	
	DDRJ |= (1<< PJ4);//CS
	PORTJ &= ~(1 << PJ4);
		log_trace("ramInit Done");
	return 0;
}

int16_t ramReadWord(uint16_t addr)
{
	int16_t result = -1;
	if (!ramCheckLock())
	{
		ramSetMcuAddrPortWrite();
		ramSetAddress(addr);
		ramSetCs();
		ramSetOE();
		_delay_us(2);
		result = ramGetData();
		_delay_us(2);
		ramClrCs();
		ramReleaseOE();
	}
	ramReleaseLine();		
	return result;
}

int16_t ramWriteWord(uint16_t addr, uint16_t data)
{
	int16_t result = -1;
	if (!ramCheckLock())
	{
		ramSetMcuAddrPortWrite();
		ramSetMcuDataPortWrite();
		ramSetAddress(addr);
		ramSetData(data);
		ramSetWrite();
		ramSetCs();
		_delay_us(2);
		ramClrCs();
		ramSetRead();
	}
	ramReleaseLine();
	return result;
}

int16_t ramDataToBus(uint16_t addr)
{
	int16_t result = -1;
	if (!ramCheckLock())
	{
		ramSetMcuAddrPortWrite();
		ramSetAddress(addr);
		ramSetOE();
		ramSetCs();
		result = 0;
	}
	return result;
}

int16_t ramDataFromBus(uint16_t addr)
{
	int16_t result = -1;
	if (!ramCheckLock())
	{
		ramSetMcuAddrPortWrite();
		ramSetAddress(addr);
		ramSetWrite();
		ramSetCs();
		_delay_us(2);
		result = 0;
	}
	return result;
}

void ramLock(void)
{
	ramLockAccess = 1;
	ramReleaseLine();
}

void ramUnlock(void)
{
	ramLockAccess = 0;
}

uint8_t ramCheckLock(void)
{
	//dbg_trace_val("RamLock=",ramLockAccess);
	return ramLockAccess;
}
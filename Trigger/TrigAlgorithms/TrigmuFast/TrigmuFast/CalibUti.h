/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBUTI_H
#define CALIBUTI_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

namespace LVL2CALIBRATION
{

void printWord(std::ostream&, uint32_t);
std::string hexDump(uint8_t*, uint16_t);

bool CalibDataLoader(std::ifstream&,uint8_t*,uint16_t);

void copyChars(const uint8_t*, uint8_t* ptr, uint16_t);
uint32_t wordFromLitEndChars(const uint8_t*);
uint32_t wordFromBigEndChars(const uint8_t*);
void charsFromLitEndWord(const uint32_t, uint8_t*);
void charsFromBigEndWord(const uint32_t, uint8_t*);

uint32_t floatIntoWord(float value);
float  wordIntoFloat(uint32_t word);

uint32_t bit16IntoWord(const uint16_t,const uint16_t);
void wordIntoBit16(const uint32_t, uint16_t&,uint16_t&);

void   IPfromLong    (const uint32_t, char*);
void   IPfromChar    (const char* ,uint32_t&);
char** local_address (int*);
std::string fill_ip  (void);

uint32_t TStoWords(void);


bool checkLim(const int,int[2]);
uint32_t MdtIdIntoWord(const int, const int, const int, const int, const int, 
                       const int);
uint32_t RpcIdIntoWord(const int, const int, const int, const int, const int, 
                       const int, const int, const int, const int);
uint32_t TgcIdIntoWord(const int, const int, const int, const int, const int,
                       const int );
uint32_t CscIdIntoWord(const int, const int, const int, const int, const int, 
                       const int, const int);
		       
		       
void WordIntoMdtId(uint32_t, int&, int&, int&, int&, int&, int&);
void WordIntoRpcId(uint32_t, int&, int&, int&, int&, int&, int&,int&,int&,int&);
void WordIntoTgcId(uint32_t, int&, int&, int&, int&, int&, int&); 
void WordIntoCscId(uint32_t, int&, int&, int&, int&, int&, int&, int&);


bool RpcHitIntoBit16 (const uint16_t,const uint16_t,const uint16_t,
                      const uint16_t,const uint16_t,const uint16_t,
			    uint16_t&);

void Bit16IntoRpcHit (const uint16_t,uint16_t&,uint16_t&,uint16_t&,
                                     uint16_t&,uint16_t&,uint16_t&);
  

}   // end of namespace LVL2CALIBRATION


#endif

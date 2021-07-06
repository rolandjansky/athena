// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoCommon/Exception.h"

#include <iostream>
#include <iomanip>

using namespace std;

TCS::BaseTOB::BaseTOB(uint32_t roiWord, const std::string& tobName) :
  m_roiWord(roiWord),
  m_tobName(tobName)
{}

TCS::BaseTOB::~BaseTOB() = default;

int
TCS::BaseTOB::sizeCheckM(int value, unsigned int size) const
{
   int max(0x1 << (size-1));
   int min(~max + 1);
   --max;
   if( (-value)>max || (-value)<min) {
     TCS_EXCEPTION("Integer value " << -value << " outside firmware specifications. Maximum number of bits is " << size << " -> range ["<<min<<" - "<<max<<"]" << ", for " << m_tobName);
   }
   return value;
}


int
TCS::BaseTOB::sizeCheck(int value, unsigned int size) const
{
   int max(0x1 << (size-1));
   int min(~max + 1);
   --max;
   if(value>max || value<min) {
      TCS_EXCEPTION("Integer value " << value << " outside firmware specifications. Maximum number of bits is " << size << " -> range ["<<min<<" - "<<max<<"]" << ", for " << m_tobName);
   }
   return value;
}

unsigned int
TCS::BaseTOB::sizeCheck(unsigned int value, unsigned int size) const
{
   unsigned int max( (0x1 << size)-1 );
   if(value>max) {
      TCS_EXCEPTION("Unsigned integer value " << value << " outside firmware specifications. Maximum number of bits is " << size << " -> range [0"<<" - "<<max<<"]" << ", for " << m_tobName);
   }
   return value;
}


namespace TCS {


std::ostream & operator<< (std::ostream & o, const TCS::BaseTOB& top) {
    top.print(o);
    return o;
}


}

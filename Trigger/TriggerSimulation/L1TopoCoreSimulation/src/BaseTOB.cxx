/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  BaseTOB.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#include "L1TopoCoreSimulation/BaseTOB.h"
#include "L1TopoCoreSimulation/Exception.h"

#include <iostream>
#include <iomanip>

using namespace std;

int
TCS::BaseTOB::sizeCheck(int value, unsigned int size) {
   int max(0x1 << (size-1));
   int min(~max + 1);
   --max;
   if(value>max || value<min) {
      TCS_EXCEPTION("Value " << value << " outside firmware specifications. Maximum number of bits is " << size << " -> range ["<<min<<" - "<<max<<"]");
   }
   return value;
}

std::ostream & operator<< (std::ostream & o, const TCS::BaseTOB& top) {
    top.print(o);
    return o;
}

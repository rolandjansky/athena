/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <vector>

#include "HLTRates2COOL/HLTCountCoolWriter.h"
#include "HLTRates2COOL/HLTCounter.h"

int main()
{
   //cool connection

   std::string coolConnection="sqlite://;schema=testhltcounters.db;dbname=COMP200";

   std::cout << "Creating coolWriter("<<coolConnection<<")" << std::endl;  
   hltca::HLTCountCoolWriter coolWriter(coolConnection);

   bool print=true;
   coolWriter.createSchema(print);
  
 
   std::vector<hltca::HLTCounter> counters;

   std::cout << std::endl << "Test writing ! " << std::endl;

   for(unsigned int i=1; i<20; i++) {
      counters.push_back( hltca::HLTCounter(i,(10*i),i,(2*i),(3*i),hltca::HLTCounter::L2) );
      counters.push_back( hltca::HLTCounter(i,(10*i+3),i,(2*i),(3*i),hltca::HLTCounter::EF) );
   }
  
   coolWriter.writeHLTCountersPayload((unsigned int)100,(unsigned int)1,counters);
  
   return 0;
}

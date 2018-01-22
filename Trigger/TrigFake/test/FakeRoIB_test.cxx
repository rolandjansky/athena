/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "CTPfragment/CTPdataformatVersion.h"
#include "../src/FakeRoIB.h"
using namespace ROIB;
int main() {

  std::vector<uint32_t> tbp(16);
  tbp[0] = 0x11; // just not to be all 0 in TBP
  std::vector<uint32_t> tav(16);
  tav[0] = 0x11; // just not to be all 0 in TAV
  CTPdataformatVersion format(5); // we will use version 5 
  CTPResult r = FakeRoIBHelper::buildCTPFragment( tbp, tav, 0x55 ); // 0x55 is event number 
  std::cout << "time words: " << format.getNumberTimeWords() << " RoIB words per bunch " << format.getRoIBwordsPerBunch() << " " << format.getDAQwordsPerBunch() << std::endl;
  std::cout << " header size " << r.header().size() << " trailer size " << r.trailer().size()  <<  " CTP RoI " << r.roIVec().size() << std::endl;


  std::cout << std::endl;
  std::cout << r.print(true);
  std::cout << std::endl;
  
  
  if ( not r.isComplete() ) {
    std::cout << "CTP Result is not complete \n" ;
    return 1;    
  }

  return 0;
}

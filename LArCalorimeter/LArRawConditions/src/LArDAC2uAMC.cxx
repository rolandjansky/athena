/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArDAC2uAMC.h"

LArDAC2uAMC::LArDAC2uAMC() :LArDAC2uAComplete()
{}

StatusCode LArDAC2uAMC::initialize () {   
  return (CONTAINER::initialize()) ;
}

LArDAC2uAMC::~LArDAC2uAMC() {}


/* retrieve DAC2uA ******************************************************
 */
const float& LArDAC2uAMC::DAC2UA(const HWIdentifier& ) const 
{ 
  std::cout << "LArDACuAMC is deprecated. Use LArDACuASym!" << std::endl;
  std::abort();
  return dummy;
}


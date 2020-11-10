/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasMC.h" 

LArMinBiasMC::LArMinBiasMC()  :LArMinBiasComplete()
{}

StatusCode LArMinBiasMC::initialize() {
   return (CONTAINER::initialize()) ;
}

LArMinBiasMC::~LArMinBiasMC() {}


/* retrieve RMS of E in Minimum Bias events ******************************
 */
const float& LArMinBiasMC::minBiasRMS(const HWIdentifier&) const 
{ 
  std::cout << "LArMinBiasMC is deprecated. Use LArMinBiasSym!" << std::endl;
  std::abort();
}

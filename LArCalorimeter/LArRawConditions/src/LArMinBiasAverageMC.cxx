/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasAverageMC.h" 

float LArMinBiasAverageMC::dummy = ERRORCODE;

LArMinBiasAverageMC::LArMinBiasAverageMC()  :LArMinBiasAverageComplete()
{}

StatusCode LArMinBiasAverageMC::initialize() {
  return (CONTAINER::initialize()) ;
}

LArMinBiasAverageMC::~LArMinBiasAverageMC() {}


/* retrieve Average of E in Minimum Bias events ******************************
 */
const float& LArMinBiasAverageMC::minBiasAverage(const HWIdentifier&) const { 
  
  std::cout << "LArMinBiasAverageMC is deprecated. Use LArMinBiasAverageSym" << std::endl;
  std::abort();
  return LArMinBiasAverageMC::dummy;
}


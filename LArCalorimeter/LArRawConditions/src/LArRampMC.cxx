/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArRampMC.h" 

LArRampMC::LArRampMC() : LArRampComplete()
{
} 
StatusCode LArRampMC::initialize(){
  return (LArRampCompleteBase::initialize()) ; 
}

LArRampMC::~LArRampMC() {}

/* retrieve Ramp ******************************************************
 */
LArRampMC::RampRef_t LArRampMC::ADC2DAC(const HWIdentifier&, int) const { 

  std::cout << "LArRampMC is deprecated. Use LArRampSym" << std::endl;
  std::abort();
  static ILArRamp::RampRef_t dummy;
  return dummy;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "LArRawConditions/LArNoiseMC.h"


LArNoiseMC::LArNoiseMC()  : LArNoiseComplete()
{} 

StatusCode LArNoiseMC::initialize()
{
  return (CONTAINER::initialize()) ;
}

LArNoiseMC::~LArNoiseMC() {}


/* retrieve Noise ******************************************************
 */
const float& LArNoiseMC::noise(const HWIdentifier&, int) const 
{ 
  std::cout << "LArNoiseMC is deprecated. Use LArNoiseSym" << std::endl;
  std::abort();
}


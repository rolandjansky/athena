/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArfSamplMC.h" 

LArfSamplMC::LArfSamplMC() : LArfSamplComplete()
{}


StatusCode LArfSamplMC::initialize() {   
  return (CONTAINER::initialize());   
}

LArfSamplMC::~LArfSamplMC() {}


/* retrieve fSampl ******************************************************
 */
const float& LArfSamplMC::FSAMPL(const HWIdentifier&) const 
{
  std::cout << "LArfSampleMC is deprecated. Use LArfSampleSym" << std::endl;
  std::abort();
  return LArfSamplMC::dummy;
}

const float& LArfSamplMC::FSAMPL(const Identifier&) const
{
  std::cout << "LArfSampleMC is deprecated. Use LArfSampleSym" << std::endl;
  std::abort();
  return LArfSamplMC::dummy;
}


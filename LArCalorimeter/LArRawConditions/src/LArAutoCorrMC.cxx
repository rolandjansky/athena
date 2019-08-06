/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArAutoCorrMC.h" 

LArAutoCorrMC::LArAutoCorrMC() :LArAutoCorrComplete()
{}

StatusCode LArAutoCorrMC::initialize()  { 
  return (CONTAINER::initialize()) ; 
}

LArAutoCorrMC::~LArAutoCorrMC() {}


/* retrieve AutoCorr ******************************************************
 */
ILArAutoCorr::AutoCorrRef_t LArAutoCorrMC::autoCorr(const HWIdentifier&, int) const { 
  std::cout << "LArAutoCorrMC is deprecated. Use LArAutoCorrSym" << std::endl;
  std::abort();
  static ILArAutoCorr::AutoCorrRef_t dummy;
  return dummy;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMphysOverMcalMC.h" 

//float LArMphysOverMcalMC::dummy=LArElecCalib::ERRORCODE;

LArMphysOverMcalMC::LArMphysOverMcalMC()  
:LArMphysOverMcalComplete()
{} 

StatusCode LArMphysOverMcalMC::initialize() {    
  return (CONTAINER::initialize()) ;
}

LArMphysOverMcalMC::~LArMphysOverMcalMC() {}


/* retrieve MphysOverMcal ******************************************************
 */
const float& LArMphysOverMcalMC::MphysOverMcal(const HWIdentifier&, int) const 
{ 
  std::cout << "LArMphysOverMcalMC is deprecated. Use LArMphysOverMcalSym" << std::endl;
  return dummy;
}


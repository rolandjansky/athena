/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "LArRawConditions/LAruA2MeVMC.h" 

#include <iostream> 
using namespace std ;

LAruA2MeVMC::LAruA2MeVMC() : LAruA2MeVComplete()
{ }

StatusCode LAruA2MeVMC::initialize(){   
  return (CONTAINER::initialize()); 
}

LAruA2MeVMC::~LAruA2MeVMC() {}


/* retrieve uA2MeV ******************************************************
 */
const float& LAruA2MeVMC::UA2MEV(const HWIdentifier&) const  { 
  std::cout << "LAruA2MeVMC is deprecated! Use LAruA2MeVSym" << std::endl;
  std::abort();
  return dummy;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoIAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMRoIAuxContainer_v1.h"
        
namespace xAOD {

  CPMRoIAuxContainer_v1::CPMRoIAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( roiWord ); 
  }
  
} // namespace xAOD

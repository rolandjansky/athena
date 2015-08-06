/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXRoIAuxContainer_v1.cxx 681007 2015-07-07 13:34:12Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXRoIAuxContainer_v1.h"
        
namespace xAOD {

  CMXRoIAuxContainer_v1::CMXRoIAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( cmxRoIWords );   
  }
  
} // namespace xAOD

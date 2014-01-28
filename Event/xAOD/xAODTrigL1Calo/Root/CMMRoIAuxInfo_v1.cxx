/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoIAuxInfo_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMRoIAuxInfo_v1.h"
        
namespace xAOD {

  CMMRoIAuxInfo_v1::CMMRoIAuxInfo_v1() :
    AuxInfoBase() 
  {
    AUX_VARIABLE( jetEtRoiWord );
    AUX_VARIABLE( energyRoiWord0 ); 
    AUX_VARIABLE( energyRoiWord1 ); 
    AUX_VARIABLE( energyRoiWord2 );     
  }
  
} // namespace xAOD

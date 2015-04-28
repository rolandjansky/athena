/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsAuxContainer_v2.cxx 643054 2015-01-30 13:55:12Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v2.h"
        
namespace xAOD {

  JEMEtSumsAuxContainer_v2::JEMEtSumsAuxContainer_v2() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( module );
    AUX_VARIABLE( peak ); 
    AUX_VARIABLE( etVec );
    AUX_VARIABLE( exVec );
    AUX_VARIABLE( eyVec );    
  }
  
} // namespace xAOD

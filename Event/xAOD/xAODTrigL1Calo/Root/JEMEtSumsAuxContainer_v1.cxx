/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v1.h"
        
namespace xAOD {

  JEMEtSumsAuxContainer_v1::JEMEtSumsAuxContainer_v1() :
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

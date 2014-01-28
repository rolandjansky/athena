/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMHitsAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMHitsAuxContainer_v1.h"
        
namespace xAOD {

  CPMHitsAuxContainer_v1::CPMHitsAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( module );
    AUX_VARIABLE( peak ); 
    AUX_VARIABLE( hitsVec0 );
    AUX_VARIABLE( hitsVec1 );    
  }
  
} // namespace xAOD

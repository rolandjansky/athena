/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMJetHitsAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMJetHitsAuxContainer_v1.h"
        
namespace xAOD {

  CMMJetHitsAuxContainer_v1::CMMJetHitsAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( dataID );
    AUX_VARIABLE( peak ); 
    AUX_VARIABLE( hitsVec ); 
    AUX_VARIABLE( errorVec );  
  }
  
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMEtSumsAuxContainer_v1.h"
        
namespace xAOD {

  CMMEtSumsAuxContainer_v1::CMMEtSumsAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( crate );
    AUX_VARIABLE( dataID );
    AUX_VARIABLE( peak ); 
    AUX_VARIABLE( etVec );
    AUX_VARIABLE( exVec );  
    AUX_VARIABLE( eyVec );
    AUX_VARIABLE( etErrorVec );
    AUX_VARIABLE( exErrorVec );  
    AUX_VARIABLE( eyErrorVec );   
  }
  
} // namespace xAOD

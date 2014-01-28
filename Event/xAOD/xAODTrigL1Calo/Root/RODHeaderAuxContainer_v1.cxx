/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v1.h"
        
namespace xAOD {

  RODHeaderAuxContainer_v1::RODHeaderAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( version );
    AUX_VARIABLE( sourceId );
    AUX_VARIABLE( run ); 
    AUX_VARIABLE( lvl1Id );
    AUX_VARIABLE( bcId );  
    AUX_VARIABLE( trigType );
    AUX_VARIABLE( detType );    
    AUX_VARIABLE( statusWords );  
    AUX_VARIABLE( payloadSize );    
  }  
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderAuxContainer_v2.cxx 694350 2015-09-11 12:02:19Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v2.h"
        
namespace xAOD {

  RODHeaderAuxContainer_v2::RODHeaderAuxContainer_v2() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( version );
    AUX_VARIABLE( sourceId );
    AUX_VARIABLE( run ); 
    AUX_VARIABLE( lvl1Id );
    AUX_VARIABLE( bcid );  
    AUX_VARIABLE( trigType );
    AUX_VARIABLE( lvl1DetType );    
    AUX_VARIABLE( statusWords );  
    AUX_VARIABLE( payloadSize );    
  }  
} // namespace xAOD

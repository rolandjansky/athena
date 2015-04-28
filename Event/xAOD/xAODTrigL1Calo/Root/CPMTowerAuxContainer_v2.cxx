/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerAuxContainer_v2.cxx 642658 2015-01-29 12:40:24Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v2.h"
        
namespace xAOD {

  CPMTowerAuxContainer_v2::CPMTowerAuxContainer_v2() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( emEnergyVec );
    AUX_VARIABLE( hadEnergyVec  );
    AUX_VARIABLE( emErrorVec  ); 
    AUX_VARIABLE( hadErrorVec  );
    AUX_VARIABLE( eta );
    AUX_VARIABLE( phi );
    AUX_VARIABLE( peak );    
  }
  
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerAuxContainer_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v1.h"
        
namespace xAOD {

  CPMTowerAuxContainer_v1::CPMTowerAuxContainer_v1() :
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

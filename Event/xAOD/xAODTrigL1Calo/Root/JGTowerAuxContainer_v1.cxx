/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// Local include(s):
#include "xAODTrigL1Calo/versions/JGTowerAuxContainer_v1.h"

namespace xAOD {

  JGTowerAuxContainer_v1::JGTowerAuxContainer_v1() :
    AuxContainerBase()
  {
    
    ///@name IParticle variables            
    AUX_VARIABLE( eta );           
    AUX_VARIABLE( phi );
    AUX_VARIABLE( deta );
    AUX_VARIABLE( dphi );
    AUX_VARIABLE( Id );
    AUX_VARIABLE( et );
    AUX_VARIABLE( SCIndex );
    AUX_VARIABLE( TileIndex );
    AUX_VARIABLE( sampling);
  }

} // namespace xAOD

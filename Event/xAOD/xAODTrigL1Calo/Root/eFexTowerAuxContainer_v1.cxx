/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigL1Calo/versions/eFexTowerAuxContainer_v1.h"

namespace xAOD {

  eFexTowerAuxContainer_v1::eFexTowerAuxContainer_v1() :
    AuxContainerBase()
    {
    
    AUX_VARIABLE( eta );           
    AUX_VARIABLE( phi );
    AUX_VARIABLE( module );
    AUX_VARIABLE( fpga );
    AUX_VARIABLE( et_count );
    AUX_VARIABLE( em_status );
    AUX_VARIABLE( had_status );
    }

} // namespace xAOD

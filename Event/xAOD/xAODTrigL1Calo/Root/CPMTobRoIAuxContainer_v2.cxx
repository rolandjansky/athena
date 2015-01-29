/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoIAuxContainer_v2.h"

namespace xAOD{

  CPMTobRoIAuxContainer_v2::CPMTobRoIAuxContainer_v2() :
    AuxContainerBase()
  {
    AUX_VARIABLE( roiWord );
  }

} // namespace xAOD


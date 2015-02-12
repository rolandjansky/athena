/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoIAuxContainer_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoIAuxContainer_v2.h"

namespace xAOD{

  CPMTobRoIAuxContainer_v2::CPMTobRoIAuxContainer_v2() :
    AuxContainerBase()
  {
    AUX_VARIABLE( roiWord );
  }

} // namespace xAOD


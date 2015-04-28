/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoIAuxContainer_v1.cxx 652818 2015-03-09 22:54:24Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoIAuxContainer_v1.h"

namespace xAOD{

  CPMTobRoIAuxContainer_v1::CPMTobRoIAuxContainer_v1() :
    AuxContainerBase()
  {
    AUX_VARIABLE( roiWord );
  }

} // namespace xAOD


// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoIContainer_v2.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOBROICONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOBROICONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoI_v2.h"

namespace xAOD{
  /// Define the CPMTobRoI as a simple DataVector
  typedef DataVector< xAOD::CPMTobRoI_v2 > CPMTobRoIContainer_v2;
}
#endif


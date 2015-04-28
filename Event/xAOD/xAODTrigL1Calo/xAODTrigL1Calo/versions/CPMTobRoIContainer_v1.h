// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoIContainer_v1.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOBROICONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOBROICONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoI_v1.h"

namespace xAOD{
  /// Define the CPMTobRoI as a simple DataVector
  typedef DataVector< xAOD::CPMTobRoI_v1 > CPMTobRoIContainer_v1;
}
#endif


// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoIContainer_v1.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMTOBROICONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMTOBROICONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMTobRoI_v1.h"

namespace xAOD{
  /// Define the JEMTobRoI as a simple DataVector
  typedef DataVector< xAOD::JEMTobRoI_v1 > JEMTobRoIContainer_v1;
}
#endif


// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPHitsContainer_v1.h 652809 2015-03-09 22:01:53Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXCPHITSCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXCPHITSCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPHits_v1.h"

namespace xAOD{
  /// Define the CMXCPHits as a simple DataVector
  typedef DataVector< xAOD::CMXCPHits_v1 > CMXCPHitsContainer_v1;
}
#endif


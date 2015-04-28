// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsContainer_v1.h 652810 2015-03-09 22:33:00Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXETSUMSCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXETSUMSCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSums_v1.h"

namespace xAOD{
  /// Define the CMXEtSums as a simple DataVector
  typedef DataVector< xAOD::CMXEtSums_v1 > CMXEtSumsContainer_v1;
}
#endif


// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSumsContainer_v2.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXETSUMSCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXETSUMSCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSums_v2.h"

namespace xAOD{
  /// Define the CMXEtSums as a simple DataVector
  typedef DataVector< xAOD::CMXEtSums_v2 > CMXEtSumsContainer_v2;
}
#endif


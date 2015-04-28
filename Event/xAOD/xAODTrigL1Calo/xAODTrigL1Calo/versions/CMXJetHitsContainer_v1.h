// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHitsContainer_v1.h 652810 2015-03-09 22:33:00Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETHITSCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETHITSCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetHits_v1.h"

namespace xAOD{
  /// Define the CMXJetHits as a simple DataVector
  typedef DataVector< xAOD::CMXJetHits_v1 > CMXJetHitsContainer_v1;
}
#endif


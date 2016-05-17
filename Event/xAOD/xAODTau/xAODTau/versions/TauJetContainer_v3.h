// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_VERSIONS_TAUCONTAINER_V3_H
#define XAODTAU_VERSIONS_TAUCONTAINER_V3_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTau/versions/TauJet_v3.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TauJet_v3 > TauJetContainer_v3;
}

#endif // XAODTAU_VERSIONS_TAUCONTAINER_V3_H

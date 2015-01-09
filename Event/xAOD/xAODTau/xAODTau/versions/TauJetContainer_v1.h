// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetContainer_v1.h 629816 2014-11-20 14:55:09Z janus $
#ifndef XAODTAU_VERSIONS_TAUCONTAINER_V1_H
#define XAODTAU_VERSIONS_TAUCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTau/versions/TauJet_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TauJet_v1 > TauJetContainer_v1;
}

#endif // XAODTAU_VERSIONS_TAUCONTAINER_V1_H

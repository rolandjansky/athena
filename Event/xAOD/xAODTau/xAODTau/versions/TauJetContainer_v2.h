// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetContainer_v2.h 796007 2017-02-07 15:38:04Z griffith $
#ifndef XAODTAU_VERSIONS_TAUCONTAINER_V2_H
#define XAODTAU_VERSIONS_TAUCONTAINER_V2_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTau/versions/TauJet_v2.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TauJet_v2 > TauJetContainer_v2;
}

#endif // XAODTAU_VERSIONS_TAUCONTAINER_V2_H

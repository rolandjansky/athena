// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetContainer_v1.h 569436 2013-11-08 17:00:41Z janus $
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

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TauJetContainer_v1, 1177172564, 1 )
#endif // XAOD_STANDALONE

#endif // XAODTAU_VERSIONS_TAUCONTAINER_V1_H

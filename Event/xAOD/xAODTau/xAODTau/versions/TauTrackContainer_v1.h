// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_VERSIONS_TAUTRACKCONTAINER_V1_H
#define XAODTAU_VERSIONS_TAUTRACKCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTau/versions/TauTrack_v1.h"

namespace xAOD {
   /// The container is a simple typedef for a vector of TauTracks
   typedef DataVector< xAOD::TauTrack_v1 > TauTrackContainer_v1;
}

#endif // XAODTAU_VERSIONS_TAUTRACKCONTAINER_V1_H

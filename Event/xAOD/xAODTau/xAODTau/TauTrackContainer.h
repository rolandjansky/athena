// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_TAUTRACKCONTAINER_H
#define XAODTAU_TAUTRACKCONTAINER_H

// Local include(s):
#include "xAODTau/TauTrack.h"
#include "xAODTau/versions/TauTrackContainer_v1.h"

namespace xAOD {
   /// Definition of the current TauTrack container version
   typedef TauTrackContainer_v1 TauTrackContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TauTrackContainer, 1306133771, 1 )

#endif // XAODTAU_TAUTRACKCONTAINER_H

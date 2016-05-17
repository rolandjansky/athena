// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_TAUTRACKAUXCONTAINER_H
#define XAODTAU_TAUTRACKAUXCONTAINER_H

// Local include(s):
#include "xAODTau/versions/TauTrackAuxContainer_v1.h"

namespace xAOD {
   typedef TauTrackAuxContainer_v1 TauTrackAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TauTrackAuxContainer , 1136575378, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTAU_TAUTRACKAUXCONTAINER_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetContainer.h 631748 2014-11-28 09:58:13Z janus $
#ifndef XAODTAU_TAUJETCONTAINER_H
#define XAODTAU_TAUJETCONTAINER_H

// Local include(s):
#include "xAODTau/TauJet.h"
#include "xAODTau/versions/TauJetContainer_v2.h"

namespace xAOD {
   /// Definition of the current "taujet container version"
   typedef TauJetContainer_v2 TauJetContainer;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TauJetContainer, 1177172564, 1 )
#endif // XAOD_STANDALONE

#endif // XAODTAU_TAUJETCONTAINER_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJetContainer.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAU_DITAUJETCONTAINER_H
#define XAODDITAU_DITAUJETCONTAINER_H

// Local include(s):
#include "xAODTau/DiTauJet.h"
#include "xAODTau/versions/DiTauJetContainer_v1.h"

namespace xAOD {
   /// Definition of the current DiTauJet container version
   typedef DiTauJetContainer_v1 DiTauJetContainer;
}

// Set up a CLID for the container:
// #ifndef XAOD_STANDALONE
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::DiTauJetContainer, 1281324766, 1 )
// #endif // XAOD_STANDALONE

#endif // XAODDITAU_DITAUJETCONTAINER_H
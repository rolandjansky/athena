// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetContainer.h 796007 2017-02-07 15:38:04Z griffith $
#ifndef XAODTAU_TAUJETCONTAINER_H
#define XAODTAU_TAUJETCONTAINER_H

// Local include(s):
#include "xAODTau/TauJet.h"
#include "xAODTau/versions/TauJetContainer_v3.h"

namespace xAOD {
   /// Definition of the current "taujet container version"
   typedef TauJetContainer_v3 TauJetContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TauJetContainer, 1177172564, 1 )

#endif // XAODTAU_TAUJETCONTAINER_H

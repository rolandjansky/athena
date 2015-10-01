// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJetContainer_v1.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAU_VERSIONS_DITAUCONTAINER_V1_H
#define XAODDITAU_VERSIONS_DITAUCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTau/versions/DiTauJet_v1.h"

namespace xAOD {
   /// The container is a simple typedef for a vector of DiTauJets
   typedef DataVector< xAOD::DiTauJet_v1 > DiTauJetContainer_v1;
}

#endif // XAODDITAU_VERSIONS_DITAUCONTAINER_V1_H
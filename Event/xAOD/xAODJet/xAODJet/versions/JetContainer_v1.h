// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_VERSIONS_JETCONTAINER_V1_H
#define XAODJET_VERSIONS_JETCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODJet/versions/Jet_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Jet_v1 > JetContainer_v1;
}

#endif // XAODJET_VERSIONS_JETCONTAINER_V1_H

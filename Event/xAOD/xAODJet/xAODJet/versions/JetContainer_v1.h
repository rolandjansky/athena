// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetContainer_v1.h 562740 2013-09-24 14:12:01Z krasznaa $
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

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::JetContainer_v1, 1244316195, 1 )
#endif // XAOD_STANDALONE

#endif // XAODJET_VERSIONS_JETCONTAINER_V1_H

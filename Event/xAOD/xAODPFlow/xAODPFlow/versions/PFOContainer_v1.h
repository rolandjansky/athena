// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOContainer_v1.h 582152 2014-02-07 13:09:21Z krasznaa $
#ifndef XAODPFLOW_VERSIONS_PFOCONTAINER_V1_H
#define XAODPFLOW_VERSIONS_PFOCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODPFlow/versions/PFO_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::PFO_v1 > PFOContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::PFOContainer_v1, 1177172577, 1 )
#endif // XAOD_STANDALONE

#endif // XAODPFLOW_VERSIONS_PFOCONTAINER_V1_H

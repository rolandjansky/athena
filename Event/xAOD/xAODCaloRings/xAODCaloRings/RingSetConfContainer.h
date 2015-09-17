/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfContainer.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_RINGSETCONFCONTAINER_H
#define XAODCALORINGS_RINGSETCONFCONTAINER_H

// STL include(s):
#include <vector>

// Core include(s):
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloRings/RingSetConf.h"
#include "xAODCaloRings/versions/RingSetConfContainer_v1.h"

namespace xAOD {
/// Definition of the current "RingSetConf container version"
typedef RingSetConfContainer_v1 RingSetConfContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetConfContainer > >
  RingSetConfELVec;
} // namespace xAOD

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::RingSetConfContainer , 1157997427 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODCALORINGS_RINGSETCONFCONTAINER_H



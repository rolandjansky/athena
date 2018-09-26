/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfContainer.h 707323 2015-11-12 02:45:01Z wsfreund $
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
  RingSetConfLinks;
// There is probably no need to use AthenaKernel/MetaCont since the parameters
// are not expected to change during the reconstruction, it is probably not
// needed. Example on: xAODTrigger/TriggerMenuContainer.h
} // namespace xAOD

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::RingSetConfContainer , 1157997427 , 1 )

#endif // XAODCALORINGS_RINGSETCONFCONTAINER_H



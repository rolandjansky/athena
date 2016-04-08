/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetContainer.h 707323 2015-11-12 02:45:01Z wsfreund $ 
#ifndef XAODCALORINGS_RINGSETCONTAINER_H
#define XAODCALORINGS_RINGSETCONTAINER_H

// STL include(s):
#include <vector>

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODCaloRings/versions/RingSetContainer_v1.h"

namespace xAOD {
/// Definition of the current "RingSet container version"
typedef RingSetContainer_v1 RingSetContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetContainer > >
  RingSetLinks;
} // namespace xAOD

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::RingSetContainer , 1213110449 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODCALORINGS_RINGSETCONTAINER_H


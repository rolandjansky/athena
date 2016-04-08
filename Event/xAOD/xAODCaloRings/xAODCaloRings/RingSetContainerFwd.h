/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetContainerFwd.h 707323 2015-11-12 02:45:01Z wsfreund $ 
#ifndef XAODCALORINGS_RINGSETCONTAINERFWD_H
#define XAODCALORINGS_RINGSETCONTAINERFWD_H

// STL include(s):
#include <vector>

// This is a forward declaration for the latest RingSet container type.
// Core include(s):
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloRings/tools/DataVectorFwd.h"
#include "xAODCaloRings/RingSetFwd.h"

namespace xAOD {
/// Forward declaration of RingSetContainer
typedef DataVector< RingSet > RingSetContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetContainer > >
  RingSetLinks;
} // namespace xAOD

#endif // XAODCALORINGS_RINGSETCONTAINERFWD_H


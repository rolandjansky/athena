/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetContainerFwd.h 659538 2015-04-08 17:29:18Z wsfreund $ 
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
  RingSetELVec;
} // namespace xAOD

#endif // XAODCALORINGS_RINGSETCONTAINERFWD_H


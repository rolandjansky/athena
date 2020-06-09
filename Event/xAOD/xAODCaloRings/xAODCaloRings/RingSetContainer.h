// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_RINGSETCONTAINER_H
#define XAODCALORINGS_RINGSETCONTAINER_H

// STL include(s):
#include <vector>

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODCaloRings/RingSet.h"
#include "xAODCaloRings/versions/RingSetContainer_v1.h"

namespace xAOD {

/// Definition of the current "RingSet container version"
typedef RingSetContainer_v1 RingSetContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetContainer > >
  RingSetLinks;

} // namespace xAOD

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::RingSetContainer , 1213110449 , 1 )

#endif // XAODCALORINGS_RINGSETCONTAINER_H


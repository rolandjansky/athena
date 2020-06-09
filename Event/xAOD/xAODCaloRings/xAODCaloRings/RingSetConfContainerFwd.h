// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_RINGSETCONFCONTAINERFWD_H
#define XAODCALORINGS_RINGSETCONFCONTAINERFWD_H

// This is a forward declaration for the latest RingSetConf container type.

// Local include(s).
#include "xAODCaloRings/RingSetConfFwd.h"

// EDM include(s).
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

// System include(s).
#include <vector>

namespace xAOD {

/// Forward declaration of RingSetConfContainer
typedef DataVector< RingSetConf > RingSetConfContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetConfContainer > >
  RingSetConfLinks;

} // namespace xAOD

#endif // XAODCALORINGS_RINGSETCONFCONTAINERFWD_H

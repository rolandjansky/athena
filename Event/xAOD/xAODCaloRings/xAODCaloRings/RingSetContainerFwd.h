// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_RINGSETCONTAINERFWD_H
#define XAODCALORINGS_RINGSETCONTAINERFWD_H

// Local include(s):
#include "xAODCaloRings/RingSetFwd.h"

// EDM include(s).
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

// System include(s).
#include <vector>

namespace xAOD {

/// Forward declaration of RingSetContainer
typedef DataVector< RingSet > RingSetContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetContainer > >
  RingSetLinks;

} // namespace xAOD

#endif // XAODCALORINGS_RINGSETCONTAINERFWD_H

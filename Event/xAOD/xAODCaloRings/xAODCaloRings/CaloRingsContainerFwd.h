// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_CALORINGSCONTAINERFWD_H
#define XAODCALORINGS_CALORINGSCONTAINERFWD_H

// This is a forward declaration for the latest CaloRings container type.

// Local include(s).
#include "xAODCaloRings/CaloRingsFwd.h"

// Core include(s).
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

// System include(s).
#include <vector>

namespace xAOD {

/// Forward declaration of CaloRingsContainer
typedef DataVector< CaloRings > CaloRingsContainer;
/// Declare element links vector
typedef std::vector< ElementLink< CaloRingsContainer > >
  CaloRingsLinks;

} // namespace xAOD

#endif // XAODCALORINGS_CALORINGSCONTAINERFWD_H

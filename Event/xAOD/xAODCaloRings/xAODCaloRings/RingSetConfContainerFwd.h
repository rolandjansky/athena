/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfContainerFwd.h 707323 2015-11-12 02:45:01Z wsfreund $ 
#ifndef XAODCALORINGS_RINGSETCONFCONTAINERFWD_H
#define XAODCALORINGS_RINGSETCONFCONTAINERFWD_H

// This is a forward declaration for the latest RingSetConf container type.

// Core include(s):
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloRings/tools/DataVectorFwd.h"
#include "xAODCaloRings/RingSetConfFwd.h"

/// Declare that this is a vector of CaloRings-derived objects
//DATAVECTOR_BASE_FWD( xAOD::RingSetConf, SG::AuxElement );

namespace xAOD {
/// Forward declaration of RingSetConfContainer
typedef DataVector< RingSetConf > RingSetConfContainer;
/// Declare element links vector
typedef std::vector< ElementLink< RingSetConfContainer > >
  RingSetConfLinks;
} // namespace xAOD

#endif // XAODCALORINGS_RINGSETCONFCONTAINERFWD_H

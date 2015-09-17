/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsContainerFwd.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_CALORINGSCONTAINERFWD_H
#define XAODCALORINGS_CALORINGSCONTAINERFWD_H

// This is a forward declaration for the latest CaloRings container type.

// Core include(s):
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloRings/tools/DataVectorFwd.h"
#include "xAODCaloRings/CaloRingsFwd.h"

/// Declare that this is a vector of CaloRings-derived objects
//DATAVECTOR_BASE_FWD( xAOD::CaloRings, SG::AuxElement );

namespace xAOD {
/// Forward declaration of CaloRingsContainer
typedef DataVector< CaloRings > CaloRingsContainer;
/// Declare element links vector
typedef std::vector< ElementLink< CaloRingsContainer > >
  CaloRingsELVec;
} // namespace xAOD

#endif // XAODCALORINGS_CALORINGSCONTAINERFWD_H


// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_CALORINGSAUXCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_CALORINGSAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// Local include(s).
#include "xAODCaloRings/RingSetContainer.h"

namespace xAOD {

/// Auxiliary store for @c xAOD::CaloRingsContainer
class CaloRingsAuxContainer_v1 : public AuxContainerBase {

public:
  /// @name xAOD::CaloRingsAuxContainer constructors
  CaloRingsAuxContainer_v1();

private:
  /// @{
  /// The RingSet ElementLink Vector:
  std::vector< RingSetLinks > ringSetLinks;
  /// @}

}; // class CaloRingsAuxContainer_v1

} // namespace xAOD

// Base declaration for the class.
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CaloRingsAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODCALORINGS_VERSIONS_CALORINGSAUXCONTAINER_V1_H

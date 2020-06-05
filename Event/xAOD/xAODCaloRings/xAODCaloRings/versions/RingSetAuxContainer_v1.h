// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_RINGSETAUXCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

/// Auxiliary store for @c xAOD::RingSetContainer
class RingSetAuxContainer_v1 : public AuxContainerBase {

public:

  /// @name xAOD::RingSetAuxContainer constructors
  RingSetAuxContainer_v1();

private:

  /// @{
  /// @brief Rings transverse energy information.
  std::vector< std::vector<float> > ringsE;
  /// @}


}; // class RingSetAuxContainer_v1

} // namespace xAOD

// Base declaration for the class.
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::RingSetAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODCALORINGS_VERSIONS_RINGSETAUXCONTAINER_V1_H



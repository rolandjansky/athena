/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetAuxContainer_v1.h 659533 2015-04-08 17:05:53Z wsfreund $
#ifndef XAODCALORINGS_VERSIONS_RINGSETAUXCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

/// Temporary container used until we have I/O for AuxStoreInternal
///
/// This class is meant to serve as a temporary way to provide an auxiliary
/// store with Athena I/O capabilities for the RingSet EDM. Will be exchanged for
/// a generic auxiliary container type (AuxStoreInternal) later on. Copied
/// from xAOD::Egamma.
///
/// $Revision: 659533 $
/// $Date: 2015-04-08 19:05:53 +0200 (Wed, 08 Apr 2015) $
///

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

#ifndef XAOD_STANDALONE
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::RingSetAuxContainer_v1, xAOD::AuxContainerBase );
#endif

#endif // XAODCALORINGS_VERSIONS_RINGSETAUXCONTAINER_V1_H



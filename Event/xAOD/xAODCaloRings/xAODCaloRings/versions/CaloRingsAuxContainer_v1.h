/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsAuxContainer_v1.h 707323 2015-11-12 02:45:01Z wsfreund $
#ifndef XAODCALORINGS_VERSIONS_CALORINGSAUXCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_CALORINGSAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

#include "xAODCaloRings/versions/RingSetContainer_v1.h"

namespace xAOD {

/// Temporary container used until we have I/O for AuxStoreInternal
///
/// This class is meant to serve as a temporary way to provide an auxiliary
/// store with Athena I/O capabilities for the CaloRings EDM. Will be exchanged for
/// a generic auxiliary container type (AuxStoreInternal) later on. Copied
/// from xAOD::Egamma.
///
/// $Revision: 707323 $
/// $Date: 2015-11-12 03:45:01 +0100 (Thu, 12 Nov 2015) $
///

class CaloRingsAuxContainer_v1 : public AuxContainerBase {

public:

  /// @name xAOD::CaloRingsAuxContainer constructors
  CaloRingsAuxContainer_v1();

private:

  /// @{
  /// The RingSet ElementLink Vector:
  std::vector< RingSetLinks_v1 > ringSetLinks;
  /// @}


}; // class CaloRingsAuxContainer_v1

} // namespace xAOD

#ifndef XAOD_STANDALONE
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::CaloRingsAuxContainer_v1, xAOD::AuxContainerBase );
#endif

#endif // XAODCALORINGS_VERSIONS_CALORINGSAUXCONTAINER_V1_H


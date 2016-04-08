/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfAuxContainer_v1.h 704590 2015-10-29 17:47:12Z wsfreund $
#ifndef XAODCALORINGS_VERSIONS_RINGSETCONFAUXCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETCONFAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

/// Temporary container used until we have I/O for AuxStoreInternal
///
/// This class is meant to serve as a temporary way to provide an auxiliary
/// store with Athena I/O capabilities for the Metadata containing the RingSet
/// configuration EDM. Will be exchanged for a generic auxiliary container type
/// (AuxStoreInternal) later on. Copied from xAOD::Egamma.
///
/// $Revision: 704590 $
/// $Date: 2015-10-29 18:47:12 +0100 (Thu, 29 Oct 2015) $
///

class RingSetConfAuxContainer_v1 : public AuxContainerBase {

public:

  /// @name xAOD::RingSetConfAuxContainer_v1 constructors
  RingSetConfAuxContainer_v1();

private:

  /// @{
  /// Ringset number of Rings
  std::vector<unsigned> nRings;    
  /// RingSet CaloSample layers used to build this RingSet
  std::vector<std::vector<unsigned/* = CaloSampling::CaloSample*/> > layers;
  /// Ring width in eta
  std::vector<float> etaWidth;   
  /// Ring width in phi
  std::vector<float> phiWidth;
  /// Maximun distance cell in eta to seed
  std::vector<float> cellMaxDEtaDist;
  /// Maximun distance cell in phi to seed
  std::vector<float> cellMaxDPhiDist;
  /// The Calorimeter Joint Layer (see CaloRingsDefs.h)
  std::vector<unsigned/*= Ringer::CalJointLayer*/> calJointLayer;
  /// The Calorimeter Joint Section (see CaloRingsDefs.h)
  std::vector<unsigned/*= Ringer::CalJointSection*/> calJointSection;
  /// Ring division in eta
  std::vector<char> doEtaAxesDivision;
  /// Ring division in phi
  std::vector<char> doPhiAxesDivision;
  /// Hold the start of this layer on a vectorized representation:
  std::vector<unsigned> layerStartIdx;
  /// Hold the end of this layer on a vectorized representation:
  std::vector<unsigned> layerEndIdx;
  /// Hold the start of this section on a vectorized representation:
  std::vector<unsigned> sectionStartIdx;
  /// Hold the end of this section on a vectorized representation:
  std::vector<unsigned> sectionEndIdx;
  /// @}


}; // class RingSetConfAuxContainer_v1

} // namespace xAOD

#ifndef XAOD_STANDALONE
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::RingSetConfAuxContainer_v1, xAOD::AuxContainerBase );
#endif

#endif // XAODCALORINGS_VERSIONS_RINGSETCONFAUXCONTAINER_V1_H

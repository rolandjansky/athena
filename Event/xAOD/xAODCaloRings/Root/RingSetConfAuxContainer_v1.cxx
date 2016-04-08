/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfAuxContainer_v1.cxx 704590 2015-10-29 17:47:12Z wsfreund $ 
// Local include(s):
#include "xAODCaloRings/versions/RingSetConfAuxContainer_v1.h"

namespace xAOD {
  
RingSetConfAuxContainer_v1::RingSetConfAuxContainer_v1()
  : AuxContainerBase() {

  /// Ringset number of Rings
  AUX_VARIABLE(nRings);
  /// RingSet CaloSample layers used to build this RingSet
  AUX_VARIABLE(layers);
  /// Ring width in eta
  AUX_VARIABLE(etaWidth);
  /// Ring width in phi
  AUX_VARIABLE(phiWidth);
  /// Maximun cell distance in eta to seed
  AUX_VARIABLE(cellMaxDEtaDist);
  /// Maximun cell distance in phi to seed
  AUX_VARIABLE(cellMaxDPhiDist);
  /// The Calorimeter Joint Layer (see CaloRingsDefs.h)
  AUX_VARIABLE(calJointLayer);
  /// The Calorimeter Joint Section (see CaloRingsDefs.h)
  AUX_VARIABLE(calJointSection);
  /// Ring division in eta
  AUX_VARIABLE(doEtaAxesDivision);
  /// Ring division in phi
  AUX_VARIABLE(doPhiAxesDivision);
  /// Hold the start of this layer on a vectorized representation:
  AUX_VARIABLE(layerStartIdx);
  /// Hold the end of this layer on a vectorized representation:
  AUX_VARIABLE(layerEndIdx);
  /// Hold the start of this section on a vectorized representation:
  AUX_VARIABLE(sectionStartIdx);
  /// Hold the end of this section on a vectorized representation:
  AUX_VARIABLE(sectionEndIdx);
 
}
  
} // namespace xAOD

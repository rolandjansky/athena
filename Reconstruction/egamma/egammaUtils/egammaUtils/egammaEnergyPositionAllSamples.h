/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAENERGYPOSITIONALLSAMPLES_H
#define EGAMMACALOTOOLS_EGAMMAENERGYPOSITIONALLSAMPLES_H

/// @class egammaEnergyPositionAllSamples
/// @brief Helpers to define the energy and position in each sampling
///  From the original (eta,phi) position of the cluster, it finds the location
///  (sampling, barrel/end-cap, granularity)
///  For this we use the CaloCluster method inBarrel() and inEndcap()
///  but also, in case close to the crack region where both 
///  boolean can be true, the energy reconstructed in the sampling
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
///

#include "xAODCaloEvent/CaloClusterFwd.h"

namespace egammaEnergyPositionAllSamples{
 /** @brief return the uncorrected sum of energy in all samples*/
  double e(const xAOD::CaloCluster& cluster);
  /** @brief return the uncorrected cluster energy in pre-sampler*/
  double e0(const xAOD::CaloCluster& cluster);
  /** @brief return the uncorrected cluster energy in 1st sampling*/
  double e1(const xAOD::CaloCluster& cluster);
  /** @brief return the uncorrected cluster energy in 2nd sampling*/
  double e2(const xAOD::CaloCluster& cluster);
  /** @brief return the uncorrected cluster energy in 3rd sampling*/
  double e3(const xAOD::CaloCluster& cluster);
  /** @brief return boolean to know if we are in barrel/end-cap*/
  bool inBarrel(const xAOD::CaloCluster& cluster, int is);

}

#endif

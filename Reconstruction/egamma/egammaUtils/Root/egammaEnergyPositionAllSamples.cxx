/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaUtils/egammaEnergyPositionAllSamples.h"
#include "xAODCaloEvent/CaloCluster.h"

double
egammaEnergyPositionAllSamples::e(const xAOD::CaloCluster& cluster)
{
  return (e0(cluster) + e1(cluster) + e2(cluster) + e3(cluster));
}

double
egammaEnergyPositionAllSamples::e0(const xAOD::CaloCluster& cluster)
{
  return (cluster.eSample(CaloSampling::PreSamplerB) + cluster.eSample(CaloSampling::PreSamplerE));
}

double
egammaEnergyPositionAllSamples::e1(const xAOD::CaloCluster& cluster) 
{
  return (cluster.eSample(CaloSampling::EMB1) + cluster.eSample(CaloSampling::EME1));
}

double
egammaEnergyPositionAllSamples::e2(const xAOD::CaloCluster& cluster)
{
  return (cluster.eSample(CaloSampling::EMB2) + cluster.eSample(CaloSampling::EME2));
}

double
egammaEnergyPositionAllSamples::e3(const xAOD::CaloCluster& cluster)
{
  return (cluster.eSample(CaloSampling::EMB3) + cluster.eSample(CaloSampling::EME3));
}

bool
egammaEnergyPositionAllSamples::inBarrel(const xAOD::CaloCluster& cluster, const int sampling)
{

  // from cluster position and energy define
  // if we are in barrel or end-cap
  // In particular it uses energy deposit in 2nd sampling
  // to check close to the crack region
  // sampling (0=presampler,1=strips,2=middle,3=back)
  //
  if (cluster.inBarrel() && !cluster.inEndcap()) {
    return true;//barrel
  } else if (!cluster.inBarrel() && cluster.inEndcap()) {
    return  false;//endcap
  }
  /*if both in barrel and end-cap then have to
   * rely on energy deposition
   * be careful to test 0 precisely
   * As 0 (no deposit) > noise (which is negative)*/
  else if (cluster.inBarrel() && cluster.inEndcap()) {
    switch (sampling) {
    case 0: {
      if (cluster.eSample(CaloSampling::PreSamplerB) == 0. && cluster.eSample(CaloSampling::PreSamplerE) != 0.) {
        return false; // endcap
      }
      if (cluster.eSample(CaloSampling::PreSamplerB) != 0. && cluster.eSample(CaloSampling::PreSamplerE) == 0.) {
        return true; // barrel
      }
      if (cluster.eSample(CaloSampling::PreSamplerB) >= cluster.eSample(CaloSampling::PreSamplerE)) {
        return true; // barrel;
      } else {
        return false; // endcap;
      }
    }
    case 1: {
      if (cluster.eSample(CaloSampling::EMB1) == 0. && cluster.eSample(CaloSampling::EME1) != 0.) {
        return false; // endcap
      }
      if (cluster.eSample(CaloSampling::EMB1) != 0. && cluster.eSample(CaloSampling::EME1) == 0.) {
        return true; // barrel
      }
      if (cluster.eSample(CaloSampling::EMB1) >= cluster.eSample(CaloSampling::EME1)) {
        return true; // barrel
      } else {
        return false; // endcap
      }
    }
    case 2: {
      if (cluster.eSample(CaloSampling::EMB2) == 0. && cluster.eSample(CaloSampling::EME2) != 0.) {
        return false; // endcap
      }
      if (cluster.eSample(CaloSampling::EMB2) != 0. && cluster.eSample(CaloSampling::EME2) == 0.) {
        return true; // barrel
      }

      if (cluster.eSample(CaloSampling::EMB2) >= cluster.eSample(CaloSampling::EME2)) {
        return true; // barrel
      } else {
        return false; // endcap
      }
    }
    case 3: {
      if (cluster.eSample(CaloSampling::EMB3) == 0. && cluster.eSample(CaloSampling::EME3) != 0.) {
        return false; // endcap
      }
      if (cluster.eSample(CaloSampling::EMB3) != 0. && cluster.eSample(CaloSampling::EME3) == 0.) {
        return true; // barrel
      }

      if (cluster.eSample(CaloSampling::EMB3) >= cluster.eSample(CaloSampling::EME3)) {
        return true; // barrel
      } else {
        return false; // endcap
      }
    }
    default: {
      return true; // barrel default
    }
    }
  }

  return true; // barrel defautl
}


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/CandidateMatchHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <cmath>

Amg::Vector3D
CandidateMatchHelpers::approxXYZwrtPoint(const xAOD::CaloCluster& cluster,
                                         const Amg::Vector3D& point,
                                         const bool isEndCap)
{

  return approxXYZwrtATLAS(cluster, isEndCap) - point;
}

Amg::Vector3D
CandidateMatchHelpers::approxXYZwrtATLAS(const xAOD::CaloCluster& cluster,
                                         const bool isEndCap)
{
  // These are at the face of the calorimeter
  const double RfaceCalo = 1500;
  const double ZfaceCalo = 3700;
  // Get the Rclus , Zclus given the cluster eta
  double Rclus = RfaceCalo;
  double Zclus = ZfaceCalo;
  const double clusterEta = cluster.eta();
  if (clusterEta != 0) {
    /*
     * tahn(eta) == cos(theta)
     * 1/cosh(clusterEta) == sin(theta)
     * tan =sin/cos
     */
    const double tanthetaclus = (1 / cosh(clusterEta)) / tanh(clusterEta);
    if (isEndCap) {
      Rclus = fabs(ZfaceCalo * (tanthetaclus));
      // Negative Eta ---> negative Z
      if (clusterEta < 0) {
        Zclus = -Zclus;
      }
    } else {
      if (tanthetaclus != 0) {
        Zclus = RfaceCalo / (tanthetaclus);
      }
    }
  } else { // when eta ==0
    Zclus = 0;
  }

  const double clusterPhi = cluster.phi();
  return { Rclus * cos(clusterPhi), Rclus * sin(clusterPhi), Zclus };
}

double
CandidateMatchHelpers::PhiROT(const double Et,
                              const double Eta,
                              const int charge,
                              const double r_first,
                              const bool isEndCap)
{
  // Used in order to derive the formula below
  const double Rcalo = 1200;

  // correct phi for extrapolation to calo
  double phiRot = 0.0;
  double ecCorr = 1.0;
  if (isEndCap) {
    const double ecFactor = 1.0 / (0.8 * atan(15.0 / 34.0));
    const double sinTheta0 = 2.0 * atan(exp(-fabs(Eta)));
    ecCorr = sinTheta0 * sqrt(sinTheta0) * ecFactor;
  }
  ////
  const double Rscaled = (Rcalo - r_first) * (1. / Rcalo);
  phiRot = Rscaled * ecCorr * charge * 430. / (Et);
  return phiRot;
}


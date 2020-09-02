/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwECcrack_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the cracks (for the time being only barrel - endcap)
          Tuned using 50 GeV Et photons
          Correction tuned on G3 samples.

atrecon Orig: emreco/qecrack.F           

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool


********************************************************************/

#include "CaloSwECcrack_g3.h"

// constants from atrecon/emreco/qecrack.F 
using xAOD::CaloCluster;
const float CaloSwECcrack_g3::P0[3] = {116.8, 169.77, 62.224};
const float CaloSwECcrack_g3::P1[1] = {1.614};
const float CaloSwECcrack_g3::P2[3] = {188.1, 242.89, 78.836};


void CaloSwECcrack_g3::makeCorrection (const Context& /*myctx*/,
                                       CaloCluster* cluster) const
{

  float eta = cluster->etaBE(2);   // use second sampling 

  float aeta = fabs(eta);

  float qECcrack = 1.;
  if (aeta > 1.35 && aeta < 1.46) 
    qECcrack = P0[0] - P0[1]*aeta + P0[2]*aeta*aeta;
  else if (aeta >= 1.46 && aeta < 1.475)
    qECcrack = P1[0];
  else if (aeta >= 1.475 && aeta < 1.55) 
    qECcrack = P2[0] - P2[1]*aeta + P2[2]*aeta*aeta;
  else
    return;       // No correction required

  // make the correction

  float energy = cluster->e();
  // set the energy, and rescale each sampling. 
  setenergy(cluster, energy*qECcrack);

}

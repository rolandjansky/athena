/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwPhimod_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the modulations with the position in phi
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qphimod.F

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/
#include "CaloSwPhimod_g3.h"
#include <cmath>



// make correction to one cluster 
void CaloSwPhimod_g3::makeCorrection (const Context& myctx,
                                      xAOD::CaloCluster* cluster) const
{

  float eta2 = cluster->etaBE(2); // use second sampling
  float phi2 = cluster->phiBE(2); // use the average over Bar and EC.
  if (eta2 < -900 || phi2 < -900)
    return;

  float aeta2 = fabs(eta2);
  float nabs;
  if (aeta2 < 1.5) 
      nabs = 1024; 
  else if (aeta2 < 2.5)
      nabs = 768;
  else
      nabs = 256.;

  std::vector<float> tab = qphmop (myctx, aeta2);
  assert (tab.size() == 5);
  float qphimod = 
    tab[0] + 
    tab[1] * cos(nabs*phi2) +
    tab[2] * cos(2.*nabs*phi2)+
    tab[3] * cos(3.*nabs*phi2)+ 
    tab[4] * sin(nabs*phi2);
  
  // set energy, and rescale each sampling
  setenergy(cluster, qphimod * cluster->e());
  
  
}

std::vector<float> CaloSwPhimod_g3::qphmop (const Context& myctx,
                                            float aeta) const
{
  const CxxUtils::Array<2> correction = m_correction (myctx);

  std::vector<float> coef (correction.size(1)-1);

  int i1 = 0;
  int i2 = 0;
  if (aeta < correction[0][0]) {
    for (unsigned int j = 1; j < correction.size(1); j++)
      coef[j-1] = correction[0][j];
    return coef;
  }
    
  else if (aeta >= correction[0][0]) {
    for (unsigned int i = 0; i < correction.size()-1; i++) {
      if (aeta >= correction[i][0] && aeta < correction[i+1][0]) {
        i1 = i;
        i2 = i+1;
        break;
      }
    }
  }
  if (i1 == i2) {
    i2 = correction.size()-1;
    i1 = i2-1;
  }

  float deta = correction[i2][0] - correction[i1][0];
  if (deta == 0)
    deta = 1;

  float m = (aeta - correction[i1][0]) / deta;
  for (unsigned int j = 1; j < correction.size(1); j++) {
    float dy = correction[i2][j] - correction[i1][j];
    coef[j-1] = (correction[i1][j] + dy * m);
  }

  return coef;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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



// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
using xAOD::CaloCluster;
CaloSwPhimod_g3::CaloSwPhimod_g3(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : CaloClusterCorrection(type,name,parent)
{
  declareConstant ("correction", m_correction);
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloSwPhimod_g3::~CaloSwPhimod_g3()
{ }

// make correction to one cluster 
void CaloSwPhimod_g3::makeCorrection(const EventContext& /*ctx*/,
                                     CaloCluster* cluster) const
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

  std::vector<float> tab = qphmop (aeta2);
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

std::vector<float> CaloSwPhimod_g3::qphmop (float aeta) const
{
  std::vector<float> coef (m_correction.size(1)-1);

  int i1 = 0;
  int i2 = 0;
  if (aeta < m_correction[0][0]) {
    for (unsigned int j = 1; j < m_correction.size(1); j++)
      coef[j-1] = m_correction[0][j];
    return coef;
  }
    
  else if (aeta >= m_correction[0][0]) {
    for (unsigned int i = 0; i < m_correction.size()-1; i++) {
      if (aeta >= m_correction[i][0] && aeta < m_correction[i+1][0]) {
        i1 = i;
        i2 = i+1;
        break;
      }
    }
  }
  if (i1 == i2) {
    i2 = m_correction.size()-1;
    i1 = i2-1;
  }

  float deta = m_correction[i2][0] - m_correction[i1][0];
  if (deta == 0)
    deta = 1;

  float m = (aeta - m_correction[i1][0]) / deta;
  for (unsigned int j = 1; j < m_correction.size(1); j++) {
    float dy = m_correction[i2][j] - m_correction[i1][j];
    coef[j-1] = (m_correction[i1][j] + dy * m);
  }

  return coef;
}


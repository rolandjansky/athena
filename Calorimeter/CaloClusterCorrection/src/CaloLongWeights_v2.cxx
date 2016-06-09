/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloLongWeights_v2.cxx
PACKAGE:  offline/Calorimter/CaloClusterCorrection
 
AUTHORS:  Mohamed Aharrouche
CREATED:  Jan 25, 2006

PURPOSE:  correction for the energy lost in the dead material

********************************************************************/
#include "CaloLongWeights_v2.h"
#include "CaloClusterCorrection/interpolate.h"


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
 
CaloLongWeights_v2::CaloLongWeights_v2(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant("correction",      m_correction);
  declareConstant("energies",        m_energies);
  declareConstant("degree",          m_degree);
  declareConstant("EtaMax",          m_etamax);
  declareConstant("EtaBarrel",       m_barrel_frontier);
  declareConstant("EtaEndCap",       m_endcap_frontier);
}
 
void CaloLongWeights_v2::makeTheCorrection(const EventContext& /*ctx*/,
                                           CaloCluster* cluster,
                                           const CaloDetDescrElement* /*elt*/,
                                           float eta,
                                           float /*adj_eta*/,
                                           float /*phi*/,
                                           float /*phi*/,
                                           CaloSampling::CaloSample /*samp*/) const
{ 
  float the_eta = std::abs (eta);
  if (the_eta >= m_etamax) return;


  bool inBarrel = false;
  if (the_eta < m_barrel_frontier)
    inBarrel= true;
  else if (the_eta <= m_endcap_frontier)
    return;

  //initial cluster energy
  float energy = cluster->e();

  //Weights
  float coefs[5];

  //eta bin
  int etaIndex = (int) (the_eta * (1./0.025));

  //samples
  unsigned int n_samples = m_energies.size();
  
  
  unsigned int shape[] = {n_samples, 6};
  CaloRec::WritableArrayData<2> vectParm (shape);

  
  for(unsigned int i=0;i<n_samples;i++)
    {
      vectParm[i][0] = m_energies[i];
      for(unsigned int j=0;j<5;j++)
	{
	  vectParm[i][j+1] = m_correction[i][etaIndex][j+1];
	}
    }

  for(unsigned int k=0; k<5;k++)
    {
      if(energy < m_energies[0])
        coefs[k] = vectParm[0][k+1];
      else if(energy > m_energies[n_samples-1])
        coefs[k] = vectParm[n_samples-1][k+1];
      else
	coefs[k] = interpolate (vectParm, energy, m_degree, k+1);
    }



  typedef CaloSampling::CaloSample Samplingmap[4];
  static const Samplingmap LBarrel = { CaloSampling::PreSamplerB,
      CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3 };
  static const Samplingmap LEndcap = { CaloSampling::PreSamplerE,
      CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3 };
  const Samplingmap& L = inBarrel ? LBarrel : LEndcap;

  
  float Egap=0.0;

  if(cluster->eSample(L[0])*cluster->eSample(L[1])>0.)
    Egap=coefs[3]*std::sqrt(cluster->eSample(L[0])*cluster->eSample(L[1]));
  
  
  cluster->setEnergy ( L[0],cluster->eSample(L[0])*coefs[0]);
  
  float Eleak = cluster->eSample(L[3])*coefs[1];
  
  //total energy
  float Erec = 0.0;
  for (int iSample=1; iSample < 4 ; ++iSample) 
    {
      Erec += cluster->eSample (L[iSample]);
    }
    
  
  cluster->setE (coefs[4]*Erec + cluster->eSample (L[0]) + Eleak + coefs[2]+ Egap);

}


/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 
void CaloLongWeights_v2::makeTheCorrection (const Context& myctx,
                                            CaloCluster* cluster,
                                            const CaloDetDescrElement* /*elt*/,
                                            float eta,
                                            float /*adj_eta*/,
                                            float /*phi*/,
                                            float /*phi*/,
                                            CaloSampling::CaloSample /*samp*/) const
{
  float the_eta = std::abs (eta);
  if (the_eta >= m_etamax (myctx)) return;


  bool inBarrel = false;
  if (the_eta < m_barrel_frontier (myctx))
    inBarrel= true;
  else if (the_eta <= m_endcap_frontier (myctx))
    return;

  const CxxUtils::Array<3> correction = m_correction (myctx);
  const CxxUtils::Array<1> energies   = m_energies (myctx);
  const int degree = m_degree (myctx);

  //initial cluster energy
  float energy = cluster->e();

  //Weights
  float coefs[5];

  //eta bin
  int etaIndex = (int) (the_eta * (1./0.025));

  //samples
  unsigned int n_samples = energies.size();
  
  
  unsigned int shape[] = {n_samples, 6};
  CaloRec::WritableArrayData<2> vectParm (shape);


  for(unsigned int i=0;i<n_samples;i++)
    {
      vectParm[i][0] = energies[i];
      for(unsigned int j=0;j<5;j++)
	{
	  vectParm[i][j+1] = correction[i][etaIndex][j+1];
	}
    }

  for(unsigned int k=0; k<5;k++)
    {
      if(energy < energies[0])
        coefs[k] = vectParm[0][k+1];
      else if(energy > energies[n_samples-1])
        coefs[k] = vectParm[n_samples-1][k+1];
      else
	coefs[k] = interpolate (vectParm, energy, degree, k+1);
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


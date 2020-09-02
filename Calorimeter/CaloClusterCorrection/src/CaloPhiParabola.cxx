/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloPhiParabola.cxx
PACKAGE:  offline/Calorimter/CaloClusterCorrection
 
AUTHORS:  Mohamed Aharrouche
CREATED:  Jan 25, 2006

PURPOSE:  corrects for the parabolic behavior in the E vs Phi curve

********************************************************************/
#include "CaloPhiParabola.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;

void CaloPhiParabola::makeTheCorrection (const Context& myctx,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* /*elt*/,
                                         float /*eta*/,
                                         float /*adj_eta*/,
                                         float /*phi*/,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample /*samp*/)
  const
{
  const CxxUtils::Array<2> correction = m_correction (myctx);
  const CxxUtils::Array<1> energies = m_energies (myctx);
  const int degree = m_degree (myctx);
  
  //
  float coefs[3];

  unsigned int n_samples = energies.size();

  unsigned int shape[] = {n_samples, 4};
  CaloRec::WritableArrayData<2> pol2Parm (shape);

  
  for(unsigned int i=0;i<n_samples;i++)
    {
      pol2Parm[i][0] = energies[i];
      for(unsigned int j=0;j<3;j++)
	{
	  pol2Parm[i][j+1] = correction[i][j];
	}
    }


  float energy = cluster->e();
  for(unsigned int k=0; k<3;k++)
    {
      if(energy < energies[0])
        coefs[k] = pol2Parm[0][k+1];
      else if(energy > energies[n_samples-1])
        coefs[k] = pol2Parm[n_samples-1][k+1];
      else
	coefs[k] = interpolate (pol2Parm, energy, degree, k+1);
    }

  //
  float phi2 = cluster->phiBE(2);

  float mod = coefs[0]+coefs[1]*phi2+coefs[2]*phi2*phi2;
  
  //
  setenergy (cluster, cluster->e() / mod);
}



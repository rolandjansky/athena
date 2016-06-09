/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

CaloPhiParabola::CaloPhiParabola (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : CaloClusterCorrectionCommon (type,name,parent)
{
  declareConstant ("correction",   m_correction);
  declareConstant("energies",      m_energies);
  declareConstant("degree",        m_degree);
}


void CaloPhiParabola::makeTheCorrection (const EventContext& /*ctx*/,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* /*elt*/,
                                         float /*eta*/,
                                         float /*adj_eta*/,
                                         float /*phi*/,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample /*samp*/)
  const
{
  
  //
  float coefs[3];

  unsigned int n_samples = m_energies.size();

  unsigned int shape[] = {n_samples, 4};
  CaloRec::WritableArrayData<2> pol2Parm (shape);

  
  for(unsigned int i=0;i<n_samples;i++)
    {
      pol2Parm[i][0] = m_energies[i];
      for(unsigned int j=0;j<3;j++)
	{
	  pol2Parm[i][j+1] = m_correction[i][j];
	}
    }


  float energy = cluster->e();
  for(unsigned int k=0; k<3;k++)
    {
      if(energy < m_energies[0])
        coefs[k] = pol2Parm[0][k+1];
      else if(energy > m_energies[n_samples-1])
        coefs[k] = pol2Parm[n_samples-1][k+1];
      else
	coefs[k] = interpolate (pol2Parm, energy, m_degree, k+1);
    }

  //
  float phi2 = cluster->phiBE(2);

  float mod = coefs[0]+coefs[1]*phi2+coefs[2]*phi2*phi2;
  
  //
  setenergy (cluster, cluster->e() / mod);
}



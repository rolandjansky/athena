/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwEta1e_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 1 as a function of eta for endcap
          (Tuned using 100 GeV E photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta1e.age          

Updated:  Feb 11, 2003   (MW)
          new correction for DC1 data
Updated:  Feb 28, 2003    (MW)
          protect against unphysical clusters

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/
#include "CaloSwEta1e_g3.h"
#include "CaloClusterCorrection/interpolate.h"
#include <cmath>

using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;

// granularity of middle EM barrel layer

  const float CaloSwEta1e_g3::s_middle_layer_granularity = 0.025;

// make correction to one cluster 
void CaloSwEta1e_g3::makeCorrection (const Context& myctx,
                                     CaloCluster* cluster) const
{
  // Only for endcap
  if (!cluster->inEndcap())
    return;

  const CxxUtils::Array<3> correction = m_correction (myctx);
  const float correction_coef = m_correction_coef (myctx);
  const int correction_degree = m_correction_degree (myctx);

  float eta = cluster->etaSample(CaloSampling::EME1);
  if (eta == -999.) return;
  float etamax = cluster->etamax(CaloSampling::EME1);
  float aeta = fabs(eta);

  int corrndx;
  float ufrac;
  if (aeta > 2.0)
  {
    ufrac = s_middle_layer_granularity/4.;
    corrndx = 0;
  }
  else if (aeta > 1.8) 
  {
    ufrac = s_middle_layer_granularity*(1./6);
    corrndx = 1;
  }
  else if (aeta > 1.5) 
  {
    ufrac = s_middle_layer_granularity/8.;
    corrndx = 2;
  }
  else
  {
    // not in endcap, no correction. 
    return;
  }

  float u = fmod(eta-etamax, ufrac/2.);
  u += ufrac/2.;
  if (eta < 0.) u = ufrac-u;
  float deta = correction_coef * interpolate (correction[corrndx],
                                              u,
                                              correction_degree);
  if (eta < 0)
    deta = -deta;

  // Make the correction:
  cluster->setEta( CaloSampling::EME1, eta - deta);
}



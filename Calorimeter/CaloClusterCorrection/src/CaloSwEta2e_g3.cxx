/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwEta2e_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 2 as a function of eta
	  for endcap EM calorimeter. 
          (Tuned using 100 GeV E photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Updated:  Feb 11, 2003   (MW)
          new correction for DC1 data
Updated:  Feb 28, 2003    (MW)
          protect against unphysical clusters

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/

#include "CaloSwEta2e_g3.h"
#include "CaloClusterCorrection/interpolate.h"
#include <cmath>

using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;

// granularity of middle EM barrel layer

  const float CaloSwEta2e_g3::s_middle_layer_granularity = 0.025;


// make correction to one cluster 
void CaloSwEta2e_g3::makeCorrection (const Context& myctx,
                                     CaloCluster* cluster) const
{
  // Only for endcap
  if (!cluster->inEndcap())
    return;

  float eta = cluster->etaSample(CaloSampling::EME2);
  if (eta == -999.) return;
  float etamax = cluster->etamax(CaloSampling::EME2);
  float aeta = fabs(eta);

  // peak around center of cell (+=0.0125)
  float u2 = fmod(eta-etamax,s_middle_layer_granularity/2.);
  u2 += s_middle_layer_granularity/2.;
  // be careful with negative eta values, position correction is not symmetric.
  if (eta < 0.) u2 = s_middle_layer_granularity-u2;

  //   First order (average) S-shape correction
  float etc2 = aeta - interpolate (m_correction(myctx),
                                   u2,
                                   m_correction_degree(myctx));

  if (eta < 0.)
    etc2 = -etc2;

  // make correction to second sampling
  cluster->setEta( CaloSampling::EME2, etc2);
}





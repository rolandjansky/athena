/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwEta1b_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 1 as a function of eta
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta1b.age          

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  Feb 28, 2003    (MW)
          barrel goes only up to eta=1.475, not 1.5

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/
#include "CaloSwEta1b_g3.h"
#include "CaloClusterCorrection/interpolate.h"
#include <cmath>

using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;

const float CaloSwEta1b_g3::s_strip_granularity = 0.003125;    // 0.025/8

// make correction to one cluster.
void CaloSwEta1b_g3::makeCorrection (const Context& myctx,
                                     CaloCluster* lar_cluster) const
{
  // Only for barrel
  if (!lar_cluster->inBarrel())
    return;

  const CxxUtils::Array<2> correction = m_correction(myctx);
  const CxxUtils::Array<1> correction_bins = m_correction_bins(myctx);
  const int correction_degree = m_correction_degree(myctx);
  assert (correction.size(1)-1 == correction_bins.size());

  float eta = lar_cluster->etaSample(CaloSampling::EMB1);
  if (eta == -999.) return;
  float aeta = fabs(eta);
  float u1 = fmod(aeta,s_strip_granularity);

  //   we evaluate the s-shape in each eta range

  unsigned int shape[] = {correction_bins.size(), 2};
  CaloRec::WritableArrayData<2> w (shape);

  for (unsigned int i=0; i<correction_bins.size(); i++) {
    w[i][0] = correction_bins[i];
    w[i][1] = interpolate (correction, u1, correction_degree, i+1);
  };

  //   finally we interpolate for the actual eta2
  float deta = m_correction_coef(myctx) * interpolate (w, aeta, m_interp_degree(myctx));
  if (eta < 0)
    deta = -deta;

  // Make the correction
  lar_cluster->setEta( CaloSampling::EMB1, eta - deta);
}



/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwEta2b_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 2 as a function of eta
	  for barrel EM calorimeter. 
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta2b.age          

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

#include "CaloSwEta2b_g3.h"
#include "CaloClusterCorrection/interpolate.h"
#include <cmath>

using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;

// granularity of middle EM barrel layer

const float CaloSwEta2b_g3::s_middle_layer_granularity = 0.025;


void CaloSwEta2b_g3::makeCorrection (const Context& myctx,
                                     CaloCluster* cluster) const
{
  // Only for barrel
  if (!cluster->inBarrel())
    return;

  const CxxUtils::Array<2> correction = m_correction (myctx);
  const CxxUtils::Array<2> residuals = m_residuals (myctx);
  const CxxUtils::Array<1> residual_bins = m_residual_bins (myctx);
  const int correction_degree = m_correction_degree(myctx);
  const int residual_eval_degree = m_residual_eval_degree(myctx);
  const int residual_degree = m_residual_degree(myctx);
  const float correction_coef = m_correction_coef(myctx);
  const float residual_coef = m_residual_coef(myctx);

  assert (residuals.size(1)-1 == residual_bins.size());

  // eta of second sampling
  float eta = cluster->etaSample(CaloSampling::EMB2); 
  if (eta == -999.) return;
  float aeta = fabs(eta);

  //   u2 is the distance to the inner edge of the cell (granularity 0.025)
  float u2 = fmod(aeta,s_middle_layer_granularity) ;

  //   First order (average) S-shape correction
  float deta = correction_coef * interpolate (correction,
                                              u2,
                                              correction_degree);

  //    Residual S-shape correction: six ranges in eta from 0 to 1.2
  //    Evaluated by 2nd order interpolation between 10 tabulated values

  // first we evaluate our residual in each eta range, given u2

  unsigned int shape[] = {residual_bins.size(), 2};
  CaloRec::WritableArrayData<2> w (shape);
  
  for (unsigned int i=0; i < residual_bins.size(); i++) {
    w[i][0] = residual_bins[i];
    w[i][1] = interpolate (residuals,
			   u2, 
			   residual_eval_degree,
			   i+1);
  }

  // finally we interpolate (2nd order) for the actual eta

  deta += residual_coef * interpolate (w,
                                       aeta,
                                       residual_degree);
  if (eta < 0)
    deta = -deta;

  // make correction to second sampling
  cluster->setEta(CaloSampling::EMB2, eta - deta);
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


CaloSwEta2b_g3::CaloSwEta2b_g3(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : CaloClusterCorrection(type,name,parent,
                          "CaloSwEta${LAYER}${BE}_g3")
{
  declareConstant ("correction",           m_correction);
  declareConstant ("residuals",            m_residuals);
  declareConstant ("residual_bins",        m_residual_bins);
  declareConstant ("correction_degree",    m_correction_degree);
  declareConstant ("residual_eval_degree", m_residual_eval_degree);
  declareConstant ("residual_degree",      m_residual_degree);
  declareConstant ("correction_coef",      m_correction_coef);
  declareConstant ("residual_coef",        m_residual_coef);
  declareConstant ("region",               m_region, true/*temp*/);
}

CaloSwEta2b_g3::~CaloSwEta2b_g3()
{ }


void CaloSwEta2b_g3::makeCorrection(const EventContext& /*ctx*/,
                                    CaloCluster* cluster) const
{
  assert (m_residuals.size(1)-1 == m_residual_bins.size());

  // Only for barrel
  if (!cluster->inBarrel())
    return;

  // eta of second sampling
  float eta = cluster->etaSample(CaloSampling::EMB2); 
  if (eta == -999.) return;
  float aeta = fabs(eta);

  //   u2 is the distance to the inner edge of the cell (granularity 0.025)
  float u2 = fmod(aeta,s_middle_layer_granularity) ;

  //   First order (average) S-shape correction
  float deta = m_correction_coef * interpolate (m_correction,
						u2,
						m_correction_degree);

  //    Residual S-shape correction: six ranges in eta from 0 to 1.2
  //    Evaluated by 2nd order interpolation between 10 tabulated values

  // first we evaluate our residual in each eta range, given u2

  unsigned int shape[] = {m_residual_bins.size(), 2};
  CaloRec::WritableArrayData<2> w (shape);
  
  for (unsigned int i=0; i < m_residual_bins.size(); i++) {
    w[i][0] = m_residual_bins[i];
    w[i][1] = interpolate (m_residuals,
			   u2, 
			   m_residual_eval_degree,
			   i+1);
  }

  // finally we interpolate (2nd order) for the actual eta

  deta += m_residual_coef * interpolate (w,
					 aeta,
					 m_residual_degree);
  if (eta < 0)
    deta = -deta;

  // make correction to second sampling
  cluster->setEta(CaloSampling::EMB2, eta - deta);
}

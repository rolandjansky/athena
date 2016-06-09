/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloSwEta1b_g3::CaloSwEta1b_g3(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : CaloClusterCorrection(type, name, parent,
                          "CaloSwEta${LAYER}${BE}_g3")
{
  declareConstant ("correction",        m_correction);
  declareConstant ("correction_bins",   m_correction_bins);
  declareConstant ("correction_degree", m_correction_degree);
  declareConstant ("interp_degree",     m_interp_degree);
  declareConstant ("correction_coef",   m_correction_coef);
  declareConstant ("region",            m_region, true/*temp*/);
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloSwEta1b_g3::~CaloSwEta1b_g3()
{ }

// make correction to one cluster.
void CaloSwEta1b_g3::makeCorrection(const EventContext& /*ctx*/,
                                    CaloCluster* lar_cluster) const
{
  assert (m_correction.size(1)-1 == m_correction_bins.size());

  // Only for barrel
  if (!lar_cluster->inBarrel())
    return;

  float eta = lar_cluster->etaSample(CaloSampling::EMB1);
  if (eta == -999.) return;
  float aeta = fabs(eta);
  float u1 = fmod(aeta,s_strip_granularity);

  //   we evaluate the s-shape in each eta range

  unsigned int shape[] = {m_correction_bins.size(), 2};
  CaloRec::WritableArrayData<2> w (shape);

  for (unsigned int i=0; i<m_correction_bins.size(); i++) {
    w[i][0] = m_correction_bins[i];
    w[i][1] = interpolate (m_correction, u1, m_correction_degree, i+1);
  };

  //   finally we interpolate for the actual eta2
  float deta = m_correction_coef * interpolate (w, aeta, m_interp_degree);
  if (eta < 0)
    deta = -deta;

  // Make the correction
  lar_cluster->setEta( CaloSampling::EMB1, eta - deta);
}



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTopoEMLongWeights.cxx
PACKAGE:  offline/Calorimter/CaloSwLongWeights
 
AUTHORS:  N. Kerschen 
CREATED:  Nov 8, 2006

PURPOSE:  Longitudinal weights for topoEM cluster (thresholds:633) 

********************************************************************/
#include "CaloTopoEMLongWeights.h"
#include "CaloClusterCorrection/interpolate.h"


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
 

 // -------------------------------------------------------------
 // Constructor 
 // -------------------------------------------------------------
CaloTopoEMLongWeights::CaloTopoEMLongWeights(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant("correction",      m_correction);
  declareConstant("eta_start_crack", m_eta_start_crack);
  declareConstant("eta_end_crack",   m_eta_end_crack);
  declareConstant("etamax",          m_etamax);
  declareConstant("degree",          m_degree);
  declareConstant("use_raw_eta",     m_use_raw_eta);
}
 
/**
 * @brief Virtual function for the correction-specific code.
 * @param ctx     The event context.
 * @param cluster The cluster to correct.
 *                It is updated in place.
 * @param elt     The detector description element corresponding
 *                to the cluster location.
 * @param eta     The @f$\eta@f$ coordinate of the cluster, in this sampling.
 * @param adj_eta The @f$\eta@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param phi     The @f$\phi@f$ coordinate of the cluster, in this sampling.
 * @param adj_phi The @f$\phi@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param samp    The calorimeter sampling we're examining.
 *                This is a sampling code as defined by
 *                @c CaloSampling::CaloSample; i.e., it has both
 *                the calorimeter region and sampling encoded.
 */
void CaloTopoEMLongWeights::makeTheCorrection
  (const EventContext& /*ctx*/,
   CaloCluster* cluster,
   const CaloDetDescrElement* /*elt*/,
   float eta,
   float adj_eta,
   float /*phi*/,
   float /*adj_phi*/,
   CaloSampling::CaloSample /*samp*/) const
{ 
  // ??? In principle, we should use adj_eta for the interpolation
  //     and range checks.  However, the v2 corrections were derived
  //     using regular eta instead.
  float the_aeta;
  if (m_use_raw_eta)
    the_aeta = std::abs (adj_eta);
  else
    the_aeta = std::abs (eta);

  if (the_aeta >= m_etamax) return;

  int si;
  if (the_aeta < m_eta_start_crack)
    si = 0;
  else if (the_aeta > m_eta_end_crack)
    si = 1;
  else {
    // No corrections are applied for the crack region.
    return;
  }

  unsigned int shape[] = {2};
  CaloRec::WritableArrayData<1> interp_barriers (shape);
  interp_barriers[0] = m_eta_start_crack;
  interp_barriers[1] = m_eta_end_crack;

  float pars[6];
  int ibin = static_cast<int> (the_aeta / m_etamax * m_correction.size());
  pars[0] = m_correction[ibin][1];
  pars[1] = m_correction[ibin][2];
  pars[2] = m_correction[ibin][3];
  pars[3] = m_correction[ibin][4];
  pars[4] = m_correction[ibin][5];
  pars[5] = m_correction[ibin][6];

  //  for (int i=0; i<6; i++)
  //  pars[i] = interpolate (m_correction,
  //			   the_aeta,
  //			   m_degree,
  //			   i+1,
  //			   interp_barriers);
  
  static
  CaloSampling::CaloSample samps[2][4] = {
    { CaloSampling::PreSamplerB,
      CaloSampling::EMB1,
      CaloSampling::EMB2,
      CaloSampling::EMB3 },
    { CaloSampling::PreSamplerE,
      CaloSampling::EME1,
      CaloSampling::EME2,
      CaloSampling::EME3 }
  };

  float b = 0;
  if (pars[0] != 0) {
    float a = cluster->e()*exp(-pars[4])/pars[0];
    if (a>1) b = pars[0]*log(a) + pars[0];
  }
  
  cluster->setEnergy (samps[si][0], cluster->eSample (samps[si][0]) * pars[1]);
  cluster->setEnergy (samps[si][3], cluster->eSample (samps[si][3]) * pars[2]);

  double total = 0;
  for (int sampling=0; sampling<4; ++sampling)
    total += cluster->eSample (samps[si][sampling]);

  cluster->setE (pars[3] * (b + pars[5] + total));
}
 


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwLongWeights.cxx
PACKAGE:  offline/Calorimter/CaloSwLongWeights
 
AUTHORS:  K.Loureiro, S.Paganis 
CREATED:  July 5, 2004

PURPOSE:  Optimization correction for the weights in the
          different layers of the EM calorimeter.
          The weights are tuned on electrons at 
          20,50,100 and 150 GeV, from -2.5 <eta < 2.5
           with fixed vertex
          base class: CaloClusterCorrectionCommon
          Uses ToolWithConstants to get corrections constants

********************************************************************/
#include "CaloSwLongWeights.h"
#include "CaloClusterCorrection/interpolate.h"


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
 

 // -------------------------------------------------------------
 // Constructor 
 // -------------------------------------------------------------
CaloSwLongWeights::CaloSwLongWeights(const std::string& type,
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
  declareConstant("preserve_offset", m_preserve_offset);
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
void CaloSwLongWeights::makeTheCorrection
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

  float pars[4];
  int ibin = static_cast<int> (the_aeta / m_etamax * m_correction.size());
  pars[0] = m_correction[ibin][1];
  pars[1] = m_correction[ibin][2];
  for (int i=2; i<4; i++)
    pars[i] = interpolate (m_correction,
                           the_aeta,
                           m_degree,
                           i+1,
                           interp_barriers);

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

  float e_offset = 0;
  if (m_preserve_offset) {
    double total0 = 0;
    for (int sampling=0; sampling<4; ++sampling)
      total0 += cluster->eSample (samps[si][sampling]);
    e_offset = cluster->e() - total0;
  }

  cluster->setEnergy (samps[si][0], cluster->eSample (samps[si][0]) * pars[0]);
  cluster->setEnergy (samps[si][3], cluster->eSample (samps[si][3]) * pars[1]);

  double total = e_offset;
  for (int sampling=0; sampling<4; ++sampling)
    total += cluster->eSample (samps[si][sampling]);

  cluster->setE (pars[2] * (pars[3] + total));
}
 


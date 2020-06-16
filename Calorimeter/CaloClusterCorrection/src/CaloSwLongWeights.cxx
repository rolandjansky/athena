/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 

/**
 * @brief Virtual function for the correction-specific code.
 * @param myctx   ToolWithConstants context.
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
  (const Context& myctx,
   CaloCluster* cluster,
   const CaloDetDescrElement* /*elt*/,
   float eta,
   float adj_eta,
   float /*phi*/,
   float /*adj_phi*/,
   CaloSampling::CaloSample /*samp*/) const
{
  const float eta_start_crack = m_eta_start_crack (myctx);
  const float eta_end_crack = m_eta_end_crack (myctx);

  // ??? In principle, we should use adj_eta for the interpolation
  //     and range checks.  However, the v2 corrections were derived
  //     using regular eta instead.
  float the_aeta;
  if (m_use_raw_eta(myctx))
    the_aeta = std::abs (adj_eta);
  else
    the_aeta = std::abs (eta);

  const float etamax = m_etamax (myctx);
  if (the_aeta >= etamax) return;

  int si;
  if (the_aeta < eta_start_crack)
    si = 0;
  else if (the_aeta > eta_end_crack)
    si = 1;
  else {
    // No corrections are applied for the crack region.
    return;
  }

  const CxxUtils::Array<2> correction = m_correction (myctx);
  const int degree = m_degree (myctx);

  unsigned int shape[] = {2};
  CaloRec::WritableArrayData<1> interp_barriers (shape);
  interp_barriers[0] = eta_start_crack;
  interp_barriers[1] = eta_end_crack;

  float pars[4];
  int ibin = static_cast<int> (the_aeta / etamax * correction.size());
  pars[0] = correction[ibin][1];
  pars[1] = correction[ibin][2];
  for (int i=2; i<4; i++)
    pars[i] = interpolate (correction,
                           the_aeta,
                           degree,
                           i+1,
                           interp_barriers);

  static const
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
  if (m_preserve_offset(myctx)) {
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
 


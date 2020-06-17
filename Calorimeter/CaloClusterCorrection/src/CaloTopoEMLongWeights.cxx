/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
void CaloTopoEMLongWeights::makeTheCorrection
  (const Context& myctx,
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
  if (m_use_raw_eta(myctx))
    the_aeta = std::abs (adj_eta);
  else
    the_aeta = std::abs (eta);

  const float etamax = m_etamax (myctx);
  if (the_aeta >= etamax) return;

  const float eta_start_crack = m_eta_start_crack (myctx);
  const float eta_end_crack = m_eta_end_crack (myctx);

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

  unsigned int shape[] = {2};
  CaloRec::WritableArrayData<1> interp_barriers (shape);
  interp_barriers[0] = eta_start_crack;
  interp_barriers[1] = eta_end_crack;

  float pars[6];
  int ibin = static_cast<int> (the_aeta / etamax * correction.size());
  pars[0] = correction[ibin][1];
  pars[1] = correction[ibin][2];
  pars[2] = correction[ibin][3];
  pars[3] = correction[ibin][4];
  pars[4] = correction[ibin][5];
  pars[5] = correction[ibin][6];

  //  for (int i=0; i<6; i++)
  //  pars[i] = interpolate (correction,
  //			   the_aeta,
  //			   degree,
  //			   i+1,
  //			   interp_barriers);
  
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
 


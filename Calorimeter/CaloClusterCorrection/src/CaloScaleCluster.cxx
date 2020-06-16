/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloScaleCluster.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2006
 * @brief Scale cluster energy by an eta-dependent factor.
 */


#include "CaloScaleCluster.h"
#include "CaloClusterCorrection/interpolate.h"
#include <cmath>


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
void CaloScaleCluster::makeTheCorrection (const Context& myctx,
                                          CaloCluster* cluster,
                                          const CaloDetDescrElement* /*elt*/,
                                          float /*eta*/,
                                          float adj_eta,
                                          float /*phi*/,
                                          float /*adj_phi*/,
                                          CaloSampling::CaloSample /*samp*/)
  const
{
  const CxxUtils::Array<2> correction = m_correction (myctx);
  
  float adj_aeta = std::abs (adj_eta);
  float fac;
  if (std::abs (adj_eta) > m_etamax (myctx))
    fac = correction[correction.size()-1][1];
  else
    fac = interpolate (correction, adj_aeta, m_degree (myctx));
  cluster->setE (cluster->e() / fac);
}

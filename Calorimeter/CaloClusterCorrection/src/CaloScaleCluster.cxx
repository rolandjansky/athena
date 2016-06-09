/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloScaleCluster.cxx,v 1.1 2006-07-07 04:00:06 ssnyder Exp $
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
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloScaleCluster::CaloScaleCluster (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant ("correction", m_correction);
  declareConstant ("degree",     m_degree);
  declareConstant ("etamax",     m_etamax);
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
void CaloScaleCluster::makeTheCorrection (const EventContext& /*ctx*/,
                                          CaloCluster* cluster,
                                          const CaloDetDescrElement* /*elt*/,
                                          float /*eta*/,
                                          float adj_eta,
                                          float /*phi*/,
                                          float /*adj_phi*/,
                                          CaloSampling::CaloSample /*samp*/)
  const
{
  float adj_aeta = std::abs (adj_eta);
  float fac;
  if (std::abs (adj_eta) > m_etamax)
    fac = m_correction[m_correction.size()-1][1];
  else
    fac = interpolate (m_correction, adj_aeta, m_degree);
  cluster->setE (cluster->e() / fac);
}

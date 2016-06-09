/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwEtamod_v2.cxx,v 1.4 2008-01-25 04:14:22 ssnyder Exp $
/**
 * @file  CaloSwEtamod_v2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2004
 * @brief EM calorimeter @f$\eta@f$ modulation corrections.
 * 
 * Derived from previous versions:
 * AUTHORS:  H. Ma, S. Rajagopalan
 * CREATED:  Dec. 15, 1999
 * 
 * PURPOSE:  correction for the modulations with the position in eta
 *           (Tuned using 50 GeV Et photons)  
 *           base class: CaloClusterCorrectionCommon
 * 
 * Atrecon Orig: emreco/qetamod.F
 * 
 * Updated:  May 10, 2000    (SR, HM)
 *           Migrated to Athena Framework from PASO
 * 
 * Updated:  Jan 5, 2001    (HM)
 *           QA. 
 * 
 * Updated:  May 5, 2004    (Sven Menke)
 * 	  base class changed from algo to tool
 * 
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 */


#include "CaloSwEtamod_v2.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <cmath>


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
using std::abs;


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloSwEtamod_v2::CaloSwEtamod_v2 (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : CaloClusterCorrectionCommon (type,name,parent)
{ 
  declareConstant ("correction",       m_correction);
  declareConstant ("rfac",             m_rfac);
  declareConstant ("rfac_degree",      m_rfac_degree);
  declareConstant ("energies",         m_energies);
  declareConstant ("energy_degree",    m_energy_degree);
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
void CaloSwEtamod_v2::makeTheCorrection (const EventContext& /*ctx*/,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* elt,
                                         float eta,
                                         float adj_eta,
                                         float /*phi*/,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample /*samp*/)
  const
{
  // Compute the eta offset within the cell.
  float etamod = eta - elt->eta() + elt->deta()/2;
  if (elt->eta_raw() < 0)
    etamod = elt->deta() - etamod;

  // These can happen due to DD bugs.
  if (etamod < 0)
    etamod = 0;
  else if (etamod > elt->deta())
    etamod = elt->deta();

  float adj_aeta = std::abs (adj_eta);

  // ??? In most of the calorimeter, the cells in sampling 2 have an eta
  //     width of 0.025.  This correction was derived with this assumption.
  //     However, it's not completely accurate: in the gap region, some
  //     cells have a different size.  For example, the last sampling 2
  //     cell in the barrel (starting at 1.4) has a width of 0.075.
  //     For this pass, fix up etamod to match what was used when
  //     the correction was derived.  This should be cleaned up in
  //     a subsequent version.
  if (adj_aeta >= 1.4 && adj_aeta <= 1.475)
    etamod = fmod (adj_aeta, 0.025);

  // Before doing the energy interpolation, make a crude total correction
  // of the energy.  This is needed since the corrections are tabulated
  // using the true cluster energies.
  float energy = cluster->e();
  float rfac = interpolate (m_rfac, adj_aeta, m_rfac_degree);
  energy /= rfac;

  // Calculate the correction.
  float corr = energy_interpolation (energy,
                                     Builder (*this, etamod),
                                     m_energies,
                                     m_energy_degree);

  // set energy, and rescale each sampling
  setenergy (cluster, cluster->e() / corr);
}


/**
 * @brief Constructor for energy interpolation table helper class.
 * @param corr The parent correction object.
 * @param etamod The eta offset within the cell.
 */
CaloSwEtamod_v2::Builder::Builder (const CaloSwEtamod_v2& corr, float etamod)
  : m_corr (corr),
    m_etamod (etamod)
{
}


/**
 * @brief Calculate the correction for one tabulated energy index.
 * @param energy_ndx The tabulated energy index for which
 *                   the correction should be calculated.
 * @param good[out]  Set to true if the calculation succeeds.
 */
float CaloSwEtamod_v2::Builder::calculate (int energy_ndx, bool& good) const
{
  good = true;
  return m_corr.m_correction[energy_ndx][0] 
    + m_etamod*m_corr.m_correction[energy_ndx][1]
    + m_etamod*m_etamod*m_corr.m_correction[energy_ndx][2];
}

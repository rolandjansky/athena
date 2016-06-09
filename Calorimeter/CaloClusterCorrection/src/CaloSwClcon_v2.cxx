/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwClcon_v2.cxx,v 1.3 2008-01-25 04:14:22 ssnyder Exp $
/**
 * @file  CaloSwClcon_v2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2004
 * @brief EM calorimeter containment correction.
 *
 * Derived from previous versions:
 * AUTHORS:  S. Rajagopalan
 * CREATED:  Dec. 15, 1999
 * 
 * PURPOSE:  correction for the finite containment for a given cluster size
 *          (Tuned using 50 GeV Et photons)
 * 
 * 	  Base class: CaloClusterCorrectionCommon
 * 
 * Atrecon Orig: emrecon/qclcon.F
 *           
 * Updated:  May 10, 2000    (SR, HM)
 *           Migrated to Athena Framework from PASO
 * 
 * Updated:  Jan 5, 2001    (HM)
 *           QA. 
 * 
 * Updated:  Feb 2003 (LC)
 *           New corrections for DC1 geometry
 * 
 * Updated:  May 5, 2004    (Sven Menke)
 * 	  base class changed from algo to tool
 * 
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 */


#include "CaloSwClcon_v2.h"
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
CaloSwClcon_v2::CaloSwClcon_v2 (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant ("etamax",     m_etamax);
  declareConstant ("degree",     m_degree);
  declareConstant ("correction", m_correction);
  declareConstant ("energies",       m_energies);
  declareConstant ("energy_degree",  m_energy_degree);
  declareConstant ("use_raw_eta",    m_use_raw_eta);
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
void CaloSwClcon_v2::makeTheCorrection (const EventContext& /*ctx*/,
                                        CaloCluster* cluster,
                                        const CaloDetDescrElement* /*elt*/,
                                        float eta,
                                        float adj_eta,
                                        float /*phi*/,
                                        float /*adj_phi*/,
                                        CaloSampling::CaloSample /*samp*/)
  const
{
  // ??? In principle, we should use adj_eta for the interpolation
  //     and range checks.  However, the v2 corrections were derived
  //     using regular eta instead.
  float the_aeta;
  if (m_use_raw_eta)
    the_aeta = std::abs (adj_eta);
  else
    the_aeta = std::abs (eta);

  if (the_aeta >= m_etamax)
    return;

  // Calculate the correction for each energy.
  unsigned int n_energies = m_energies.size();
  unsigned int shape[] = {n_energies, 2};
  CaloRec::WritableArrayData<2> offstab (shape);

  for (unsigned int i=0; i<n_energies; i++) {
    offstab[i][0] = m_energies[i];
    offstab[i][1] = interpolate (m_correction,
                                 the_aeta,
                                 m_degree,
                                 i+1);
  }

  // Find the median correction.
  float energy = cluster->e() * offstab[n_energies/2][1];

  // Now interpolate in energy.
  float corr;
  if (energy <= offstab[0][0])
    corr = offstab[0][1];
  else if (energy >= offstab[n_energies-1][0])
    corr = offstab[n_energies-1][1];
  else
    corr = interpolate (offstab, energy, m_energy_degree);
 
  // Do the correction
  setenergy (cluster, corr * cluster->e());
}

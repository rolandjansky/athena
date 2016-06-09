/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwPhioff_v2.cxx,v 1.4 2008-01-25 04:14:22 ssnyder Exp $
/**
 * @file  CaloSwPhioff_v2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date July, 2004
 * @brief EM calorimeter @f$\phi@f$ offset corrections.
 * 
 * Derived from previous versions:
 * NAME:     CaloSwPhioff.cxx
 * PACKAGE:  offline/Calorimeter/CaloClusterCorrection
 *
 * AUTHORS:  H. Ma, S. Rajagopalan
 * CREATED:  Dec. 15, 1999
 *
 * PURPOSE:  correction for the phi offset due to accordion structure
 * base class: CaloClusterCorrectionCommon
 *
 * Atrecon Orig: emreco/qphioff.F
 *
 * Updated:  May 10, 2000    (SR, HM)
 *           Migrated to Athena Framework from PASO
 *
 * Updated:  Jan 5, 2001    (HM)
 *           QA. 
 *
 * Updated:  Jan 7, 2003   MW
 *           new corrections in end-cap for DC1 layout
 * Updated:  Feb 28, 2003    (MW)
 *           protect against unphysical clusters
 *
 * Updated:  May 5, 2004    (Sven Menke)
 *           base class changed from algo to tool
 *
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 */


#include "CaloSwPhioff_v2.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include <cmath> 


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloSwPhioff_v2::CaloSwPhioff_v2 (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : CaloClusterCorrectionCommon (type, name, parent)
{
  declareConstant ("interp_barriers",   m_interp_barriers);
  declareConstant ("degree",            m_degree);
  declareConstant ("correction",        m_correction);
  declareConstant ("correction_coef",   m_correction_coef);
  declareConstant ("flip_phi",          m_flip_phi);
  declareConstant ("energies",          m_energies);
  declareConstant ("energy_degree",     m_energy_degree);
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
void CaloSwPhioff_v2::makeTheCorrection (const EventContext& /*ctx*/,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* elt,
                                         float /*eta*/,
                                         float adj_eta,
                                         float phi,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample samp) const
{
  // Calculate the correction.
  float offs = m_correction_coef * energy_interpolation
    (cluster->e(),
     Builder (*this, std::abs (adj_eta)),
     m_energies,
     m_energy_degree);

  // Flip the sign, if needed.
  if (m_flip_phi && elt->eta_raw() < 0)
    offs = -offs;

  // Apply the correction.
  cluster->setPhi (samp, CaloPhiRange::fix (phi + offs));
}


/**
 * @brief Constructor for energy interpolation table helper class.
 * @param corr The parent correction object.
 * @param aeta The absolute value of @f$\eta@f$ at which the correction
 *             is being evaluated (in cal-local coordinates).
 */
CaloSwPhioff_v2::Builder::Builder (const CaloSwPhioff_v2& corr, float aeta)
  : m_corr (corr),
    m_aeta (aeta)
{
}


/**
 * @brief Calculate the correction for one tabulated energy index.
 * @param energy_ndx The tabulated energy index for which
 *                   the correction should be calculated.
 * @param good[out]  Set to true if the calculation succeeds.
 */
float CaloSwPhioff_v2::Builder::calculate (int energy_ndx, bool& good) const
{
  good = true;
  return interpolate (m_corr.m_correction[energy_ndx],
                      m_aeta,
                      m_corr.m_degree,
                      1,
                      m_corr.m_interp_barriers);
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
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
void CaloSwPhioff_v2::makeTheCorrection (const Context& myctx,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* elt,
                                         float /*eta*/,
                                         float adj_eta,
                                         float phi,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample samp) const
{
  // Calculate the correction.
  float offs = m_correction_coef(myctx) * energy_interpolation
    (cluster->e(),
     Builder (m_correction (myctx),
              m_interp_barriers (myctx),
              m_degree (myctx),
              std::abs (adj_eta)),
     m_energies(myctx),
     m_energy_degree(myctx));

  // Flip the sign, if needed.
  if (m_flip_phi(myctx) && elt->eta_raw() < 0)
    offs = -offs;

  // Apply the correction.
  cluster->setPhi (samp, CaloPhiRange::fix (phi + offs));
}


/**
 * @brief Constructor for energy interpolation table helper class.
 * @param correction Tabulated arrays of function parameters.
 * @param interp_barriers Allow breaking up the interpolation into independent regions.
 * @param degree Degree of the polynomial interpolation.
 * @param aeta The absolute value of @f$\eta@f$ at which the correction
 *             is being evaluated (in cal-local coordinates).
 */
CaloSwPhioff_v2::Builder::Builder (const CxxUtils::Array<3>& correction,
                                   const CxxUtils::Array<1>& interp_barriers,
                                   int degree,
                                   float aeta)
  : m_correction (correction),
    m_interp_barriers (interp_barriers),
    m_degree (degree),
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
  return interpolate (m_correction[energy_ndx],
                      m_aeta,
                      m_degree,
                      1,
                      m_interp_barriers);
}

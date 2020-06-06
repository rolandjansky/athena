/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwPhimod_v2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2004
 * @brief EM calorimeter phi modulation corrections.
 * 
 * Derived from previous versions:
 * AUTHORS:  H. Ma, S. Rajagopalan
 * CREATED:  Dec. 15, 1999
 * 
 * PURPOSE:  correction for the modulations with the position in phi
 *           (Tuned using 50 GeV Et photons)  
 *           base class: CaloClusterCorrectionCommon
 * 
 * Atrecon Orig: emreco/qphimod.F
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

#include "CaloSwPhimod_v2.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CLHEP/Units/PhysicalConstants.h" // for pi
#include <cmath>


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
using CLHEP::pi;
using std::atan;
using std::abs;
using std::cos;



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
void CaloSwPhimod_v2::makeTheCorrection (const Context& myctx,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* /*elt*/,
                                         float eta,
                                         float adj_eta,
                                         float /*phi*/,
                                         float adj_phi,
                                         CaloSampling::CaloSample /*samp*/)
  const
{
  // ??? In principle, we should use adj_eta for the interpolation
  //     and range checks.  However, the v2 corrections were derived
  //     using regular eta instead.
  float the_aeta;
  if (m_use_raw_eta(myctx)) {
    the_aeta = std::abs (adj_eta);
    if (adj_eta < 0)
      adj_phi = -adj_phi;
  }
  else {
    the_aeta = std::abs (eta);
    if (eta < 0)
      adj_phi = -adj_phi;
  }

  // Number of absorbers.
  // It would be better to be able to get this from the detector description,
  // but i can't find these numbers there.
  int nabs;
  if (the_aeta < 1.5) 
    nabs = 1024; 
  else if (the_aeta < 2.5)
    nabs = 768;
  else
    nabs = 256;

  // Before doing the energy interpolation, make a crude total correction
  // of the energy.  This is needed since the corrections are tabulated
  // using the true cluster energies.
  float energy = cluster->e();
  float rfac = interpolate (m_rfac(myctx), the_aeta, m_rfac_degree(myctx));
  energy /= rfac;

  float corr = energy_interpolation (energy,
                                     Builder (m_correction (myctx),
                                              m_interp_barriers (myctx),
                                              m_degree (myctx),
                                              m_correction_coef (myctx),
                                              the_aeta, adj_phi, nabs),
                                     m_energies(myctx),
                                     m_energy_degree(myctx));

  // set energy, and rescale each sampling
  setenergy (cluster, cluster->e() / corr);
}


/**
 * @brief Constructor for energy interpolation table helper class.
 * @param correction The correction table.
 * @param interp_barriers Allow breaking up the interpolation into independent regions.
 * @param degree Interpolation degree.
 * @param correction_coef Coefficient by which to scale the entire correction.
 * @param aeta The absolute value of @f$\eta@f$ at which the correction
 *             is being evaluated (in cal-local coordinates).
 * @param phi  The @f$\phi@f$ at which the correction
 *             is being evaluated (in cal-local coordinates).
 * @param nabs Number of absorbers in @f$2\pi@f$.
 */
CaloSwPhimod_v2::Builder::Builder (const CxxUtils::Array<3>& correction,
                                   const CxxUtils::Array<1>& interp_barriers,
                                   int degree,
                                   float correction_coef,
                                   float aeta,
                                   float phi,
                                   int nabs)
  : m_correction (correction),
    m_interp_barriers (interp_barriers),
    m_degree (degree),
    m_correction_coef (correction_coef),
    m_aeta (aeta),
    m_phi (phi),
    m_nabs (nabs)
{
}


/**
 * @brief Calculate the correction for one tabulated energy index.
 * @param energy_ndx The tabulated energy index for which
 *                   the correction should be calculated.
 * @param good[out]  Set to true if the calculation succeeds.
 */
float CaloSwPhimod_v2::Builder::calculate (int energy_ndx, bool& good) const
{
  good = true;
  float par[4];
  for (int j=0; j<4; j++) {
    par[j] = interpolate (m_correction[energy_ndx],
                          m_aeta,
                          m_degree,
                          j+1,
                          m_interp_barriers);
  }
  double a = atan (par[2])*(1./pi) + 0.5;
  return 1 + m_correction_coef * abs (par[0]) *
      (a*cos(m_nabs*m_phi + par[1]) +
       (1-a)*cos (2*m_nabs*m_phi + par[3]));
}

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwClcon_v2.h,v 1.4 2008-01-25 04:14:20 ssnyder Exp $
/**
 * @file  CaloSwClcon_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2004
 * @brief EM calorimeter containment correction.
 *
 * This correction corrects for energy leakage outside of the cluster,
 * as well as for any other as-yet uncorrected effects.
 *
 * Derived from previous versions:
 * AUTHORS:  S. Rajagopalan
 * CREATED:  Dec. 15, 1999
 * 
 * PURPOSE:  correction for the finite containment for a given cluster size
 *           (Tuned using 50 GeV Et photons)
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
 * Updated:  May 5, 2004    (Sven Menke)
 * 	  base class changed from algo to tool
 * 
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 */


#ifndef CALOCLUSTERCORRECTION_CALOSWCLCON_V2_H
#define CALOCLUSTERCORRECTION_CALOSWCLCON_V2_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


/**
 * @class CaloSwClcon_v2.
 * @brief EM calorimeter containment correction.
 *
 * This tool, in principle, corrects for the transverse leakage of the
 * cluster energy outside of the fixed-size window.  In practice, however,
 * this is the last correction to be evaluated, and therefore, this
 * correction is whatever factor is needed to achieve closure.  That is,
 * we choose this correction so that after all corrections are complete,
 * the peaks of the @f$E_{\rm meas}/E_{\rm true}@f$ distributions
 * (binned in @f$\eta@f$) are at 1.  (A future version may revert
 * to being strictly a containment correction, with the closure
 * done elsewhere.)
 *
 * This correction consists of scaling the overall energy by an
 * @f$\eta@f$ and energy-dependent factor.  The energy dependence
 * is handled in this way.  The correction is tabulated for a number
 * of different energies.  We first correct using the median
 * tabulated energy.  The result of this is then used to interpolate
 * in the table to find the final correction factor.
 */
class CaloSwClcon_v2 :
  public CaloClusterCorrectionCommon
{
 public:
  // constructor 
  CaloSwClcon_v2 (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

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
  virtual void makeTheCorrection (const EventContext& ctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const override;

  
private:
  /// Calibration constant: maximum eta for which this correction is defined.
  float             m_etamax;

  /// Calibration constant: degree of the polynomial interpolation.
  int               m_degree;

  /// Calibration constant: tabulated arrays of function parameters.
  CaloRec::Array<2> m_correction;

  /// Calibration constant: table of energies at which the correction
  /// was tabulated.
  CaloRec::Array<1> m_energies;

  /// Calibration constant: degree of the polynomial interpolation in energy.
  int               m_energy_degree;

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  bool m_use_raw_eta;
};


#endif // not CALOCLUSTERCORRECTION_CALOSWCLCON_V2_H


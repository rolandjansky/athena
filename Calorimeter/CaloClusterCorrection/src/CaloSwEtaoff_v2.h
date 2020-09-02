// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwEtaoff_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date July, 2004
 * @brief EM calorimeter eta offset (S-shape) corrections.
 *
 * The measured cluster eta positions are biased towards the centers
 * of the cells.  This correction attempts to remove this bias.
 */

#ifndef CALOCLUSTERCORRECTION_CALOSWETAOFF_V2_H
#define CALOCLUSTERCORRECTION_CALOSWETAOFF_V2_H

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


class CaloDetDescrElement;


/**
 * @class CaloSwEtaoff_v2
 * @brief EM calorimeter eta offset (S-shape) corrections.
 *
 * The measured cluster eta positions are biased towards the centers
 * of the cells.  This correction attempts to remove this bias.
 *
 * If you plot @f$\Delta\eta = \eta_{\rm true} - \eta_{\rm measured}@f$
 * as a function of @f$\eta@f$, the result is nearly periodic in the
 * cell structure.  We model this by binning the plots in @f$|\eta|@f$,
 * fitting an empirical function to the bins, and then interpolating
 * the fit parameters in @f$|\eta|@f$ in order to extend the function
 * over the entire range.
 *
 * The function that is fit derives from a form suggested by
 * Nicolas Kerschen.  Let the offset of the cluster within the
 * cell be @f$u@f$, where @f$u=0@f$ is at the center, and
 * @f$u=\pm 1@f$ are the edges.  Then, @f[
 *
 *     f(u) = A\tan^{-1} Bu + Cu + D|u| + E.
 *
 * @f]  We modify this function to ensure continuity by requiring
 * that @f$f(-1)=f(1)=0@f$.  We also redefine the variables in order
 * to remove correlations between them that cause the fit to behave
 * poorly.  (@f$A@f$ and @f$B@f$ are highly correlated in the above,
 * since they both strongly affect the total amplitude of the function.
 * We take the terms involving @f$A@f$ and @f$B@f$, find the
 * @f$u@f$ positions where this part of the function are at extrema,
 * and divide these terms by their value at these points,
 * with @f$A@f$ factored out.)  Signs are also adjusted so that
 * they are usually all positive.  After the constraints are imposed,
 * there are three parameters remaining: @f[
 *
 *           f(u) = A\left[{u\tan^{-1} B - \tan^{-1} Bu \over
 *                      (Z/B)\tan^{-1} B - \tan^{-1} Z} + C(1-|u|)\right],
 *
 * @f] where @f[
 *
 *           Z = \sqrt{B\over \tan^{-1} B - 1}.
 *
 * @f]  This is the form that we fit.
 *
 * The parameters are tabulated as a function of energy;
 * a second interpolation in energy is done on the parameters.
 */
class CaloSwEtaoff_v2
  : public CaloClusterCorrectionCommon
{
public:
  /// Inherit constructor.
  using CaloClusterCorrectionCommon::CaloClusterCorrectionCommon;
  

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
  virtual void makeTheCorrection (const Context& myctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const override;

private:
  /// Calibration constant: tabulated arrays of function parameters.
  Constant<CxxUtils::Array<3> > m_correction
  { this, "correction", "Tabulated arrays of function parameters." };

  /// Calibration constant: allow breaking up the interpolation into
  /// independent regions.
  Constant<CxxUtils::Array<1> > m_interp_barriers
  { this, "interp_barriers", "Allow breaking up the interpolation into independent regions." };

  /// Calibration constant: degree of the polynomial interpolation.
  Constant<int> m_degree
  { this, "degree", "Degree of the polynomial interpolation." };

  /// Calibration constant: table of energies at which the correction
  /// was tabulated.
  Constant<CxxUtils::Array<1> > m_energies
  { this, "energies", "Table of energies at which the correction was tabulated." };

  /// Calibration constant: degree of the polynomial interpolation in energy.
  Constant<int> m_energy_degree
  { this, "energy_degree", "Degree of the polynomial interpolation in energy." };
};


#endif // not CALOCLUSTERCORRECTION_CALOSWETAOFF_V2_H

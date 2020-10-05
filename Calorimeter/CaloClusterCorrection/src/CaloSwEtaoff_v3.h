// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwEtaoff_v3.h
 * @author scott snyder <snyder@bnl.gov>
 * @date July, 2006
 * @brief EM calorimeter eta offset (S-shape) corrections, version 3.
 *
 * The measured cluster eta positions are biased towards the centers
 * of the cells.  This correction attempts to remove this bias.
 */

#ifndef CALOCLUSTERCORRECTION_CALOSWETAOFF_V3_H
#define CALOCLUSTERCORRECTION_CALOSWETAOFF_V3_H

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


class CaloDetDescrElement;


/**
 * @class CaloSwEtaoff_v3
 * @brief EM calorimeter eta offset (S-shape) corrections, version 3.
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
 * For purposes of this correction, the calorimeter is divided
 * into disjoint regions in @f$\eta@f$; the correction is applied separately
 * in each region.  The region boundaries were set by looking
 * at where the behavior of the @f$\eta@f$ correction changes
 * discontinuously (so the regions are large in uniform regions
 * and small near cracks).
 *
 * Within each region, the correction is based on an empirical
 * function of the offset @f$u@f$ of the cluster within the cell,
 * where @f$u=0@f$ is at the center, and @f$u=\pm 1@f$ are the edges.
 * The function also depends on a set of parameters, @f$A@f$, @f$B@f$,
 * and so on.
 * Each of these parameters is then taken as a polynomial
 * in @f$ \eta @f$, with coefficients @f$ A_i @f$, @f$ B_i @f$, and so on.  These coefficients
 * are the parameters of the correction.  There is one set
 * of these coefficients per region per energy.
 *
 * There are several different empirical functions implemented.
 * Each is identified by a small integer number.
 * (Note that some functional forms tried in the analysis were
 * not used in the final result and were not implemented here;
 * thus, there are gaps in the numbering.)
 *
 * Form 0 is as follows: @f[
 *
 *      f(u) = A\left[\tan^{-1} Bu + C_{\pm}u + E\right],
 *
 * @f] where @f$C_{\pm}=C@f$ if @f$u\ge0@f$ and @f$C_{\pm}=D@f$
 * if @f$u<0@f$.
 *
 * The second implemented form is form 3.  This form
 * is the same as Form 0 with the constraint @f$f(-1)=f(1)@f$: @f[
 *
 *      f(u) = A\left[\tan^{-1} Bu + C_{\pm}u + E\right],
 *
 * @f] where @f$C_{\pm}=C@f$ if @f$u\ge0@f$ and
 * @f$C_{\pm}=-C-2\tan^{-1} B@f$ if @f$u<0@f$.
 *
 * The next implemented form is form 4.   This form
 * is the same as used in the V2 corrections; it is equivalent
 * to Form 0 with the constraints @f$f(-1)=f(1)=0@f$: @f[
 *
 *           f(u) = A\left[{u\tan^{-1} B - \tan^{-1} Bu \over
 *                      (Z/B)\tan^{-1} B - \tan^{-1} Z} + C(1-|u|)\right],
 *
 * @f] where @f[
 *
 *           Z = \sqrt{B\over \tan^{-1} B - 1}.
 *
 * @f]
 *
 * The next implemented form is form 5.   This form is a simple
 * polynomial in @f$|\eta|@f$
 *
 * The next implemented form is form 10.   This is based
 * on form 3, with the addition of a sinusoidal term, to absorb
 * remaining residual.  In addition, linear terms are added
 * in the regions @f$|u|>0.9@f$ in order to assure continuity
 * at the edges.  So: @f[
 *
 *      f(u) = A\left[\tan^{-1} Bu + C_{\pm}u + E\right] +
 *             \alpha\cos(\beta\pi u + \gamma) + \Delta,
 *
 * @f], where @f$C_{\pm}=C@f$ if @f$u\ge0@f$ and
 * @f$C_{\pm}=-C-2\tan^{-1} B@f$ if @f$u<0@f$; 
 * @f$\delta=\alpha(\cos(\beta\pi+\gamma)-\cos(-\beta\pi+\gamma))@f$;
 * and @f$\Delta= - (u-0.9)/0.1\times \delta/2@f$ if $u>0.9$,
 * @f$\Delta= (-0.9-u)/0.1\times \delta/2@f$ if $u<-0.9$,
 * and @f$\Delta=0@f$ otherwise.
 *
 * The next implemented form is form 11.   This is based
 * on form 0, with the addition of a sinusoidal term, to absorb
 * remaining residual.  So: @f[
 *
 *      f(u) = A\left[\tan^{-1} Bu + C_{\pm}u + E\right] +
 *             \alpha\cos(\beta\pi u + \gamma),
 *
 * @f], where @f$C_{\pm}=C@f$ if @f$u\ge0@f$ and @f$C_{\pm}=D@f$
 * if @f$u<0@f$.
 *
 * To apply the correction, we first find the proper region.
 * Then, we loop over each energy for which the correction
 * was tabulated.  For some energies, we may have not had
 * enough statistics to get a reliable fit; those energies
 * are skipped.  Otherwise, we evaluate the polynomials for
 * the function parameters and then evaluate the empirical
 * function.  This gives us a list of corrections per energy.
 * To get the final correction, we interpolate using the
 * cluster energy.  (The energy should already have had
 * at least a crude correction.)
 */
class CaloSwEtaoff_v3
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
  /**
   * @class CaloSwEtaoff_v3::Builder
   * @brief Helper class for calculating the energy interpolation table.
   */
  class Builder
    : public TableBuilder
  {
  public:
    /// Constructor.  Gets correction information,
    /// the abs(eta) at which the correction is being
    /// evaluated (in cal-local coordinates), the fractional
    /// cell offset u, and the index of the region for this
    /// correction.
    Builder (const CxxUtils::Array<4>& correction,
             const CxxUtils::Array<2>& regions,
             const CxxUtils::Array<2>& forms,
             float aeta,
             float u,
             int region_ndx);

    /// Calculate the correction for tabulated energy ENERGY_NDX.
    virtual float calculate (int energy_ndx, bool& good) const;

  private:
    /// Evaluate the function Form 0.
    float calc0 (float aeta, float u, const CaloRec::Array<2>& coef) const;

    /// Evaluate the function Form 3.
    float calc3 (float aeta, float u, const CaloRec::Array<2>& coef) const;

    /// Evaluate the function Form 4.
    float calc4 (float aeta, float u, const CaloRec::Array<2>& coef) const;

    /// Evaluate the function Form 5.
    float calc5 (float aeta, float u, const CaloRec::Array<2>& coef) const;

    /// Evaluate the function Form 10.
    float calc10 (float aeta, float u, const CaloRec::Array<2>& coef) const;

    /// Evaluate the function Form 11.
    float calc11 (float aeta, float u, const CaloRec::Array<2>& coef) const;

    /// Evaluate the function Form 13.
    float calc13 (float aeta, float u, const CaloRec::Array<2>& coef, float xlo, float xhi) const;

    // Tabulated arrays of function parameters.
    const CxxUtils::Array<4>& m_correction;

    /// Table of regions.
    const CxxUtils::Array<2>& m_regions;

    /// Functional form per region.
    const CxxUtils::Array<2>& m_forms;

    /// The index of the region in which we're evaluating the correction.
    int m_region_ndx;

    /// The abs(eta) at which the correction is being
    /// evaluated (in cal-local coordinates).
    float m_aeta;

    /// The fractional offset in the cell of this cluster.
    float m_u;

    /// The index of the functional form we're evaluating.
    int m_form;
  };
  friend class Builder;

  /// Find the index of the region containing a given @f$\eta@f$ value.
  int find_region (const CxxUtils::Array<2>& regions,
                   float aeta) const;

  /// Calibration constant: tabulated arrays of function parameters.
  /// Index 0: energy
  /// Index 1: region
  /// Index 2: parameter
  /// Index 3: polynomial coefficient
  ///
  /// Note: If [e][r][0][-1] (where -1 means the last in the array)
  /// is exactly 0, for any e and r, then this energy/region is skipped.
  /// (This is usually because insufficient statistics were available
  /// to get a good fit.)
  Constant<CxxUtils::Array<4> > m_correction
  { this, "correction", "Tabulated arrays of function parameters." };

  /// Calibration constant: table of regions.  For each region, we have:
  ///  - Lower @f$|\eta|@f$ for the region.
  ///  - Upper @f$|\eta|@f$ for the region.
  ///  - Index of the functional form for this region.
  ///  - Cell size for this region.
  ///  - Offset of cell low edge wrt a multiple of the cell size
  ///    (not currently used).
  ///
  /// If the forms variable is provided, then the functional form is
  /// taken from there instead of from here.
  Constant<CxxUtils::Array<2> > m_regions
  { this, "regions", "Table of regions." };

  enum {
    REG_LO = 0,
    REG_HI = 1,
    REG_FORM = 2,
    REG_CELLSIZE = 3,
    REG_PHASE = 4,
    REG_ENTRIES = 5 // Number of entries expected.
  };

  /// Calibration constant: table of energies at which the correction
  /// was tabulated.
  Constant<CxxUtils::Array<1> > m_energies
  { this, "energies", "Table of energies at which the correction was tabulated." };

  /// Calibration constant: degree of the polynomial interpolation in energy.
  Constant<int> m_energy_degree
  { this, "energy_degree", "Degree of the polynomial interpolation in energy." };

  /// Calibration constant: Functional form to use per region per energy.
  /// If this is empty, the form is taken instead from the region table
  /// (and is thus same for  all energies).
  Constant<CxxUtils::Array<2> > m_forms
  { this, "forms", "Functional form to use per region per energy." };
};


#endif // not CALOCLUSTERCORRECTION_CALOSWETAOFF_V3_H

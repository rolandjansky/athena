// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwPhimod_v2.h,v 1.5 2008-01-25 04:14:21 ssnyder Exp $
/**
 * @file  CaloSwPhimod_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2004
 * @brief EM calorimeter phi modulation corrections.
 *
 * Correct for energy modulations in phi.
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


#ifndef CALOCLUSTERCORRECTION_CALOSWPHIMOD_V2_H
#define CALOCLUSTERCORRECTION_CALOSWPHIMOD_V2_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


/**
 * @class CaloSwPhimod_v2
 * @brief EM calorimeter @f$\phi@f$ modulation corrections.
 *
 * The energy response of the calorimeter varies slightly depending
 * on the position of the particle impact in @f$\phi@f$ relative
 * to the accordion structure.  We try to remove these modulations
 * as best we can.  This is a small correction (< 1%).  We're further
 * hampered in doing a good job by the fact that the wavelength
 * of these modulations in @f$\phi@f$ is on the order of a few
 * mrad, which is similar to the @f$\phi@f$ resolution.  This leads
 * to the modulations being washed out.
 *
 * Nevertheless, we proceed by plotting the ratio
 * @f$E_{\rm meas}/E_{\rm true}@f$ vs.
 * the @f$\phi@f$ offset within an absorber, all binned
 * in @f$\eta@f$.  (In order to get the modulations to show up,
 * when making the profile plots, you need to plot the peaks
 * from Gaussian fits to the distributions, not the means.)
 * In each @f$\eta@f$ bin, we then fit a 2-term Fourier series,
 * parametrized like this: @f[
 *
 *   1 + A\left[\alpha\cos(N\phi+C) + (1-\alpha)\cos(2N\phi+D)\right],
 *
 * @f] where @f$\alpha = \tan^{-1} B/\pi + 1/2@f$ and @f$N@f$ is the
 * total number of absorbers in @f$2\pi@f$ (either 1024 or 768).
 *
 * The correction is tabulated at several different energies;
 * we then do a final interpolation in energy to find the final
 * correction.  (Note, however, that the observed energy dependence
 * seems to come mainly from the energy dependence of the
 * @f$\phi@f$ resolution.  It would be better to separate out this
 * effect in a future version.)
 *
 * These corrections are presently done before the gross energy
 * corrections, done later.  But in order to get decent results
 * out of the energy interpolation done here, we need to get
 * a better estimate of the energy.  So we have a crude response
 * correction built-in here, which is used to correct the cluster
 * energy for the purpose of energy interpolation.  (This can hopefully
 * be removed in a future version which does the corrections in a
 * more logical order.)
 */
class CaloSwPhimod_v2
: public CaloClusterCorrectionCommon
{
public:

  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloSwPhimod_v2(const std::string& type,
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
  /**
   * @class CaloSwPhimod_v2::Builder
   * @brief Helper class for calculating the energy interpolation table.
   */
  class Builder
    : public TableBuilder
  {
  public:
    /// Constructor.  Gets the parent correction object,
    /// the abs(eta) and phi at which the correction is being
    /// evaluated (in cal-local coordinates), and the number
    /// of absorbers.
    Builder (const CaloSwPhimod_v2& corr, float aeta, float phi, int nabs);

    /// Calculate the correction for tabulated energy ENERGY_NDX.
    virtual float calculate (int energy_ndx, bool& good) const;

  private:
    /// The parent correction object.
    const CaloSwPhimod_v2& m_corr;

    /// The abs(eta) at which the correction is being
    /// evaluated (in cal-local coordinates).
    float m_aeta;

    /// The cluster phi.
    float m_phi;

    /// Number of absorbers in @f$2\pi@f$.
    int m_nabs;
  };
  friend class Builder;

  /// Calibration constant: tabulated arrays of function parameters.
  CaloRec::Array<3> m_correction;

  /// Calibration constant: allow breaking up the interpolation into
  /// independent regions.
  CaloRec::Array<1> m_interp_barriers;

  /// Calibration constant: degree of the polynomial interpolation.
  int               m_degree;

  /// Calibration constant: coefficient by which to scale the
  /// entire correction.
  float             m_correction_coef;

  // Calibration constant: Correction factors for crude containment
  // correction used internally for the energy interpolation.
  CaloRec::Array<2> m_rfac;

  // Calibration constant: Interpolation degree for crude containment
  // correction used internally for the energy interpolation.
  int               m_rfac_degree;

  /// Calibration constant: table of energies at which the correction
  /// was tabulated.
  CaloRec::Array<1> m_energies;

  /// Calibration constant: degree of the polynomial interpolation in energy.
  int               m_energy_degree;

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  bool m_use_raw_eta;
};


#endif // not CALOCLUSTERCORRECTION_CALOSWPHIMOD_V2_H

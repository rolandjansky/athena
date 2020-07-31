// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwPhioff_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date July, 2004
 * @brief EM calorimeter phi offset corrections.
 *
 * Correct for the eta-dependent bias in phi.
 *
 * Derived from previous versions:
 * AUTHORS:  H. Ma, S. Rajagopalan
 * CREATED:  Dec. 15, 1999
 *
 * PURPOSE:  correction for the phi offset due to accordion structure
 *           base class: CaloClusterCorrectionCommon
 *
 * Atrecon Orig: emreco/qphioff.F
 *
 * Updated:  Feb 11, 2003   (MW)
 *           new correction for DC1 data
 *
 * Updated:  May 5, 2004    (Sven Menke)
 *	     base class changed from algo to tool
 *
 * Updated:  June, 2004    (sss)
 *           Use ToolWithConstants to get correction constants.
 */


#ifndef CALOCLUSTERCORRECTION_CALOSWPHIOFF_V2_H
#define CALOCLUSTERCORRECTION_CALOSWPHIOFF_V2_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


/**
 * @class CaloSwPhioff_v2.
 * @brief EM calorimeter @f$\phi@f$ offset corrections.
 *
 * The correction is an interpolation in a table, as a function
 * of abs(eta).  One thing to be careful about: in geant 4, this
 * correction has opposite sign for eta>0 and eta<0.  For geant 3,
 * it had the same sign in both halves.  This behavior is controlled
 * by m_flip_phi.
 *
 * The correction parameters are tabulated by energy.  A second interpolation
 * is done on the cluster energy.
 */
class CaloSwPhioff_v2
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
   * @class CaloSwPhioff_v2::Builder
   * @brief Helper class for calculating the energy interpolation table.
   */
  class Builder
    : public TableBuilder
  {
  public:
    /// Constructor.  Gets the correction information,
    /// and the abs(eta) at which the correction is being
    /// evaluated (in cal-local coordinates).
    Builder (const CxxUtils::Array<3>& correction,
             const CxxUtils::Array<1>& interp_barriers,
             int degree,
             float aeta);

    /// Calculate the correction for tabulated energy ENERGY_NDX.
    virtual float calculate (int energy_ndx, bool& good) const;

  private:
    /// Tabulated arrays of function parameters.
    const CxxUtils::Array<3> m_correction;

    /// Allow breaking up the interpolation into independent regions.
    const CxxUtils::Array<1> m_interp_barriers;

    /// Degree of the polynomial interpolation.
    int m_degree;

    /// The abs(eta) at which the correction is being
    /// evaluated (in cal-local coordinates).
    float m_aeta;
  };

  /// Calibration constant: tabulated arrays of function parameters.
  Constant<CaloRec::Array<3> > m_correction
  { this, "correction", "Tabulated arrays of function parameters." };

  /// Calibration constant: allow breaking up the interpolation into
  /// independent regions.
  Constant<CxxUtils::Array<1> > m_interp_barriers
  { this, "interp_barriers", "Allow breaking up the interpolation into independent regions." };

  /// Calibration constant: degree of the polynomial interpolation.
  Constant<int> m_degree
  { this, "degree", "Degree of the polynomial interpolation." };

  /// Calibration constant: coefficient by which to scale the
  /// entire correction.
  Constant<float> m_correction_coef
  { this, "correction_coef", "Coefficient by which to scale the entire correction." };

  /// Calibration constant: if true, the correction gets an opposite
  /// sign if eta < 0.
  Constant<bool> m_flip_phi
  { this, "flip_phi", "If true, the correction gets an opposite sign if eta < 0." };

  /// Calibration constant: table of energies at which the correction
  /// was tabulated.
  Constant<CxxUtils::Array<1> > m_energies
  { this, "energies", "Table of energies at which the correction was tabulated." };

  /// Calibration constant: degree of the polynomial interpolation in energy.
  Constant<int> m_energy_degree
  { this, "energy_degree", "Degree of the polynomial interpolation in energy." };
};


#endif // not CALOCLUSTERCORRECTION_CALOSWPHIOFF_V2_H

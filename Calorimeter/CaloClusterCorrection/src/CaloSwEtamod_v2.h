// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloSwEtamod_v2.h,v 1.5 2008-01-25 04:14:21 ssnyder Exp $
/**
 * @file  CaloSwEtamod_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2004
 * @brief EM calorimeter @f$\eta@f$ modulation corrections.
 *
 * Correct for energy modulations in eta.
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


#ifndef CALOCLUSTERCORRECTION_CALOSWETAMOD_V2_H
#define CALOCLUSTERCORRECTION_CALOSWETAMOD_V2_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


/**
 * @class CaloSwEtamod_v2.
 * @brief EM calorimeter @f$\eta@f$ modulation corrections.
 *
 * In addition to the @f$\phi@f$ modulations, there is also a small
 * variation in the response which depends on the position of the
 * particle impact in @f$\eta@f$ within a cell.  This is also
 * a small correction, on the order of tenths of a percent.
 *
 * The form of this correction is generally quadratic.
 * However, near cracks, the modulation features get severely distorted
 * by the effects of the cracks.  Hence, when we derive this correction,
 * we (for now) ignore the regions near cracks, and consider only the
 * `uniform' regions of the calorimeter.  We use the same binning
 * as for the @f$\phi@f$ modulation.  When we fit a quadratic to the
 * modulation signal in all the uniform bins, we find that the fit
 * parameters are mostly consistent across all the uniform bins.
 * Therefore, we combine all these bins and perform a single,
 * @f$\eta@f$-independent fit.
 *
 * The correction is tabulated at several different energies;
 * we then do a final interpolation in energy to find the final
 * correction.
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
class CaloSwEtamod_v2
: public CaloClusterCorrectionCommon
{
public:

  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloSwEtamod_v2 (const std::string& type,
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
   * @class CaloSwEtamod_v2::Builder
   * @brief Helper class for calculating the energy interpolation table.
   */
  class Builder
    : public TableBuilder
  {
  public:
    /// Constructor.  Gets the parent correction object,
    /// and the eta offset within the cell.
    Builder (const CaloSwEtamod_v2& corr, float etamod);

    /// Calculate the correction for tabulated energy ENERGY_NDX.
    virtual float calculate (int energy_ndx, bool& good) const;

  private:
    /// The parent correction object.
    const CaloSwEtamod_v2& m_corr;

    /// The eta offset within the cell.
    float m_etamod;
  };
  friend class Builder;

  /// Calibration constant: tabulated arrays of function parameters.
  CaloRec::Array<2> m_correction;

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
};


#endif // not CALOCLUSTERCORRECTION_CALOSWETAMOD_V2_H

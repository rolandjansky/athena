// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloClusterCorrection/CalOSwTransitionRegionsCorr.h
 * @author L. Carminati
 * @date December, 2004
 * @brief Energy corrections for the transition regions at |eta|=0 and 0.8.
 */

#ifndef CALOCLUSTERCORRECTION_CALOSWTRANSITIONREGIONSCORR_H
#define CALOCLUSTERCORRECTION_CALOSWTRANSITIONREGIONSCORR_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"

class CaloSwTransitionRegionsCorr
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
  /// Calibration constants: The range over which this correction is defined.
  Constant<float>  m_etamin_TR00
  { this, "etamin_TR00", "The range over which this correction is defined." };
  Constant<float>  m_etamax_TR00
  { this, "etamax_TR00", "The range over which this correction is defined." };
  Constant<float>  m_etamin_TR08  
  { this, "etamin_TR08", "The range over which this correction is defined." };
  Constant<float>  m_etamax_TR08
  { this, "etamax_TR08", "The range over which this correction is defined." };

  /// Calibration constant: The tabulated array of correction parameters.
  Constant<CxxUtils::Array<2> > m_correction
  { this, "correction", "The tabulated array of correction parameters." };

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  Constant<bool> m_use_raw_eta
  { this, "use_raw_eta", "If true, tabulated values are in terms of raw (local) eta." };
};


#endif // not CALOCLUSTERCORRECTION_CALOSWTRANSITIONREGIONSCORR_H

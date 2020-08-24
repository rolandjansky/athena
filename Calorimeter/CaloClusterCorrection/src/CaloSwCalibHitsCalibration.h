/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWCALIBHITSCALIBRATION_H
#define CALOCLUSTERCORRECTION_CALOSWCALIBHITSCALIBRATION_H
/********************************************************************

NAME:     CaloSwCalibHitsCalibration.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  L. Carminati
CREATED:  January 4, 2007

PURPOSE:  Calibrate SW em clusters with coefficients extracted
          from Calibration Hits.
          
********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloClusterCorrection/CaloSwCalibHitsShowerDepth.h"
#include "CaloConditions/Array.h"

class CaloSwCalibHitsCalibration : public CaloClusterCorrectionCommon
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
  Constant<CxxUtils::Array<3> >  m_correction    { this, "correction", "" };
  Constant<CxxUtils::Array<2> >  m_sampling_depth{ this, "sampling_depth", "" };
  Constant<float>             m_eta_start_crack { this, "eta_start_crack", "" };
  Constant<float>             m_eta_end_crack   { this, "eta_end_crack",   "" };
  Constant<float>             m_etamax          { this, "etamax",          "" };
  Constant<bool>              m_use_raw_eta     { this, "use_raw_eta",     "" };
  Constant<bool>  m_fix_v6_pathologies     { this, "fix_v6_pathologies", "" };
  Constant<bool>  m_updateSamplingEnergies { this, "update_sampling_energies", true, "" };

  CaloClusterCorr::CaloSwCalibHitsShowerDepth m_showerDepth;
};

#endif


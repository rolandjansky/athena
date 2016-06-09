/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  // constructor 
  CaloSwCalibHitsCalibration (const std::string& type,
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


  CaloRec::Array<3>     m_correction;
  CaloRec::Array<2>     m_sampling_depth;
  float m_eta_start_crack;
  float m_eta_end_crack;
  float m_etamax;
  //int m_degree;
  bool m_use_raw_eta;
  //bool m_preserve_offset;
  bool m_fix_v6_pathologies;
  bool m_updateSamplingEnergies;

  CaloClusterCorr::CaloSwCalibHitsShowerDepth m_showerDepth;
};

#endif


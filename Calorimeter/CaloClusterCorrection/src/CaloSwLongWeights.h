/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWLONGWEIGHTS_H
#define CALOCLUSTERCORRECTION_CALOSWLONGWEIGHTS_H
/********************************************************************

NAME:     CaloSwLongWeights.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  K.Loureiro, S.Paganis
CREATED:  July 5, 2004

PURPOSE:  Longitudinal weight corrections applied to calo clusters 
          

********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


class CaloSwLongWeights : public CaloClusterCorrectionCommon
{
public:
  // constructor 
  CaloSwLongWeights (const std::string& type,
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


  CaloRec::Array<2>     m_correction;
  float m_eta_start_crack;
  float m_eta_end_crack;
  float m_etamax;
  int m_degree;
  bool m_use_raw_eta;
  bool m_preserve_offset;

};

#endif


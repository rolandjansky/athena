/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOTOPOEMLONGWEIGHTS_H
#define CALOCLUSTERCORRECTION_CALOTOPOEMLONGWEIGHTS_H
/********************************************************************

NAME:     CaloTopoEMLongWeights.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  N.Kerschen
CREATED:  Nov 8, 2006

PURPOSE:  Longitudinal weight corrections applied to topoEM 
          

********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


class CaloTopoEMLongWeights : public CaloClusterCorrectionCommon
{
public:
  // constructor 
  CaloTopoEMLongWeights (const std::string& type,
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
                                  CaloSampling::CaloSample samp) const;


 private:


  CaloRec::Array<2>     m_correction;
  float m_eta_start_crack;
  float m_eta_end_crack;
  float m_etamax;
  int m_degree;
  bool m_use_raw_eta;
};

#endif


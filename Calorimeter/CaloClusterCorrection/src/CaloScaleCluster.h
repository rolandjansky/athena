// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  CaloScaleCluster.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2006
 * @brief Scale cluster energy by an eta-dependent factor.
 *
 * This correction scales the total cluster energy by a factor
 * that depends on eta (but not on energy).
 */


#ifndef CALOCLUSTERCORRECTION_CALOSCALECLUSTER_H
#define CALOCLUSTERCORRECTION_CALOSCALECLUSTER_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"


/**
 * @class CaloScaleCluster.
 * @brief EM calorimeter containment correction.
 *
 * This correction scales the total cluster energy by a factor
 * that depends on eta (but not on energy).  Note that only the
 * total energy is changed; the individual sampling energies
 * are not changed.
 */
class CaloScaleCluster
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
  Constant<CxxUtils::Array<2> > m_correction
  { this, "correction", "Arrays of function parameters" };

  /// Calibration constant: degree of the polynomial interpolation.
  Constant<int> m_degree
  { this, "degree", "Degree of the polynomial interpolation" };

  /// Calibration constant: maximum eta for which this correction is defined.
  Constant<float> m_etamax
  { this, "etamax", "Maximum eta for which this correction is defined" };
};


#endif // not CALOCLUSTERCORRECTION_CALOSCALECLUSTER_H


/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * AUTHORS:  N. Kerschen
 * CREATED:  Nov 2006
 *
 * PURPOSE:  Correction for the barrel-endcap cracks
 *           for topoEM cluster (based on CaloSwGap_v2.h)
 *
 */


#ifndef CALOCLUSTERCORRECTION_CALOTOPOEMGAP_H
#define CALOCLUSTERCORRECTION_CALOTOPOEMGAP_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"
#include "StoreGate/ReadHandleKey.h"


class CaloCellContainer;


class CaloTopoEMGap
  : public CaloClusterCorrectionCommon
{
public:
  /// Inherit constructor.
  using CaloClusterCorrectionCommon::CaloClusterCorrectionCommon;


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


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
  Constant<float> m_etamin_crack
  { this, "etamin_crack", "Lower edge of range over which this correction is defined." };
  Constant<float> m_etamax_crack
  { this, "etamax_crack", "Upper edge of range over which this correction is defined." };

  /// Calibration constant: The interpolation degree.
  Constant<int> m_degree
  { this, "degree", "The interpolation degree." };

  /// Calibration constant: The tabulated array of correction weights,
  /// A and alpha.
  Constant<CxxUtils::Array<2> > m_correction
  { this, "correction", "The tabulated array of correction weights, A and alpha." };

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  Constant<bool> m_use_raw_eta
  { this, "use_raw_eta", "If true, tabulated values are in terms of raw (local) eta." };

  SG::ReadHandleKey<CaloCellContainer> m_cells
  { this, "Cells", "AllCalo", "SG key for calorimeter cells" };
};


#endif // not CALOCLUSTERCORRECTION_CALOSWGAP_V2_H

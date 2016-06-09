/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
 
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloTopoEMGap(const std::string& type,
               const std::string& name,
               const IInterface* parent);


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


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
  /// Calibration constants: The range over which this correction is defined.
  float             m_etamin_crack;
  float             m_etamax_crack;

  /// Calibration constant: The interpolation degree.
  int               m_degree;

  /// Calibration constant: The tabulated array of correction weights,
  /// A and alpha.
  CaloRec::Array<2> m_correction;

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  bool m_use_raw_eta;

  SG::ReadHandleKey<CaloCellContainer> m_cells;
};


#endif // not CALOCLUSTERCORRECTION_CALOSWGAP_V2_H

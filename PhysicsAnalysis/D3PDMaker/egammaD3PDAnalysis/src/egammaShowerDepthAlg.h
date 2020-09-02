// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file egammaD3PDAnalysis/src/egammaShowerDepthAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2010
 * @brief Store in UserData the shower depth for an egamma object.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMASHOWERDEPTHALG_H
#define EGAMMAD3PDANALYSIS_EGAMMASHOWERDEPTHALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloClusterCorrection/CaloSwCalibHitsShowerDepth.h"
#include "CaloUtils/ToolWithConstants.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Store in UserData the shower depth for an egamma object.
 */
class egammaShowerDepthAlg
  : public CaloUtils::ToolWithConstants<AthAlgorithm>
{
public:
  /// Inherit constructor.
  using base_class::base_class;


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize() override;


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute() override;


private:
  /// Property: Prefix to add to aux data items.
  StringProperty m_auxPrefix
  { this, "AuxPrefix", "", "Prefix to add to aux data items." };

  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter
  { this, "Getter", "", "Getter instance for the input egamma objects." };

  /// Property: If true, don't complain if input objects are missing.
  BooleanProperty m_allowMissing
  { this, "AllowMissing", false, "If true, don't complain if input objects are missing." };

  /// Property: Table of sampling depth weights.
  Constant<CxxUtils::Array<2> > m_sampling_depth { this, "sampling_depth" };

  /// Property: Eta of the start of the crack.
  Constant<float> m_eta_start_crack { this, "eta_start_crack" };

  /// Property: Eta of the end of the crack.
  Constant<float> m_eta_end_crack { this, "eta_end_crack" };

  /// Property: Maximum eta range of the depth weight table.
  Constant<float> m_etamax { this, "etamax" };

  /// Property: Use raw eta value for region comparisons?
  // FIXME: The false setting of this is not implemented.
  Constant<bool> m_use_raw_eta { this, "use_raw_eta" };

  /// Depth calculator.
  CaloClusterCorr::CaloSwCalibHitsShowerDepth m_depthCalc;
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_EGAMMASHOWERDEPTHALG_H

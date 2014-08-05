// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
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
#include "CaloRec/ToolWithConstantsMixin.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Store in UserData the shower depth for an egamma object.
 */
class egammaShowerDepthAlg
  : public AthAlgorithm,
    public CaloRec::ToolWithConstantsMixin
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaShowerDepthAlg (const std::string& name,
                        ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Method to set a property value.
   * @param p The property name/value to set.
   * @return Gaudi status code.
   *
   * Required by @c ToolWithConstantsMixin.
   */
  using AthAlgorithm::setProperty;
  StatusCode setProperty (const Property& p);
  StatusCode setProperty (const std::string& propname,
                          const std::string& value);


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

  /// Property: Table of sampling depth weights.
  CaloRec::Array<2> m_sampling_depth;

  /// Property: Eta of the start of the crack.
  float m_eta_start_crack;

  /// Property: Eta of the end of the crack.
  float m_eta_end_crack;

  /// Property: Maximum eta range of the depth weight table.
  float m_etamax;

  /// Property: Use raw eta value for region comparisons?
  // FIXME: The false setting of this is not implemented.
  bool m_use_raw_eta;

  /// Depth calculator.
  CaloClusterCorr::CaloSwCalibHitsShowerDepth m_depthCalc;
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_EGAMMASHOWERDEPTHALG_H

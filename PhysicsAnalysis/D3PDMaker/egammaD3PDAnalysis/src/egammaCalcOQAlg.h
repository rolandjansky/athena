// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaCalcOQAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Calculate egamma OQ flags and store as UserData.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMACALCOQALG_H
#define EGAMMAD3PDANALYSIS_EGAMMACALCOQALG_H


#include "D3PDMakerUtils/UDAlgBase.h"
#include "GaudiKernel/ToolHandle.h"


class IegammaBaseTool;


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Calculate egamma OQ flags and store as UserData.
 */
class egammaCalcOQAlg
  : public UDAlgBase
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaCalcOQAlg (const std::string& name,
                   ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

  /// Property: Tool to recalculate OQ flags.
  ToolHandle<IegammaBaseTool> m_egammaOQFlagsBuilder;
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_EGAMMACALCOQALG_H

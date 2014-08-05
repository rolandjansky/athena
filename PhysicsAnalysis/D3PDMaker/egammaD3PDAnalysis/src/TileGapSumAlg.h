// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/TileGapSumAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Sum tile gap cells in a rectangular region around clusters.
 */


#ifndef EGAMMAD3PDANALYSIS_TILEGAPSUMALG_H
#define EGAMMAD3PDANALYSIS_TILEGAPSUMALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Sum tile gap cells in a rectangular region around clusters
 *        and store as a decoration.
 */
class TileGapSumAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  TileGapSumAlg (const std::string& name,
                 ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing; 

  /// Property: Getter for input cluster objects.
  ToolHandle<ICollectionGetterTool> m_clusterGetter;

  /// Property: Getter for input cell objects.
  /// Should have already been selected for only tile gap cells.
  ToolHandle<ICollectionGetterTool> m_cellGetter;
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_TILEGAPSUMALG_H

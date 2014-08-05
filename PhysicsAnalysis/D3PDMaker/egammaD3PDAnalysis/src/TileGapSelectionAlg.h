// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/TileGapSelectionAlg.h>
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Select tile gap cells from a cell container into a new container.
 */


#ifndef EGAMMAD3PDANALYSIS_TILEGAPSELECTIONALG_H
#define EGAMMAD3PDANALYSIS_TILEGAPSELECTIONALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace D3PD {


class IObjGetterTool;


/**
 * @brief Select tile gap cells from a cell container into a new container.
 */
class TileGapSelectionAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  TileGapSelectionAlg (const std::string& name,
                       ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Getter for the input collection.
  ToolHandle<IObjGetterTool> m_getter;

  /// Property: SG key for output cell container.
  std::string m_outputCellContainerName;
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_TILEGAPSELECTIONALG_H

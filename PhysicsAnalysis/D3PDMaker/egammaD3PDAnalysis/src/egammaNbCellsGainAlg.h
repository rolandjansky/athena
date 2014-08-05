// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaNbCellsGainAlg.h 604352 2014-07-01 04:52:11Z ssnyder $
/**
 * @file egammaD3PDAnalysis/src/egammaNbCellsGainAlg.h
 * @date Aug. 2013
 * @brief Number of cells with given gain by layer
 */


#ifndef EGAMMANBCELLSGAINALG_H
#define EGAMMANBCELLSGAINALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Count the number of cells in each gain, layer by layer
 */
class egammaNbCellsGainAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaNbCellsGainAlg (const std::string& name,
			 ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

};


} // namespace D3PD


#endif // not EGAMMANBCELLSGAINALG_H

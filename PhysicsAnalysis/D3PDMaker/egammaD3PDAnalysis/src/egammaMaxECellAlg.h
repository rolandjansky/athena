// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaMaxECellAlg.h 604352 2014-07-01 04:52:11Z ssnyder $
/**
 * @file egammaD3PDAnalysis/src/egammaMaxECellAlg.h
 * @author Nikiforos K. Nikiforou <nikiforo@cern.ch> adapted from various tools by Scott Snyder and others
 * @date Apr, 2012
 * @brief Find max E cell in middle layer for an egamma object EM Cluster and store in UserData.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMAMAXECELLALG_H
#define EGAMMAD3PDANALYSIS_EGAMMAMAXECELLALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>
#include <string>


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Find most energetic cell in the middle layer for an egamma object and store in UserData.
 */
class egammaMaxECellAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaMaxECellAlg (const std::string& name,
                     ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_EGAMMAMAXECELLALG_H

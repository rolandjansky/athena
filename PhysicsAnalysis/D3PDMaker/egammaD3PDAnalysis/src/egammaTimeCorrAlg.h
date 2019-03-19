// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaTimeCorrAlg.h 604352 2014-07-01 04:52:11Z ssnyder $
/**
 * @file egammaD3PDAnalysis/src/egammaTimeCorrAlg.h
 * @author Nikiforos K. Nikiforou <nikiforo@cern.ch> adapted from various tools by Scott Snyder
 * @date Apr, 2012
 * @brief Calculate corrected time for an egamma object and store in UserData.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMATIMECORRALG_H
#define EGAMMAD3PDANALYSIS_EGAMMATIMECORRALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "VxVertex/VxContainer.h"   
#include "VxVertex/RecVertex.h"

#include "CaloInterface/ICaloClusterTimeTool.h"

#include <vector>
#include <string>


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Calculate corrected time for an egamma object and store in UserData.
 */
class egammaTimeCorrAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaTimeCorrAlg (const std::string& name,
                     ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Vertex container name (to correct for TOF difference).
  std::string m_vertexContainerName;

  /// Property: Getter for input egamma objects.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

  /// Property: Cluster Time corrector tool
  ToolHandle<ICaloClusterTimeTool> m_caloClusterTimeTool; 
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_EGAMMATIMECORRALG_H

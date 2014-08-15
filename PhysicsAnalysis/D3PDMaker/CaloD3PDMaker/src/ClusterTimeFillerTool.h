// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterTimeFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for time information from a CaloCluster.
 */

#ifndef CALOD3PDMAKER_CLUSTERTIMEFILLERTOOL_H
#define CALOD3PDMAKER_CLUSTERTIMEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class CaloCluster;


namespace D3PD {


/**
 * @brief Block filler tool for time information from a CaloCluster.
 */
class ClusterTimeFillerTool
  : public BlockFillerTool<CaloCluster>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ClusterTimeFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCluster& p);


private:
  /// Variable: Cluster time.
  float* m_time;
};


} // namespace D3PD


#endif // not CALOD3PDMAKER_CLUSTERTIMEFILLERTOOL_H

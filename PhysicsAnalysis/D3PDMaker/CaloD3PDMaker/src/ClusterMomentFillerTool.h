// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterMomentFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for CaloCluster moments.
 */

#ifndef CALOD3PDMAKER_CLUSTERMOMENTFILLERTOOL_H
#define CALOD3PDMAKER_CLUSTERMOMENTFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "CaloEvent/CaloClusterMoment.h"
class CaloCluster;


namespace D3PD {


/**
 * @brief Block filler tool for CaloCluster moments.
 */
class ClusterMomentFillerTool
  : public BlockFillerTool<CaloCluster>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ClusterMomentFillerTool (const std::string& type,
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
  /// Parameter: (enum number, name) pairs describing the variables to write.
  std::vector<std::string> m_moments;


  /// Describe one variable.
  struct Var
  {
    /// Variable enum.
    CaloClusterMoment::MomentType key;

    /// Variable pointer.
    float* x;
  };

  /// List of variables we're writing.
  std::vector<Var> m_vars;
};


} // namespace D3PD


#endif // not CALOD3PDMAKER_CLUSTERMOMENTFILLERTOOL_H

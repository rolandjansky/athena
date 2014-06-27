// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/ClusterFillerTool.h
 * @brief Block filler tool for a four-momentum.
 */

#ifndef PHOTOND3PDMAKER_CLUSTERFILLERTOOL_H
#define PHOTOND3PDMAKER_CLUSTERFILLERTOOL_H

#include <vector>

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "CaloGeoHelpers/CaloSampling.h"

class CaloCluster;


namespace D3PD {


  /**
   * @brief Block filler tool for a cluster
   *
   */
class ClusterFillerTool
  : public BlockFillerTool<CaloCluster>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    ClusterFillerTool (const std::string& type,
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
    virtual StatusCode fill (const CaloCluster& c);


  private:
    /// Property: Signal state of the object to read.
    int m_signalState;

    /// Variable: Energy.
    float* m_E;

    /// Variable: Transverse energy (E*sin(theta)).
    float* m_Et;

    /// Variable: Pseudorapidity.
    float* m_eta;

    /// Variable: Azimuth.
    float* m_phi;

    /// Variable: Available calo samplings
    std::vector<CaloSampling::CaloSample> m_caloSamplings;

    /// Property: samplings to consider
    std::vector<int> m_useCaloSamplings;

  };


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_CLUSTERFILLERTOOL_H

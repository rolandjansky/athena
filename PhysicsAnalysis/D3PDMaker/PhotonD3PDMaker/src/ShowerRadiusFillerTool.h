// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/ShowerRadiusFillerTool.h
 * @brief Block filler tool for shower radius in layer 2
 */

#ifndef PHOTOND3PDMAKER_SHOWERRADIUSFILLERTOOL_H
#define PHOTOND3PDMAKER_SHOWERRADIUSFILLERTOOL_H

#include <vector>
#include "D3PDMakerUtils/BlockFillerTool.h"

class CaloCluster;

namespace D3PD {

  /**
   * @brief Block filler tool for a cluster
   *
   */
class ShowerRadiusFillerTool
  : public BlockFillerTool<CaloCluster>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    ShowerRadiusFillerTool (const std::string& type,
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
    /// Variable: radius 2nd layer.
    float* m_radius_s1;
    float* m_radius_s2;

  };

} // namespace D3PD


#endif // not PHOTOND3PDMAKER_SHOWERRADIUSFILLERTOOL_H

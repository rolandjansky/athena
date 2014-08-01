/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/L2MuonCB1FillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for CombinedMuonFeature, detail level 1
 */

#ifndef MUOND3PDMAKER_L2MUONCB1FILLERTOOL_H
#define MUOND3PDMAKER_L2MUONCB1FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class CombinedMuonFeature;


namespace D3PD {


/**
 * @brief Block filler tool for CombinedMuonFeature detail level 1.
 */
class L2MuonCB1FillerTool
  : public BlockFillerTool<CombinedMuonFeature>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  L2MuonCB1FillerTool (const std::string& type,
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
  virtual StatusCode fill (const CombinedMuonFeature& p);


private:
  /// Variable: pt of ID track
  float* m_id_pt;
  /// Variable: pt of MuFast track
  float* m_ms_pt;
};


} // namespace D3PD


#endif 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/L2MuonCB2FillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for CombinedMuonFeature, detail level 2
 */

#ifndef MUOND3PDMAKER_L2MUONCB2FILLERTOOL_H
#define MUOND3PDMAKER_L2MUONCB2FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class CombinedMuonFeature;


namespace D3PD {


/**
 * @brief Block filler tool for CombinedMuonFeature, detail level 2
 */
class L2MuonCB2FillerTool
  : public BlockFillerTool<CombinedMuonFeature>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  L2MuonCB2FillerTool (const std::string& type,
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
  /// Variable: number of pixel space points for ID track
  int* m_NPixelSpacePoints;

  /// Variable: number of SCT space points for ID track
  int* m_NSCT_SpacePoints;

  /// Variable: number of straw hits for ID track
  int* m_NStrawHits;

  /// Variable: number of high-threshold hits for ID track
  int* m_NTRHits;

};


} // namespace D3PD


#endif 

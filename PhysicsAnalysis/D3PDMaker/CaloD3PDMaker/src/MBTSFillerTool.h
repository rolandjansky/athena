// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/MBTSFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for MBTS information.
 */

#ifndef CALOD3PDMAKER_MBTSFILLERTOOL_H
#define CALOD3PDMAKER_MBTSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class TileCell;


namespace D3PD {


/**
 * @brief Block filler tool for MBTS information.
 */
class MBTSFillerTool
  : public BlockFillerTool<TileCell>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MBTSFillerTool (const std::string& type,
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
  virtual StatusCode fill (const TileCell& c);


private:
  /// Variable: Counter energy.
  float* m_E;

  /// Variable: Counter eta.
  float* m_eta;

  /// Variable: Counter phi.
  float* m_phi;

  /// Variable: Counter time.
  float* m_time;

  /// Variable: Counter quality.
  int* m_quality;

  /// Variable: Counter type.  +-1, depending on side.
  int* m_type;

  /// Variable: Counter module.  0-7, depending on phi.
  int* m_module;

  /// Variable: Counter channel.  0-1, depending on eta.
  /// Zero is closer to the beam pipe.
  int* m_channel;

  bool m_saveEtaPhi;
};


} // namespace D3PD


#endif // not CALOD3PDMAKER_MBTSFILLERTOOL_H

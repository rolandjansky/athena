// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMbD3PDMaker/fillers/BCM_RawDataFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2010
 * @brief Block filler tool for BCM_RawData objects.
 */


#ifndef TRIGMBD3PDMAKER_BCM_RAWDATAFILLERTOOL_H
#define TRIGMBD3PDMAKER_BCM_RAWDATAFILLERTOOL_H



#include "D3PDMakerUtils/BlockFillerTool.h"
class BCM_RawData;


namespace TrigMbD3PD {


/**
 * @brief Block filler tool for BCM_RawData objects.
 */
class BCM_RawDataFillerTool
  : public D3PD::BlockFillerTool<BCM_RawData>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  BCM_RawDataFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param bcm The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const BCM_RawData& bcm);


private:
  /// Variable: BCM channel.
  int* m_channel;

  /// Variable: Position of the first pulse.
  int *m_pulse1pos;

  /// Variable: Width of the first pulse.
  int *m_pulse1width;

  /// Variable: Position of the second pulse.
  int *m_pulse2pos;

  /// Variable: Width of the second pulse.
  int *m_pulse2width;

  /// Variable: Level-1 accept.
  int *m_lvl1a;

  /// Variable: Bunch crossing ID.
  int *m_bcid;

  /// Variable: Level 1 ID.
  int *m_lvl1id;
};


} // namespace TrigMbD3PD


#endif // not TRIGMBD3PDMAKER_BCM_RAWDATAFILLERTOOL_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/MBTSFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Block filler tool for combined MBTS time information.
 */


#ifndef CALOD3PDMAKER_MBTSTIMEFILLERTOOL_H
#define CALOD3PDMAKER_MBTSTIMEFILLERTOOL_H


#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
class MBTSTimeFilterTool;


namespace D3PD {


/**
 * @brief Block filler tool for combined MBTS time information.
 */
class MBTSTimeFillerTool
  : public BlockFillerTool<void>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MBTSTimeFillerTool (const std::string& type,
                      const std::string& name,
                      const IInterface* parent);


  /// Standard Gaudi initialize method.
  StatusCode initialize();


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   */
  virtual StatusCode fill ();


private:
  /// Property: The tool to calculate the time information.
  ToolHandle<MBTSTimeFilterTool> m_timetool;

  /// Variable: MBTS A-C time difference.
  float* m_timeDiff;

  /// Variable: MBTS A-side time.
  float* m_timeA;

  /// Variable: MBTS C-side time.
  float* m_timeC;

  /// Variable: Number of A-side MBTS counters used for time.
  int* m_countA;

  /// Variable: Number of C-side MBTS counters used for time.
  int* m_countC;
};


} // namespace D3PD


#endif // not CALOD3PDMAKER_MBTSTIMEFILLERTOOL_H

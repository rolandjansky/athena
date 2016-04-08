// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BackgroundWordFillerTool.h 596570 2014-05-12 12:32:04Z wlampl $
/**
 * @file BackgroundD3PDMaker/src/BackgroundWordFillerTool.h
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Mar, 2011
 * @brief Block filler tool for background event flag from EventInfo.
 */


#ifndef BACKGROUNDD3PDMAKER_BACKGROUNDWORDFILLERTOOL_H
#define BACKGROUNDD3PDMAKER_BACKGROUNDWORDFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEventInfo/EventInfo.h"
#include <vector>


namespace D3PD {


/**
 * @brief Block filler tool for background event flag info from EventInfo.
 */
class BackgroundWordFillerTool
  : public BlockFillerTool<xAOD::EventInfo>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  BackgroundWordFillerTool (const std::string& type,
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
  virtual StatusCode fill (const xAOD::EventInfo& p);


private:
  unsigned int* m_bkgword;
  std::vector<bool*> m_setbits;
};


} // namespace D3PD


#endif // not BACKGROUNDD3PDMAKER_BACKGROUNDWORDFILLERTOOL_H

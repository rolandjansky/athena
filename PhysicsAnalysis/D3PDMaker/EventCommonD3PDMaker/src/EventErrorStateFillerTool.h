// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/EventErrorStateFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Block filler tool for event error state from @c EventInfo.
 */


#ifndef EVENTCOMMOND3PPDMAKER_EVENTERRORSTATEFILLERTOOL_H
#define EVENTCOMMOND3PPDMAKER_EVENTERRORSTATEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEventInfo/EventInfo.h"
#include <vector>


namespace D3PD {


/**
 * @brief Block filler tool for event error state from @c EventInfo.
 */
class EventErrorStateFillerTool
  : public BlockFillerTool<xAOD::EventInfo>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  EventErrorStateFillerTool (const std::string& type,
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
  std::vector<unsigned int*> m_errs;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_EVENTERRORSTATEFILLERTOOL_H

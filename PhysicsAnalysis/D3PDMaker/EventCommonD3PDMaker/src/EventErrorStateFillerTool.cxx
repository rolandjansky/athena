/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/EventErrorStateFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Block filler tool for event error state from @c EventInfo.
 */


#include "EventErrorStateFillerTool.h"
#include "EventFlagNames.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"
#include <string>
#include <cassert>

using xAOD::EventInfo;

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
EventErrorStateFillerTool::EventErrorStateFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
  : BlockFillerTool<EventInfo> (type, name, parent)
{
}


/**
 * @brief Book variables for this block.
 */
StatusCode EventErrorStateFillerTool::book()
{
  m_errs.resize (EventInfo::nDets);
  for (size_t i = 0; i < nEventFlagNames; i++) {
    unsigned int flag = eventFlagNames[i].m_flag;
    assert (flag < m_errs.size());
    std::string name = eventFlagNames[i].m_name;
    name += "Error";
    CHECK( addVariable (name, m_errs[flag]) );
  }
  return StatusCode::SUCCESS;
}


StatusCode EventErrorStateFillerTool::fill (const EventInfo& p)
{
  for (size_t i = 0; i < m_errs.size(); i++) {
    if (m_errs[i])
      *m_errs[i] = p.errorState(static_cast<EventInfo::EventFlagSubDet>(i));
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD

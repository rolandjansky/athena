/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BackgroundWordFillerTool.cxx 596570 2014-05-12 12:32:04Z wlampl $
/**
 * @file BackgroundD3PDMaker/src/BackgroundWordFillerTool.cxx
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Mar, 2011
 * @brief Block filler tool for background flag info from EventInfo.
 */


#include "BackgroundWordFillerTool.h"
#include "BackgroundWordNames.h"
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
BackgroundWordFillerTool::BackgroundWordFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<EventInfo> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode BackgroundWordFillerTool::book()
{

  m_setbits.resize(EventInfo::NBackgroundWords);
  for(size_t i=0; i<nBackgroundWordNames;++i){
    unsigned int flag=backgroundWordNames[i].m_flag;
    assert( flag<m_setbits.size() );
    std::string name = backgroundWordNames[i].m_name;
    CHECK( addVariable ( name, m_setbits[flag]) );
  }
 
  CHECK( addVariable ("BkgWord", m_bkgword, "Event Info Background Word") );

  return StatusCode::SUCCESS;
}


StatusCode BackgroundWordFillerTool::fill (const EventInfo& p)
{

  *m_bkgword = p.eventFlags(EventInfo::Background);
  
  for(size_t i=0; i<m_setbits.size();++i){
    if(m_setbits[i])
      *m_setbits[i]=p.isEventFlagBitSet(EventInfo::Background,static_cast<EventInfo::BackgroundEventFlag>(i));
  }
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD

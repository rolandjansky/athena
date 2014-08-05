/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/PileUpInfoFillerTool.cxx
 * @author Jacob Searcy <jsearcy1@uoregon.edu>
 * @date Jul, 2010
 * @brief Block filler tool for @c PileUpInfo information.
 */


#include "PileUpInfoFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventID.h"


namespace D3PD {
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PileUpInfoFillerTool::PileUpInfoFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : BlockFillerTool<PileUpEventInfo::SubEvent> (type, name, parent)
{
  declareProperty ("WriteEventNumbers",        m_do_EvtNum    = true);
  book().ignore(); // Avoid coverity warnings.
}

/**
 * @brief Book variables for this block.
 */
StatusCode PileUpInfoFillerTool::book()
{
  CHECK( addVariable ("time",    m_time, "Event time") );
  CHECK( addVariable ("index",   m_index, "Event index") );
  CHECK( addVariable ("type",    m_type,
                      "Pileup type provenance.  See PileUpTimeEventIndex.") );
  if(m_do_EvtNum){
    CHECK( addVariable ("runNumber",     m_run_number, "Event Run Number") );
    CHECK( addVariable ("EventNumber",   m_event_number, "Event Number") );
  }
  return StatusCode::SUCCESS;
}

  StatusCode PileUpInfoFillerTool::fill (const PileUpEventInfo::SubEvent& p)
{

  const EventInfo* sevt = p.pSubEvt;
  *m_run_number   = sevt->event_ID()->run_number();
  *m_event_number = sevt->event_ID()->event_number();
  *m_time  = p.time();
  *m_index = p.index();
  *m_type  = p.type();

  return StatusCode::SUCCESS;
}


} // namespace D3PD

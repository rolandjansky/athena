/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/TriggerBitFillerTool.cxx
 * @author Haifeng Li <Haifeng.Li@cern.ch>, sss
 * @date Sep 03, 2009
 * @brief Block filler tool for trigger decisions.
 */


#include "TriggerBitFillerTool.h"
#include "TriggerD3PDMaker/TriggerKeyTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "boost/foreach.hpp"
#include <algorithm>



namespace D3PD {


/**
 * @brief Block filler tool for trigger decisions.
 */
TriggerBitFillerTool::TriggerBitFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
    m_trigDec ("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_incidentSvc ("IncidentSvc", name),
    m_triggerKeyTool (),
    m_allowExtend (false)
{
  declareProperty("Triggers",         m_triggers,
                  "Input list of trigger names to add to the tuple. "
                  "May contain regexps.");
  declareProperty ("TrigDecisionTool", m_trigDec,
                   "The TrigDecisionTool instance.");
  declareProperty ("IncidentSvc", m_incidentSvc,
                   "The IncidentSvc instance.");
  declareProperty ("AllowExtend", m_allowExtend,
                   "Allow adding to the list of bits if the menu changes?");
  declareProperty ("TriggerKeyTool", m_triggerKeyTool,
                   "TriggerKeyTool instance.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TriggerBitFillerTool::initialize()
{
  CHECK( BlockFillerTool<void>::initialize() );
  CHECK( m_trigDec.retrieve() );
  CHECK( m_incidentSvc.retrieve() );
  CHECK( m_triggerKeyTool.retrieve() );

  // If we want to allow adding new bits midway through,
  // hook up to listen to the TrigConf incident, which is raised
  // when the trigger menu is changed.
  if (m_allowExtend)
    m_incidentSvc->addListener (this, "TrigConf");

  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TriggerBitFillerTool::book()
{
  // Be careful --- this may be called more than once (from handle()).

  const Trig::ChainGroup* cg = m_trigDec->getChainGroup (m_triggers);
  std::vector<std::string> names = cg->getListOfTriggers();

  // Need to remove duplicates.
  std::sort (names.begin(), names.end());
  std::vector<std::string>::iterator end = std::unique (names.begin(),
                                                        names.end());

  for (std::vector<std::string>::iterator name = names.begin();
       name != end;
       ++name)
  {
    std::pair<TrigNameMap::iterator, bool> res =
      m_trigNameMap.insert (std::make_pair (*name,
                                            static_cast<bool*>(0)));
    if (res.second) {
      CHECK(  addVariable( *name, res.first->second,
                           "Did this event pass trigger " + *name)  );
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block.
 *
 * This is called once per event.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TriggerBitFillerTool::fill ()
{
  BOOST_FOREACH (const TrigNameMap::value_type& b, m_trigNameMap) {
    *b.second = m_trigDec->isPassed (b.first);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief This will be called when the trigger configuration changes.
 */
void TriggerBitFillerTool::handle (const Incident& /*inc*/)
{
  // The trigger configuration has changed.
  // If we haven't seen this configuration before, then rerun book()
  // to pick up any new bits.
  std::string key = m_triggerKeyTool->getString();
  if (m_keysSeen.insert (key).second) {
    StatusCode sc = book();
    if (sc.isFailure()) {
      REPORT_ERROR (sc) << "Can't book new bits.";
    }
  }
}


} // namespace D3PD

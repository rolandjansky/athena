/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/TriggerObjectDecisionFillerToolImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a given trigger object:
 *         non-template dependent portion.
 */


#include "TriggerD3PDMaker/TriggerObjectDecisionFillerToolImpl.h"
#include "TriggerD3PDMaker/TriggerKeyTool.h"
#include "D3PDMakerUtils/BlockFillerToolImpl.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/foreach.hpp"
#include <algorithm>


namespace D3PD {


/**
 * @brief Constructor.
 * @param tool The tool holding this object.
 * @param name The tool name.
 */
TriggerObjectDecisionFillerToolImpl::TriggerObjectDecisionFillerToolImpl
  (BlockFillerToolImpl& tool, const std::string& name)
    : m_trigDec ("Trig::TrigDecisionTool/TrigDecisionTool"),
      m_matchTool ("TrigMatchTool/TrigMatchTool"),
      m_incidentSvc ("IncidentSvc", name),
      m_triggerKeyTool (),
      m_allowExtend (false),
      m_tool (tool)
{
  tool.declareProperty ("Chains", m_chains,
                 "List of chain names for which we should record a decision. "
                 "May include regexps.");
  tool.declareProperty("TrigDecisionTool", m_trigDec,
                       "The TrigDecisionTool instance.");
  tool.declareProperty ("MatchTool", m_matchTool,
                        "Trigger object matching tool.");
  tool.declareProperty ("IncidentSvc", m_incidentSvc,
                        "The IncidentSvc instance.");
  tool.declareProperty ("AllowExtend", m_allowExtend,
                        "Allow adding to the list of bits "
                        "if the menu changes?");
  tool.declareProperty ("TriggerKeyTool", m_triggerKeyTool,
                        "TriggerKeyTool instance.");
}


/**
 * @brief Initialize the object.
 */
StatusCode TriggerObjectDecisionFillerToolImpl::initialize()
{
  CHECK( m_trigDec.retrieve() );
  CHECK( m_matchTool.retrieve() );
  CHECK( m_incidentSvc.retrieve() );
  CHECK( m_triggerKeyTool.retrieve() );

  // If we want to allow adding new bits midway through,
  // hook up to listen to the TrigConf incident, which is raised
  // when the trigger menu is changed.
  if (m_allowExtend) {
    IIncidentListener* iil = dynamic_cast<IIncidentListener*> (&m_tool);
    if (!iil) {
      StatusCode sc = StatusCode::FAILURE;
      REPORT_ERROR(sc) << "Tool doesn't derive from IInterfaceListener.";
      return sc;
    }
    m_incidentSvc->addListener (iil, "TrigConf");
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TriggerObjectDecisionFillerToolImpl::book()
{
  // Be careful --- this may be called more than once (from handle()).

  const Trig::ChainGroup* cg = m_trigDec->getChainGroup (m_chains);
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
      CHECK(  m_tool.addVariable( *name, res.first->second,
                                  "Did this object pass the chain "
                                  + *name) );
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill variables for one object.
 * @param chains List of chain names which the object passed.
 */
StatusCode
TriggerObjectDecisionFillerToolImpl::fill
   (const std::vector<std::string>& chains)
{
  for (size_t i = 0; i < chains.size(); i++) {
    TrigNameMap::iterator pos = m_trigNameMap.find (chains[i]);
    if (pos != m_trigNameMap.end())
      *pos->second = true;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the matching tool.
 */
ToolHandle<TrigMatchTool>& TriggerObjectDecisionFillerToolImpl::matchTool()
{
  return m_matchTool;
}


/**
 * @brief This will be called when the trigger configuration changes.
 */
void TriggerObjectDecisionFillerToolImpl::handle (const Incident& /*inc*/)
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

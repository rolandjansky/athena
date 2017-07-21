/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/OptimizedScheduler.h>

#include <QuickAna/EventData.h>
#include <QuickAna/IAnaTool.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/OptimizedObjects.h>
#include <QuickAna/OptimizedStore.h>
#include <QuickAna/OptimizedTool.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void OptimizedScheduler ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  OptimizedScheduler ::
  OptimizedScheduler (const std::string& name)
    : AsgTool (name), m_runNumber (0), m_eventNumber (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  OptimizedScheduler ::
  ~OptimizedScheduler ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  StatusCode OptimizedScheduler ::
  initialize()
  {
    RCU_CHANGE_INVARIANT (this);
    ATH_MSG_DEBUG("initialize");

    std::stable_sort (m_tools.begin(), m_tools.end(),
		      [] (const std::unique_ptr<OptimizedTool>& a,
                          const std::unique_ptr<OptimizedTool>& b)
                      {return a->step() < b->step();});

    ATH_MSG_INFO ("tracing tool dependencies");
    for (auto& tool1 : m_tools)
    {
      for (auto& tool2 : m_tools)
      {
        // TODO: This should have comments
	bool tool2InputForTool1 = tool1->hasDirectInput (*tool2);
	bool tool1InputForTool2 = tool2->hasDirectInput (*tool1);
	RCU_ASSERT (!(tool1InputForTool2 && tool2InputForTool1));
	if (tool1InputForTool2)
	  tool1->addOutput (&*tool2);
	else if (tool2InputForTool1)
	  tool2->addOutput (&*tool1);
      }
    }

    ATH_MSG_INFO ("sorting tools by dependencies");
    std::stable_sort (m_tools.begin(), m_tools.end(),
		      [] (const std::unique_ptr<OptimizedTool>& a,
                          const std::unique_ptr<OptimizedTool>& b)
                      {return a->hasOutput (&*b);});

    std::vector<CP::SystematicSet> systematics;
    systematics.push_back (CP::SystematicSet ());
    return setSystematics (systematics);
  }



  StatusCode OptimizedScheduler ::
  addTool (std::unique_ptr<IAnaTool> tool)
  {
    RCU_CHANGE_INVARIANT (this);
    std::unique_ptr<OptimizedTool> mytool
      (new OptimizedTool (std::move (tool)));
    m_tools.push_back (std::move (mytool));
    return StatusCode::SUCCESS;
  }



  CP::SystematicSet OptimizedScheduler ::
  affectingSystematics () const
  {
    RCU_READ_INVARIANT (this);

    CP::SystematicSet result;
    for (auto& tool : m_tools)
    {
      result.insert (tool->affectingSystematics());
    }
    return result;
  }



  CP::SystematicSet OptimizedScheduler ::
  recommendedSystematics () const
  {
    RCU_READ_INVARIANT (this);

    CP::SystematicSet result;
    for (auto& tool : m_tools)
    {
      result.insert (tool->recommendedSystematics());
    }
    return result;
  }



  StatusCode OptimizedScheduler ::
  applySystematicVariation (const CP::SystematicSet& systConfig)
  {
    RCU_CHANGE_INVARIANT (this);

    auto iter = m_objectsMap.find (systConfig);
    if (iter == m_objectsMap.end())
    {
      ATH_MSG_ERROR ("unknown systematic configuration: " << systConfig.name());
      return StatusCode::FAILURE;
    }
    m_currentObjects = iter->second.get();
    return StatusCode::SUCCESS;
  }



  StatusCode OptimizedScheduler ::
  fillEventObjects (IEventObjects*& objects)
  {
    RCU_CHANGE_INVARIANT (this);
    ATH_MSG_DEBUG("fillEventObjects");

    if (isNewEvent())
    {
      m_master->clearData ();
      for (auto& tool : m_tools)
      {
	ATH_CHECK (tool->execute ());
      }
    }
    objects = m_currentObjects;
    return StatusCode::SUCCESS;
  }



  bool OptimizedScheduler ::
  isNewEvent ()
  {
    const xAOD::EventInfo *eventInfo = nullptr;
    if (!evtStore()->retrieve (eventInfo, "EventInfo").isSuccess())
      RCU_THROW_MSG ("failed to retrieve EventInfo object");
    if (m_runNumber == eventInfo->runNumber() &&
	m_eventNumber == eventInfo->eventNumber())
      return false;
    m_runNumber = eventInfo->runNumber();
    m_eventNumber = eventInfo->eventNumber();
    return true;
  }



  const std::vector<CP::SystematicSet>& OptimizedScheduler ::
  systematics () const
  {
    RCU_READ_INVARIANT (this);
    return m_targetSystematics;
  }



  StatusCode OptimizedScheduler ::
  setSystematics (const std::vector<CP::SystematicSet>& val_systematics)
  {
    RCU_CHANGE_INVARIANT (this);

    m_targetSystematics = val_systematics;
    m_objectsMap.clear ();
    m_master.reset ();

    for (auto& sys : m_targetSystematics)
      ATH_MSG_INFO ("using systematic: \"" << sys.name() << "\"");
    m_master.reset (new OptimizedStore (m_targetSystematics, &*evtStore()));

    for (auto& tool : m_tools)
    {
      ATH_MSG_INFO ("using tool: " << tool->name() << " systematics=\"" <<
                    tool->affectingSystematics().name() << "\"");
      ATH_CHECK (tool->configureSystematics (&*m_master));
    }

    // TODO: This should have comments
    std::vector<OptimizedObjects*> myobjects;
    for (auto& sys : m_targetSystematics)
    {
      std::unique_ptr<OptimizedObjects> objects
	(new OptimizedObjects (&*m_master, sys));
      myobjects.push_back (&*objects);
      m_objectsMap.insert (std::make_pair (sys, std::move (objects)));
    }
    ATH_CHECK (m_master->initializeIndices (myobjects, m_master->objectTypes(),
                                            0, STEP_ANALYSIS));
    if (applySystematicVariation (CP::SystematicSet ()) != CP::SystematicCode::Ok)
    {
      ATH_MSG_ERROR ("failed to set default systematics");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  EventData OptimizedScheduler ::
  getEventData () const
  {
    RCU_READ_INVARIANT (this);
    EventData result;
    for (auto& sys : m_objectsMap)
    {
      sys.second->fillEventData (sys.first, result);
    }
    for (auto& tool : m_tools)
      tool->fillEventDataSource (result);
    return result;
  }
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/ToolScheduler.h>

#include <PATInterfaces/SystematicSet.h>
#include <QuickAna/EventData.h>
#include <QuickAna/IAnaTool.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void ToolScheduler ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  ToolScheduler ::
  ToolScheduler (const std::string& name)
    : AsgTool (name)
  {
    RCU_NEW_INVARIANT (this);
  }



  ToolScheduler ::
  ~ToolScheduler ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  StatusCode ToolScheduler ::
  initialize()
  {
    RCU_CHANGE_INVARIANT (this);
    ATH_MSG_DEBUG("initialize");
    m_systematics.push_back (CP::SystematicSet ());
    return StatusCode::SUCCESS;
  }



  StatusCode ToolScheduler ::
  addTool (std::unique_ptr<IAnaTool> tool)
  {
    RCU_CHANGE_INVARIANT (this);
    m_tools.push_back (std::move (tool));
    return StatusCode::SUCCESS;
  }



  CP::SystematicSet ToolScheduler ::
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



  CP::SystematicSet ToolScheduler ::
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



  StatusCode ToolScheduler ::
  applySystematicVariation (const CP::SystematicSet& systConfig)
  {
    RCU_CHANGE_INVARIANT (this);

    for (auto& tool : m_tools)
    {
      if (tool->applySystematicVariation (systConfig) !=
          CP::SystematicCode::Ok)
        return StatusCode::FAILURE;
    }
    m_postfix = systConfig.name();
    return StatusCode::SUCCESS;
  }



  StatusCode ToolScheduler ::
  fillEventObjects (IEventObjects*& objects)
  {
    RCU_CHANGE_INVARIANT (this);
    ATH_MSG_DEBUG("fillEventObjects");

    m_objects.clear (&*evtStore(), m_postfix);
    for (unsigned iter = 0; iter <= STEP_WEIGHT; ++ iter)
    {
      ATH_MSG_DEBUG("Executing step: " << iter);
      for (auto& tool : m_tools)
      {
        if (tool->step() == iter)
        {
          ATH_MSG_DEBUG("Executing tool: " << tool->name());
          try
          {
            ATH_CHECK (tool->execute (m_objects));
            if (tool->outputTypes() & (1 << OBJECT_EVENT_SELECT))
            {
              auto select = m_objects.eventSelect();
              if (select &&
                  select->auxdata<ana::SelectType> ("ana_select") == false)
              {
                objects = &m_objects;
                return StatusCode::SUCCESS;
              }
            }
          } catch (std::exception& e)
          {
            ATH_MSG_FATAL ("exception while executing tool " << tool->name() <<
                           ": " << e.what());
            return StatusCode::FAILURE;
          } catch (std::string& s)
          {
            ATH_MSG_FATAL ("exception while executing tool " << tool->name() <<
                           ": " << s);
            return StatusCode::FAILURE;
          } catch (...)
          {
            ATH_MSG_FATAL ("unknown exception while executing tool " << tool->name());
            return StatusCode::FAILURE;
          }
        }
      }
    }
    objects = &m_objects;
    return StatusCode::SUCCESS;
  }



  const std::vector<CP::SystematicSet>& ToolScheduler ::
  systematics () const
  {
    RCU_READ_INVARIANT (this);
    return m_systematics;
  }



  StatusCode ToolScheduler ::
  setSystematics (const std::vector<CP::SystematicSet>& val_systematics)
  {
    RCU_CHANGE_INVARIANT (this);
    m_systematics = val_systematics;
    return StatusCode::SUCCESS;
  }



  EventData ToolScheduler ::
  getEventData () const
  {
    RCU_READ_INVARIANT (this);
    EventData result;

    for (unsigned typeIter = 0; typeIter < EnumSize<ObjectType>::dataSize; ++ typeIter)
    {
      const ObjectType type = ObjectType (typeIter);
      bool hasType = false;
      for (auto& tool : m_tools)
      {
        if (tool->outputTypes() & (1 << type))
          hasType = true;
      }
      if (hasType)
      {
        for (auto& sys : m_systematics)
        {
          std::string anaName = "ana_" + ObjectTypeInfo::name[type] + "_" + sys.name();
          result.setName (type, sys, anaName);
        }
      }
    }
    for (auto& tool : m_tools)
      tool->fillEventDataSource (result);
    return result;
  }
}

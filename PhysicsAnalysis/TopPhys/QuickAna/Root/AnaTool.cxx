/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/AnaTool.h>
#include <QuickAna/InternalConfiguration.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void AnaTool ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  AnaTool ::
  AnaTool (const std::string& name)
    : AsgTool (name)
  {
    RCU_NEW_INVARIANT (this);
  }



  void AnaTool ::
  registerTool (IAsgTool *tool)
  {
    RCU_CHANGE_INVARIANT (this);

    CP::ISystematicsTool *systool
      = dynamic_cast<CP::ISystematicsTool*> (tool);
    if (systool)
    {
      ATH_MSG_INFO ("recovered missed ISystematicsTool: " << tool->name());
      registerTool (systool);
    } else
    {
      ATH_MSG_WARNING ("tool not ISystematicsTool: " << tool->name());
    }
  }



  void AnaTool ::
  registerTool (CP::ISystematicsTool *tool)
  {
    RCU_CHANGE_INVARIANT (this);
    m_systematicsTools.push_back (tool);
  }



  bool AnaTool ::
  isAffectedBySystematic (const CP::SystematicVariation& systematic) const
  {
    RCU_READ_INVARIANT (this);
    for (auto tool : m_systematicsTools)
    {
      if (tool->isAffectedBySystematic (systematic))
	return true;
    }
    return false;
  }



  CP::SystematicSet AnaTool ::
  affectingSystematics () const
  {
    RCU_READ_INVARIANT (this);
    CP::SystematicSet result;
    for (auto tool : m_systematicsTools)
    {
      result.insert (tool->affectingSystematics());
    }
    return result;
  }



  CP::SystematicSet AnaTool ::
  recommendedSystematics () const
  {
    RCU_READ_INVARIANT (this);
    CP::SystematicSet result;
    for (auto tool : m_systematicsTools)
    {
      result.insert (tool->recommendedSystematics());
    }
    return result;
  }



  CP::SystematicCode AnaTool ::
  applySystematicVariation (const CP::SystematicSet& systConfig)
  {
    RCU_CHANGE_INVARIANT (this);
    for (auto tool : m_systematicsTools)
    {
      if (tool->applySystematicVariation (systConfig) !=
	  CP::SystematicCode::Ok)
	return CP::SystematicCode::Unsupported;
    }
    return CP::SystematicCode::Ok;
  }



  void AnaTool ::
  fillEventDataSource (EventData& /*event*/) const
  {}



  StatusCode AnaTool ::
  useConfiguration (const InternalConfiguration& /*configuration*/)
  {
    return StatusCode::SUCCESS;
  }



  StatusCode AnaTool ::
  getInitialConfiguration (InternalConfiguration& /*conf*/)
  {
    return StatusCode::SUCCESS;
  }



  StatusCode AnaTool ::
  useInitialConfiguration (const InternalConfiguration& conf)
  {
    msg().setLevel( conf.msgLevel() );
    return StatusCode::SUCCESS;
  }
}

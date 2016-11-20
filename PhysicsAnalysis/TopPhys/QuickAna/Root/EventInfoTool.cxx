/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <QuickAna/EventInfoTool.h>

#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>

//
// method implementations
//

namespace ana
{
  EventInfoTool ::
  EventInfoTool (const std::string& name)
    : AsgTool (name), AnaTool (name)
  {}



  StatusCode EventInfoTool ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    if (type != OBJECT_EVENTINFO)
    {
      ATH_MSG_ERROR ("unsupported object type");
      return StatusCode::FAILURE;
    }
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point in event info tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode EventInfoTool ::
  initialize()
  {
    // will add tool later if needed

    return StatusCode::SUCCESS;
  }



  AnalysisStep EventInfoTool ::
  step () const
  {
    return STEP_CORRECT;
  }



  unsigned EventInfoTool ::
  inputTypes () const
  {
    return 1 << OBJECT_EVENTINFO;
  }



  unsigned EventInfoTool ::
  outputTypes () const
  {
    return 1 << OBJECT_EVENTINFO;
  }



  StatusCode EventInfoTool ::
  execute (IEventObjects& /*objects*/)
  {
    // will add decorations of new variables later

    return StatusCode::SUCCESS;
  }
}

namespace ana
{
  StatusCode makeEventInfoTool (DefinitionArgs& args)
  {
    args.add (std::unique_ptr<IAnaTool>
      (new AnaToolRetrieve (args.prefix() + "_retrieve", "EventInfo")));
    args.add (std::unique_ptr<EventInfoTool> (new EventInfoTool (args.prefix())));
    return StatusCode::SUCCESS;
  }

  QUICK_ANA_EVENTINFO_DEFINITION_MAKER ("default", makeEventInfoTool (args))
}

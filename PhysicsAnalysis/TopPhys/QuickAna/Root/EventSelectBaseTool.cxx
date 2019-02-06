/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/EventSelectBaseTool.h>

#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>

//
// method implementations
//

namespace ana
{
  EventSelectBaseTool ::
  EventSelectBaseTool (const std::string& name)
    : AsgTool (name), AnaTool (name)
  {
  }



  StatusCode EventSelectBaseTool ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    if (type != OBJECT_EVENT_SELECT)
    {
      ATH_MSG_ERROR ("unsupported object type");
      return StatusCode::FAILURE;
    }

    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point for event select base tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AnalysisStep EventSelectBaseTool ::
  step () const
  {
    return STEP_RETRIEVE;
  }



  StatusCode EventSelectBaseTool ::
  useConfiguration (const InternalConfiguration& configuration)
  {
    ATH_CHECK (AnaTool::useConfiguration (configuration));
    for (unsigned iter = 0; iter != EnumSize<SelectionStep>::dataSize; ++ iter)
      m_select.push_back (configuration.selectionName (SelectionStep (iter)));
    return StatusCode::SUCCESS;
  }



  unsigned EventSelectBaseTool ::
  inputTypes () const
  {
    return 0;
  }



  unsigned EventSelectBaseTool ::
  outputTypes () const
  {
    return 1 << OBJECT_EVENT_SELECT;
  }



  StatusCode EventSelectBaseTool ::
  execute (IEventObjects& objects)
  {
    ATH_CHECK (objects.addNew (OBJECT_EVENT_SELECT));

    EventSelectType *object = objects.eventSelect();
    for (auto& sel : m_select)
    {
      if (sel.isAvailable (*object))
      {
	ATH_MSG_ERROR ("selection field already available");
	return StatusCode::FAILURE;
      }
      sel (*object) = true;
    }

    return StatusCode::SUCCESS;
  }

}

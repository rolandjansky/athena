/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/AnaToolSelectInit.h>

#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>

//
// method implementations
//

namespace ana
{
  AnaToolSelectInit ::
  AnaToolSelectInit (const std::string& name)
    : AsgTool (name), AnaTool (name),
      m_type (OBJECT_JET)
  {}



  StatusCode AnaToolSelectInit ::
  useConfiguration (const InternalConfiguration& configuration)
  {
    ATH_CHECK (AnaTool::useConfiguration (configuration));
    for (unsigned iter = 0; iter != EnumSize<SelectionStep>::dataSize; ++ iter)
      m_select.push_back (configuration.selectionName (SelectionStep (iter)));
    return StatusCode::SUCCESS;
  }



  StatusCode AnaToolSelectInit ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    m_type = type;
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point when initializing objects");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AnalysisStep AnaToolSelectInit ::
  step () const
  {
    return STEP_SELECT_INIT;
  }



  unsigned AnaToolSelectInit ::
  inputTypes () const
  {
    return 1 << m_type;
  }



  unsigned AnaToolSelectInit ::
  outputTypes () const
  {
    return 1 << m_type;
  }



  StatusCode AnaToolSelectInit ::
  execute (IEventObjects& objects)
  {
    bool first = true;
    for (auto object : *objects.getParticles (m_type))
    {
      for (auto& sel : m_select)
      {
	if (first && sel.isAvailable (*object))
	{
	  ATH_MSG_ERROR ("selection field already available");
	  return StatusCode::FAILURE;
	}
	sel (*object) = true;
      }
      first = false;
    }
    return StatusCode::SUCCESS;
  }
}

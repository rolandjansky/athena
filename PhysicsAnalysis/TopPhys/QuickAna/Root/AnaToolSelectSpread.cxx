/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/AnaToolSelectSpread.h>

#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>

//
// method implementations
//

namespace ana
{
  AnaToolSelectSpread ::
  AnaToolSelectSpread (const std::string& name)
    : AsgTool (name), AnaTool (name),
      m_type (OBJECT_JET), m_first (false)
  {}



  AnaToolSelectSpread ::
  AnaToolSelectSpread (const std::string& name, const std::vector<std::string>& workingPoints, bool first)
    : AsgTool (name), AnaTool (name), m_workingPoints (workingPoints),
      m_type (OBJECT_JET), m_first (first)
  {}



  StatusCode AnaToolSelectSpread ::
  useConfiguration (const InternalConfiguration& configuration)
  {
    ATH_CHECK (AnaTool::useConfiguration (configuration));
    for (unsigned iter = 0; iter != EnumSize<SelectionStep>::dataSize; ++ iter)
    {
      m_data.push_back (Data (configuration, m_workingPoints,
			      SelectionStep (iter)));
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AnaToolSelectSpread ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    m_type = type;
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point when spreading working points");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AnalysisStep AnaToolSelectSpread ::
  step () const
  {
    return m_first ? STEP_SPLIT_SELECT : STEP_SPREAD_SELECT;
  }



  unsigned AnaToolSelectSpread ::
  inputTypes () const
  {
    return 1 << m_type;
  }



  unsigned AnaToolSelectSpread ::
  outputTypes () const
  {
    return 1 << m_type;
  }



  StatusCode AnaToolSelectSpread ::
  execute (IEventObjects& objects)
  {
    for (auto object : *objects.getParticles (m_type))
    {
      for (auto& data : m_data)
      {
	if (!data.select.isAvailable (*object))
	{
	  ATH_MSG_ERROR ("input field not available");
	  return StatusCode::FAILURE;
	}
	bool accept = data.select (*object);
	if (m_first || !accept)
	{
	  for (auto& sel : data.outputSelect)
	  {
	    sel (*object) = accept;
	  }
	}
      }
    }
    return StatusCode::SUCCESS;
  }



  AnaToolSelectSpread::Data ::
  Data (const InternalConfiguration& configuration,
	const std::vector<std::string>& workingPoints,
	SelectionStep step)
    : select (configuration.selectionName (step))
  {
    for (auto& wp : workingPoints)
      outputSelect.push_back (configuration.selectionName (step) + "_" + wp);
  }
}

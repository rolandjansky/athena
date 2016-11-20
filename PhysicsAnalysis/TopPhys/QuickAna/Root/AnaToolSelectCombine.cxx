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

#include <QuickAna/AnaToolSelectCombine.h>

#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>

//
// method implementations
//

namespace ana
{
  AnaToolSelectCombine ::
  AnaToolSelectCombine (const std::string& name)
    : AsgTool (name), AnaTool (name),
      m_type (OBJECT_JET)
  {}



  AnaToolSelectCombine ::
  AnaToolSelectCombine (const std::string& name, const std::vector<std::string>& workingPoints)
    : AsgTool (name), AnaTool (name), m_workingPoints (workingPoints),
      m_type (OBJECT_JET)
  {}



  StatusCode AnaToolSelectCombine ::
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



  StatusCode AnaToolSelectCombine ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    m_type = type;
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point when combining working points");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AnalysisStep AnaToolSelectCombine ::
  step () const
  {
    return STEP_SUPER_SELECT;
  }



  unsigned AnaToolSelectCombine ::
  inputTypes () const
  {
    return 1 << m_type;
  }



  unsigned AnaToolSelectCombine ::
  outputTypes () const
  {
    return 1 << m_type;
  }



  StatusCode AnaToolSelectCombine ::
  execute (IEventObjects& objects)
  {
    for (auto object : *objects.getParticles (m_type))
    {
      bool accept = true;
      for (auto& data : m_data)
      {
	if (accept)
	{
	  accept = false;
	  for (auto& sel : data.inputSelect)
	  {
	    if (!sel.isAvailable (*object))
	    {
	      ATH_MSG_ERROR ("input field not available");
	      return StatusCode::FAILURE;
	    }
	    if (sel (*object))
	      accept = true;
	  }
	}
	data.select (*object) = accept;
      }
    }
    return StatusCode::SUCCESS;
  }



  AnaToolSelectCombine::Data ::
  Data (const InternalConfiguration& configuration,
	const std::vector<std::string>& workingPoints,
	SelectionStep step)
    : select (configuration.selectionName (step))
  {
    for (auto& wp : workingPoints)
      inputSelect.push_back (configuration.selectionName (step) + "_" + wp);
  }
}

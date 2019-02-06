/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/KinSelectTool.h>

#include <QuickAna/IEventObjects.h>
#include <QuickAna/KinObjectSelect.h>

//
// method implementations
//

namespace ana
{
  KinSelectTool ::
  KinSelectTool (const std::string& name)
    : AsgTool (name), AnaTool (name)
  {}



  KinSelectTool ::
  ~KinSelectTool ()
  {}



  StatusCode KinSelectTool ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    m_objectType = type;
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point in event info tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode KinSelectTool ::
  initialize()
  {
    try
    {
      m_selectObject.reset (new KinObjectSelect (m_selection));
    } catch (std::exception& e)
    {
      ATH_MSG_ERROR ("failed to parse format \"" << m_selection << "\": " << e.what());
      return StatusCode::FAILURE;
    }
    m_selectionData.registerCut (SelectionStep::MET, "kin_select", cut_kin_select);
    return StatusCode::SUCCESS;
  }



  StatusCode KinSelectTool ::
  useConfiguration (const InternalConfiguration& configuration)
  {
    ATH_CHECK (AnaTool::useConfiguration (configuration));
    ATH_CHECK (m_selectionData.initialize (configuration, ""));
    return StatusCode::SUCCESS;
  }



  AnalysisStep KinSelectTool ::
  step () const
  {
    return STEP_KINSELECT;
  }



  unsigned KinSelectTool ::
  inputTypes () const
  {
    return 1 << m_objectType;
  }



  unsigned KinSelectTool ::
  outputTypes () const
  {
    return 1 << m_objectType;
  }



  StatusCode KinSelectTool ::
  execute (IEventObjects& objects)
  {
    xAOD::IParticleContainer *container = objects.getParticles (m_objectType);
    if (container == 0)
    {
      ATH_MSG_ERROR ("object type not defined in configuration");
      return StatusCode::FAILURE;
    }

    for (auto object : *container)
    {
      ATH_CHECK (m_selectionData.prepare (object));
      if (m_selectionData.needSelectionTool())
	cut_kin_select.setPassedIf (m_selectObject->select (*object));
      m_selectionData.apply ();
    }
    return StatusCode::SUCCESS;
  }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.

//
// includes
//

#include <QuickAna/EventSelectHasVertex.h>

#include <QuickAna/IEventObjects.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/MessageCheck.h>

//
// method implementations
//

namespace ana
{
  EventSelectHasVertex ::
  EventSelectHasVertex (const std::string& name)
    : AsgTool (name), AnaTool (name)
  {
  }



  StatusCode EventSelectHasVertex ::
  initialize()
  {
    m_selection.registerCut (SelectionStep::MET, "vertex", cut_vertex);
    return StatusCode::SUCCESS;
  }



  StatusCode EventSelectHasVertex ::
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



  StatusCode EventSelectHasVertex ::
  useConfiguration (const InternalConfiguration& configuration)
  {
    ATH_CHECK (AnaTool::useConfiguration (configuration));
    ATH_CHECK (m_selection.initialize (configuration, ""));
    return StatusCode::SUCCESS;
  }



  AnalysisStep EventSelectHasVertex ::
  step () const
  {
    return STEP_RETRIEVE;
  }



  unsigned EventSelectHasVertex ::
  inputTypes () const
  {
    return 1 << OBJECT_EVENT_SELECT;
  }



  unsigned EventSelectHasVertex ::
  outputTypes () const
  {
    return 1 << OBJECT_EVENT_SELECT;
  }



  StatusCode EventSelectHasVertex ::
  execute (IEventObjects& objects)
  {
    EventSelectType *eventSelect = objects.eventSelect();

    ATH_CHECK (m_selection.prepare (eventSelect));
    if (m_selection.needSelectionTool())
    {
      const char *contName = "PrimaryVertices";
      const xAOD::VertexContainer *vertices = nullptr;
      ANA_CHECK (evtStore()->retrieve(vertices, contName));
      bool hasVertex = false;
      for (auto vtx : *vertices)
      {
	if (vtx->vertexType() == xAOD::VxType::PriVtx)
	  hasVertex = true;
      }
      cut_vertex.setPassedIf (hasVertex);
    }
    m_selection.apply ();
    return StatusCode::SUCCESS;
  }



  namespace
  {
    StatusCode makeVertexTool (DefinitionArgs& args)
    {
      using namespace msgObjectDefinition;

      std::unique_ptr<EventSelectHasVertex> vertexTool
	(new EventSelectHasVertex (args.prefix()));
      args.add ( std::move(vertexTool) );

      return StatusCode::SUCCESS;
    }
  }

  QUICK_ANA_EVENT_SELECT_DEFINITION_MAKER( "vertex",   makeVertexTool(args) )
}

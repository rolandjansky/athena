/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/PileupTool.h>

#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>

#include "PileupReweighting/PileupReweightingTool.h"

//
// method implementations
//

namespace ana
{
  PileupTool ::
  PileupTool (const std::string& name)
    : AsgTool (name), AnaTool (name)
    , m_tool ("prw", this)
  {
  }



  StatusCode PileupTool ::
  setObjectType (ObjectType /*type*/, const std::string& workingPoint)
  {
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point for pileup tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  AnalysisStep PileupTool ::
  step () const
  {
    return STEP_PILEUP_REWEIGHT;
  }



  unsigned PileupTool ::
  inputTypes () const
  {
    return 1 << OBJECT_EVENTINFO;
  }



  unsigned PileupTool ::
  outputTypes () const
  {
    return 1 << OBJECT_EVENTINFO;
  }



  StatusCode PileupTool ::
  initialize()
  {
    // Configuration recommendations taken from
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting

    //ATH_CHECK (m_tool.make ("CP::PileupReweightingTool"));
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_tool, CP::PileupReweightingTool) );

    ATH_CHECK( m_tool.setProperty("ConfigFiles", muMcFiles) );
    ATH_CHECK( m_tool.setProperty("LumiCalcFiles", muDataFiles) );

    /// To use the default SF for rel21; still keep placeholder
    //ATH_CHECK( m_tool.setProperty("DataScaleFactor", 1./1.09) );
    //ATH_CHECK( m_tool.setProperty("DataScaleFactorUP", 1.) );
    //ATH_CHECK( m_tool.setProperty("DataScaleFactorDOWN", 1./1.18) );

    ATH_CHECK( m_tool.initialize() );

    registerTool(&*m_tool);

    return StatusCode::SUCCESS;
  }



  StatusCode PileupTool ::
  execute (IEventObjects& objects)
  {
    // First do the pileup reweighting that we need to do
    ATH_CHECK( m_tool->apply( *objects.eventinfo() ) );

    //--- PRWHash to recalculate PU weights using analysis ntuples
    //--- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting#Using_PRWHash_to_change_pileup_w

    if((*objects.eventinfo()).eventType(xAOD::EventInfo::IS_SIMULATION)) {
      ULong64_t prwhash = m_tool->getPRWHash( *objects.eventinfo() );
      objects.eventinfo()->auxdata<ULong64_t>("PRWHash") = prwhash;
    }

    // Must decorate the actual instance in the event store for the electron tool to work
    auto eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
    eventInfo->auxdecor<unsigned int>("RandomRunNumber") =
      objects.eventinfo()->auxdecor<unsigned int>( "RandomRunNumber" );

    return StatusCode::SUCCESS;
  }

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// My header first
#include <QuickAna/CleanTool.h>

// Toolkit infrastructure includes
#include <CxxUtils/make_unique.h>

// Local includes
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>
#include <JetSelectorTools/JetCleaningTool.h>
#include <JetSelectorTools/EventCleaningTool.h>


namespace ana
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CleanTool ::
  CleanTool (const std::string& name)
    : AsgTool(name), AnaTool(name),
      m_jetCleaning_tool ("jetCleaning", this),
      m_evtCleaning_tool ("evtCleaning", this)
  {
      declareProperty("WorkingPoint", m_workingPoint = "LooseBad");
      declareProperty("JvtDecorator", m_jvtDec = "Jvt_pass");
      declareProperty("OrDecorator",  m_orDec  = "OR_pass");
      declareProperty("PtCut",  m_ptCut  = 20.e3);
      declareProperty("EtaCut", m_etaCut = 4.5);
  }

  //---------------------------------------------------------------------------
  // Set object type
  //---------------------------------------------------------------------------
  StatusCode CleanTool ::
  setObjectType(ObjectType type, const std::string& workingPoint)
  {
    if(type != OBJECT_CLEANING) {
      ATH_MSG_ERROR("unsupported object type");
      return StatusCode::FAILURE;
    }
    if(!workingPoint.empty()) {
      ATH_MSG_ERROR("can't have working point for overlap removal tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode CleanTool ::
  initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() );

    using namespace ana::msgObjectDefinition;

    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_evtCleaning_tool, ECUtils::EventCleaningTool));
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_jetCleaning_tool, JetCleaningTool));
    ATH_CHECK (m_evtCleaning_tool.setProperty("JetCleaningTool", m_jetCleaning_tool));
    ATH_CHECK (m_evtCleaning_tool.setProperty("PtCut" ,  m_ptCut));
    ATH_CHECK (m_evtCleaning_tool.setProperty("EtaCut" , m_etaCut));
    ATH_CHECK (m_evtCleaning_tool.setProperty("JvtDecorator" , m_jvtDec));
    ATH_CHECK (m_evtCleaning_tool.setProperty("OrDecorator" ,  m_orDec));
    ATH_CHECK (m_evtCleaning_tool.setProperty("CleaningLevel" , m_workingPoint));
    ATH_CHECK (m_evtCleaning_tool.initialize());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Scheduling boiler plate methods
  //---------------------------------------------------------------------------
  AnalysisStep CleanTool ::
  step() const { return STEP_CLEANING; }
  //---------------------------------------------------------------------------
  unsigned CleanTool ::
  inputTypes() const
  {
    return (1 << OBJECT_EVENTINFO) | (1 << OBJECT_JET);
  }
  //---------------------------------------------------------------------------
  unsigned CleanTool ::
  outputTypes() const { return inputTypes(); } // Same as input types

  //---------------------------------------------------------------------------
  // Execute the Event Cleaning
  //---------------------------------------------------------------------------
  StatusCode CleanTool ::
  execute(IEventObjects& objects)
  {
    // apply event cleaning
    bool pass_clean = m_evtCleaning_tool->acceptEvent(objects.jets());
    objects.eventinfo()->auxdata<char>("Clean_pass") = pass_clean; 

    return StatusCode::SUCCESS;
  }

} // namespace ana


// Anonymous namespace to hold the tool factory function
namespace
{

  //---------------------------------------------------------------------------
  // Tool maker function.
  //---------------------------------------------------------------------------
  StatusCode makeCleanTool(ana::DefinitionArgs& args,
                        const std::string& WP = "LooseBad",
                        const std::string& JvtDec = "Jvt_pass",
                        const std::string& OrDec  = "OR_pass",
                        const float PtCut  = 20.e3,
                        const float EtaCut = 4.5
                       ) 
  {
    using namespace ana::msgObjectDefinition;

    auto cleanTool = CxxUtils::make_unique<ana::CleanTool>( args.prefix() );
    ANA_CHECK( cleanTool->setProperty("WorkingPoint", WP) );
    ANA_CHECK( cleanTool->setProperty("JvtDecorator", JvtDec) );
    ANA_CHECK( cleanTool->setProperty("OrDecorator", OrDec) );
    ANA_CHECK( cleanTool->setProperty("PtCut",  PtCut) );
    ANA_CHECK( cleanTool->setProperty("EtaCut", EtaCut) );
    args.add( std::move(cleanTool) );

    return StatusCode::SUCCESS;
  }

  // Register the object definitions
  QUICK_ANA_CLEAN_DEFINITION_MAKER( "default", makeCleanTool(args) )

} // anonymous namespace

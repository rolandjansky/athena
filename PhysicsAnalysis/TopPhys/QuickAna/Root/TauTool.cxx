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

#include <QuickAna/TauTool.h>

#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>
#include <TauAnalysisTools/TauEfficiencyCorrectionsTool.h>
#include <TauAnalysisTools/TauSelectionTool.h>
#include <TauAnalysisTools/TauSmearingTool.h>
#include <TauAnalysisTools/TauTruthMatchingTool.h>
#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>

#include <CxxUtils/make_unique.h>

namespace
{
  /// Defining hardcoded name for the TauSelectionTool in one place
  const std::string tauSelectionToolName = "QA_TauSelectionTool";
}

//
// method implementations
//

namespace ana
{

  //---------------------------------------------------------------------------
  // TauToolCorrect
  //---------------------------------------------------------------------------

  // Constructor
  TauToolCorrect ::
  TauToolCorrect (const std::string& name)
    : AsgTool(name), AnaToolCorrect<xAOD::TauJetContainer>(name),
      m_smearTool ("TauSmearingTool", this)
  {
  }


  // Initialization
  StatusCode TauToolCorrect ::
  initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );

    // Initialize the CP smearing tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_smearTool, TauAnalysisTools::TauSmearingTool));
    // The twiki is unclear about whether we should be doing this now
    //ATH_CHECK( m_smearTool.setProperty("ApplyMVATES", true) );
    ATH_CHECK( m_smearTool.initialize() );
    registerTool(&*m_smearTool);

    registerCut(SelectionStep::MET, "smear_tool", cut_smear_tool);

    return StatusCode::SUCCESS;
  }


  // Object correction
  StatusCode TauToolCorrect ::
  correctObject (xAOD::TauJet& tau)
  {
    ATH_MSG_DEBUG("correctObject");

    // TODO: evaluate if this is still necessary.
    // (Maybe temporary) workaround to suppress smearing tool warnings:
    // consider only taus with nTracks > 0. I count this in the smear_tool cut
    // because in principle it should be handled as an OutOfValidityRange code.
    if(tau.nTracks() > 0) {
      QA_CHECK_CUT( cut_smear_tool, m_smearTool->applyCorrection(tau) );
    }
    else cut_smear_tool.setFailedIf(true);

    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // TauToolSelect
  //---------------------------------------------------------------------------

  // Constructor
  TauToolSelect ::
  TauToolSelect (const std::string& name)
    : AsgTool(name), AnaToolSelect<xAOD::TauJetContainer>(name),
      m_configFile("QuickAna/tau_selection_medium.conf"),
      // Using public tool here so I can configure the tau SF tool :(
      m_selectionTool(tauSelectionToolName)
      //m_selectionTool("TauSelectionTool", this)
  {
    declareProperty("ConfigFile", m_configFile);
  }


  // Initialization
  StatusCode TauToolSelect ::
  initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );

    // Tau selection recommendations from:
    // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauRecommendationsSummer2016#Tau_Selection_unchanged_since_20
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selectionTool, TauAnalysisTools::TauSelectionTool));
    ATH_CHECK( m_selectionTool.setProperty("ConfigPath", m_configFile) );
    ATH_CHECK( m_selectionTool.initialize() );

    registerCut(SelectionStep::MET, "selection_step", m_selectionToolCut);

    return StatusCode::SUCCESS;
  }


  // Object selection
  StatusCode TauToolSelect ::
  selectObject (xAOD::TauJet& tau)
  {
    ATH_MSG_DEBUG("selectObject");
    m_selectionToolCut.setPassedIf( m_selectionTool->accept(tau) );
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // TauToolWeight
  //---------------------------------------------------------------------------

  // Constructor
  TauToolWeight ::
  TauToolWeight (const std::string& name)
    : AsgTool(name), AnaToolWeight<xAOD::TauJetContainer>(name),
      //m_configFile("QuickAna/tau_selection_medium.conf"),
      m_selectionTool(tauSelectionToolName),
      m_effTool("TauEfficiencyCorrectionsTool", this)
  {
    //declareProperty("ConfigFile", m_configFile);
  }


  // Initialization
  StatusCode TauToolWeight ::
  initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );

    // Need to retrieve the (hopefully) already created TauSelectionTool.
    // There's probably some way to check that I'm retrieving the proper one
    // already initialized, but I'm not sure what's the best approach.
    ATH_CHECK( m_selectionTool.retrieve() );

    // The TauEfficiencyCorrectionsTool doesn't make this easy without using
    // a public TauSelectionTool, so I'm just going to parse what I need
    // from the tau ID config file.
    //TEnv confEnv;
    //auto confPath = PathResolverFindCalibFile(m_configFile);
    //confEnv.ReadFile(confPath.c_str(), kEnvAll);

    // Initialize the CP efficiency tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_effTool, TauAnalysisTools::TauEfficiencyCorrectionsTool));
    ATH_CHECK( m_effTool.setProperty("TauSelectionTool", m_selectionTool) );
    ATH_CHECK( m_effTool.initialize() );
    registerTool(&*m_effTool);

    // Temporary bugfix for NULL pointer
    //ANA_CHECK( m_effTool->applySystematicVariation(CP::SystematicSet()) );

    return StatusCode::SUCCESS;
  }


  // Object weighting
  StatusCode TauToolWeight ::
  objectWeight (const xAOD::TauJet& tau, float& weight)
  {
    ATH_MSG_DEBUG("objectWeight");
    weight = 1;
    QA_CHECK_WEIGHT
      ( double, weight, m_effTool->getEfficiencyScaleFactor(tau, weight) );
    return StatusCode::SUCCESS;
  }



  //---------------------------------------------------------------------------
  // TauToolTruth
  //---------------------------------------------------------------------------

  // Constructor
  TauToolTruth ::
  TauToolTruth(const std::string& name)
    : AsgTool(name), AnaTool(name),
      m_lastEventNumber(0),
      m_truthTool("TauTruthMatchingTool", this)
  {
    declareProperty("WriteTruthTaus", m_writeTruthTaus=true);
  }

  // Set object type
  StatusCode TauToolTruth ::
  setObjectType(ObjectType type, const std::string& workingPoint)
  {
    if(type != OBJECT_TAU) {
      ATH_MSG_ERROR("unsupported object type");
      return StatusCode::FAILURE;
    }
    if(!workingPoint.empty()) {
      ATH_MSG_ERROR("working points not supported by TauToolTruth");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  // Analysis step
  AnalysisStep TauToolTruth::step() const
  { return STEP_PRECORRECT; }

  // Input and output types. EventInfo just used to get event number.
  unsigned TauToolTruth::inputTypes() const
  { return (1 << OBJECT_TAU) | (1 << OBJECT_EVENTINFO); }
  unsigned TauToolTruth::outputTypes() const
  { return 1 << OBJECT_TAU; }


  // Initialization
  StatusCode TauToolTruth ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_truthTool, TauAnalysisTools::TauTruthMatchingTool) );
    ATH_CHECK( m_truthTool.setProperty("WriteTruthTaus", m_writeTruthTaus) );
    ATH_CHECK( m_truthTool.initialize() );
    return StatusCode::SUCCESS;
  }

  // Execute
  StatusCode TauToolTruth::execute(IEventObjects& objects)
  {
    ATH_MSG_DEBUG("execute");

    // Prepare the truth tool once for each event.
    // To ensure this is only called once per event (default scheduler),
    // we look for a change in the event number. If EventInfo definition is
    // "none", though, the pointer will be null, in which case we just retrieve
    // it directly from the event store.
    //const xAOD::EventInfo* evtInfo = objects.eventinfo();
    //if(!evtInfo) {
    //  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );
    //}
    //auto currentEventNumber = evtInfo->eventNumber();
    //if(m_lastEventNumber != currentEventNumber) {
    //  ATH_CHECK( m_truthTool->initializeEvent() );
    //  m_lastEventNumber = currentEventNumber;
    //}

    // Perform truth matching for every tau. Probably overkill.
    for(auto tau : *objects.taus()) {
      m_truthTool->applyTruthMatch(*tau);
    }
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // Maker function for tau tools.
  //---------------------------------------------------------------------------
  StatusCode makeTauTool(DefinitionArgs& args, const std::string& configFile)
  {
    using namespace msgObjectDefinition;
    using CxxUtils::make_unique;

    // Input tau container name
    const std::string inputContainer = "TauJets";
    bool isData = args.configuration()->isData();

    // Retrieval tool
    auto retrieveTool =
      make_unique<AnaToolRetrieve>(args.prefix() + "_retrieve", inputContainer);
    args.add( std::move(retrieveTool) );

    // Truth-matching tool - only for MC
    if(!isData) {
      auto truthMatchTool =
        make_unique<TauToolTruth>(args.prefix() + "_truthMatch");
      args.add( std::move(truthMatchTool) );
    }

    // Correction tool
    auto correctTool = make_unique<TauToolCorrect>(args.prefix() + "_correct");
    args.add( std::move(correctTool) );

    // Selection tool
    auto selectTool = make_unique<TauToolSelect>(args.prefix() + "_select");
    ANA_CHECK( selectTool->setProperty("ConfigFile", configFile) );
    args.add( std::move(selectTool) );

    // Weight tool - only for MC
    if(!isData) {
      auto weightTool = make_unique<TauToolWeight>(args.prefix() + "_weight");
      //ANA_CHECK( weightTool->setProperty("ConfigFile", configFile) );
      args.add( std::move(weightTool) );
    }

    return StatusCode::SUCCESS;
  }

  // Define the maker wrapper functions with these macros
  QUICK_ANA_TAU_DEFINITION_MAKER
    ( "default", makeTauTool(args, "QuickAna/tau_selection_medium.conf") )
  QUICK_ANA_TAU_DEFINITION_MAKER
    ( "loose", makeTauTool(args, "QuickAna/tau_selection_loose.conf") )
  QUICK_ANA_TAU_DEFINITION_MAKER
    ( "tight", makeTauTool(args, "QuickAna/tau_selection_tight.conf") )

} // namespace ana

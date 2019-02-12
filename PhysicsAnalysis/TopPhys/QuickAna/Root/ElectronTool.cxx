/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Includes
//

#include <QuickAna/ElectronTool.h>

#include <CxxUtils/make_unique.h>

#include <ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h>
#include <ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h>
#include <ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h>
#include <ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>

// For recommended d0 and z0 cuts
#include <xAODTracking/TrackParticle.h>

#include <IsolationSelection/IsolationSelectionTool.h>
#include <IsolationCorrections/IsolationCorrectionTool.h>
#include <xAODTracking/TrackParticlexAODHelpers.h>

//
// Method implementations
//

namespace ana
{
  const float GeV = 1000.;

  //--------------------------------------------------------------------------
  // ElectronToolPreCorrect
  //--------------------------------------------------------------------------

  ElectronToolPreCorrect ::
  ElectronToolPreCorrect (const std::string& name)
    : AsgTool(name), AnaToolCorrect<xAOD::ElectronContainer>(name),
      m_likelihoodTool_loose ("AsgElectronLikelihoodTool_loose", this),
      m_likelihoodTool_medium ("AsgElectronLikelihoodTool_medium", this),
      m_likelihoodTool_tight ("AsgElectronLikelihoodTool_tight", this)
  {
  }


  AnalysisStep ElectronToolPreCorrect ::
  step () const
  {
    return STEP_PRECORRECT;
  }


  StatusCode ElectronToolPreCorrect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // Likelihood tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_likelihoodTool_loose, AsgElectronLikelihoodTool));
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_likelihoodTool_medium, AsgElectronLikelihoodTool));
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_likelihoodTool_tight, AsgElectronLikelihoodTool));

    ATH_CHECK (m_likelihoodTool_loose.setProperty("primaryVertexContainer", "PrimaryVertices"));
    ATH_CHECK (m_likelihoodTool_medium.setProperty("primaryVertexContainer", "PrimaryVertices"));
    ATH_CHECK (m_likelihoodTool_tight.setProperty("primaryVertexContainer", "PrimaryVertices"));

    ATH_CHECK (m_likelihoodTool_loose.setProperty("WorkingPoint", "LooseLHElectron"));
    ATH_CHECK (m_likelihoodTool_medium.setProperty("WorkingPoint", "MediumLHElectron"));
    ATH_CHECK (m_likelihoodTool_tight.setProperty("WorkingPoint", "TightLHElectron"));

    ATH_CHECK( m_likelihoodTool_loose.initialize() );
    ATH_CHECK( m_likelihoodTool_medium.initialize() );
    ATH_CHECK( m_likelihoodTool_tight.initialize() );

    /* Why are these commented out??
    registerCut (SelectionStep::MET, "likelihoodTool_loose", cut_likelihoodTool_loose);
    registerCut (SelectionStep::MET, "likelihoodTool_medium", cut_likelihoodTool_medium);
    registerCut (SelectionStep::MET, "likelihoodTool_tight", cut_likelihoodTool_tight);
    registerCut (SelectionStep::MET, "isEMTool_loose", cut_isEMTool_loose);
    registerCut (SelectionStep::MET, "isEMTool_medium", cut_isEMTool_medium);
    registerCut (SelectionStep::MET, "isEMTool_tight", cut_isEMTool_tight);*/

    return StatusCode::SUCCESS;
  }


  StatusCode ElectronToolPreCorrect ::
  correctObject (xAOD::Electron& electron)
  {
    ATH_MSG_DEBUG("correctObject");
    const Root::TAccept& passLHLoose = m_likelihoodTool_loose->accept( &electron );
    const Root::TAccept& passLHMedium = m_likelihoodTool_medium->accept( &electron );
    const Root::TAccept& passLHTight = m_likelihoodTool_tight->accept( &electron );

    electron.auxdecor<SelectType>("QuickAna_LooseAndBLayerLLH")  = static_cast<bool>(passLHLoose);
    electron.auxdecor<SelectType>("QuickAna_MediumLLH") = static_cast<bool>(passLHMedium);
    electron.auxdecor<SelectType>("QuickAna_TightLLH")  = static_cast<bool>(passLHTight);

    /*// The isEM bits -- maybe useful but comment out for now
    electron.auxdecor<unsigned int>("QuickAna_isEMLHLoose")  = passLHLoose.getCutResultInverted();
    electron.auxdecor<unsigned int>("QuickAna_isEMLHMedium") = passLHMedium.getCutResultInverted();
    electron.auxdecor<unsigned int>("QuickAna_isEMLHTight")  = passLHTight.getCutResultInverted();*/

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // ElectronToolCorrect
  //--------------------------------------------------------------------------

  // Constructor
  ElectronToolCorrect ::
  ElectronToolCorrect (const std::string& name)
    : AsgTool(name), AnaToolCorrect<xAOD::ElectronContainer>(name),
      m_calibration ("EgammaCalibrationAndSmearingTool", this),
      m_isoCorrTool ("IsolationCorrectionTool", this)
  {
    declareProperty("IsAF2", m_isAF2=false);
    declareProperty("IsData", m_isData=false);
  }


  // Initialization
  StatusCode ElectronToolCorrect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // MC15 recommendations from:
    //  https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronPhotonFourMomentumCorrection

    // Initialize the calibration tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_calibration, CP::EgammaCalibrationAndSmearingTool));
    ATH_CHECK (m_calibration.setProperty ("ESModel", "es2017_R21_v1"));
    ATH_CHECK (m_calibration.setProperty ("decorrelationModel", "1NP_v1"));
    ATH_CHECK (m_calibration.setProperty ("useAFII", m_isAF2?1:0));
    ATH_CHECK( m_calibration.initialize() );
    registerTool(&*m_calibration);

    // Initialize isolation correction tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isoCorrTool, CP::IsolationCorrectionTool));
    ATH_CHECK( m_isoCorrTool.setProperty( "IsMC", !m_isData) );
    ATH_CHECK( m_isoCorrTool.setProperty( "AFII_corr", m_isAF2) );
    ATH_CHECK( m_isoCorrTool.initialize() );

    registerCut (SelectionStep::MET, "correction", cut_correction);
    registerCut (SelectionStep::MET, "correctionIso", cut_isoCorr_tool);
    return StatusCode::SUCCESS;
  }


  // Object correction
  StatusCode ElectronToolCorrect ::
  correctObject (xAOD::Electron& electron)
  {
    ATH_MSG_DEBUG("correctObject");
    // Optimization of low PT electrons. Not sure if this makes a difference.
    if (electron.pt() < 4.*GeV) return StatusCode::SUCCESS;
    QA_CHECK_CUT (cut_correction, m_calibration->applyCorrection(electron));

    // Correct the isolation
    QA_CHECK_CUT (cut_isoCorr_tool, m_isoCorrTool->applyCorrection(electron));

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // ElectronToolSelect
  //--------------------------------------------------------------------------

  // Constructor
  ElectronToolSelect ::
  ElectronToolSelect (const std::string& name)
    : AsgTool(name), AnaToolSelect<xAOD::ElectronContainer>(name),
      m_isolationTool ("IsolationSelectionTool", this)
  {
    declareProperty("IDString", m_idStr);
    declareProperty("PtMin", m_ptMin = 4.5*GeV);
    declareProperty("EtaMax", m_etaMax = 2.47);

    declareProperty( "IsolationOn", m_isolationOn = true );
    declareProperty( "IsolationWP", m_isolationWP = "Undefined", "Working point to use" );
    // The interface for adding "UserDefined" working points has changed
    // -- for now supporting single standard working points only. will add other options when needed

    declareProperty( "IDAna", m_idAna = "" );
    // For applying tighter electron ID at analysis level - SUSY group recommendation
    declareProperty( "LooseImpactCut", m_looseImpactCut = false );
  }


  // Initialization
  StatusCode ElectronToolSelect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // Initialize the isolation selection tool
    if (m_isolationOn)
    {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isolationTool, CP::IsolationSelectionTool));
      ATH_CHECK (m_isolationTool.setProperty("ElectronWP", m_isolationWP));
      ATH_CHECK (m_isolationTool.initialize());
      registerTool(&*m_isolationTool);
    }

    // Note: for now, use isolated objects (if isolation is applied) to build MET

    registerCut (SelectionStep::MET, "id", cut_id);
    registerCut (SelectionStep::MET, "OQ", cut_OQ);
    registerCut (SelectionStep::MET, "ptMin", cut_ptMin);
    registerCut (SelectionStep::MET, "clusEta", cut_clusEta);

    if (m_idAna=="")
    { // This was a standard setup...
      registerCut (SelectionStep::MET, "d0", cut_d0);
      registerCut (SelectionStep::MET, "z0", cut_z0);
      registerCut (SelectionStep::MET, "isolationTool", cut_isolationTool);
    }
    else
    { /// SUSY group recommends *not* including d0/z0/isolation in MET cuts
      registerCut (SelectionStep::ANALYSIS, "d0", cut_d0);
      registerCut (SelectionStep::ANALYSIS, "z0", cut_z0);
      registerCut (SelectionStep::ANALYSIS, "isolationTool", cut_isolationTool);
      registerCut (SelectionStep::ANALYSIS, "finalID", cut_idAna);
    }

    return StatusCode::SUCCESS;
  }


  // Object selection
  StatusCode ElectronToolSelect ::
  selectObject (xAOD::Electron& electron)
  {
    ATH_MSG_DEBUG("selectObject");
    // Apply selection tool. See comments in ElectronToolPreCorrect.

    // The naming convention in derivation framework is different
    std::string idStr_DFCommon = m_idStr.find("LH") != std::string::npos ?
                                 "DFCommonElectrons" + m_idStr :
                                 "DFCommonElectronsIsEM" + m_idStr;

    if (electron.isAvailable<SelectType>("QuickAna_" + m_idStr))
    {
      cut_id.setPassedIf (electron.auxdataConst<SelectType>("QuickAna_" + m_idStr));
    }
    else if (electron.isAvailable<int>(idStr_DFCommon))
    {
      cut_id.setPassedIf (electron.auxdataConst<int>(idStr_DFCommon));
    }
    else
    {
      cut_id.setPassedIf (electron.passSelection(m_idStr));
    }

    // Object quality
    cut_OQ.setPassedIf (electron.isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON));

    // Electron pt
    cut_ptMin.setPassedIf (electron.pt() >= m_ptMin);

    // Electron eta
    double clusEta = electron.caloCluster()->etaBE(2);
    cut_clusEta.setPassedIf (std::fabs(clusEta) < m_etaMax);

    // d0/z0 based on the track particle
    const xAOD::TrackParticle* tp = electron.trackParticle();

    // Grab the primary vertex out of the container
    // TODO: would be better to do this once per event.
    // TODO: we should at least move this into a helper code.
    const xAOD::VertexContainer* vertexContainer = nullptr;
    ATH_CHECK( evtStore()->retrieve(vertexContainer, "PrimaryVertices" ) );
    const xAOD::Vertex* pv = nullptr;
    for (const auto* const vtx : *vertexContainer) {
      if (vtx->vertexType() == xAOD::VxType::VertexType::PriVtx) {
        pv = vtx;
        break;
      }
    }

    // Grab the event info to get the beam spot setup.
    // TODO: would be better to do this once per event.
    const xAOD::EventInfo* evt = nullptr;
    ATH_CHECK( evtStore()->retrieve( evt, "EventInfo" ) );

    // Calculate d0sig and z0*sin(theta)
    double el_d0Sig = -999;
    double el_z0SinTheta = -999;
    if(tp && evt) {
      // Calculate d0 significance using the tracking CP group's tool.
      el_d0Sig = xAOD::TrackingHelpers::d0significance
        ( tp, evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY() );
      // Calculate z0*sinTheta manually. TODO: ask for similar helper function.
      el_z0SinTheta = (tp->z0() + tp->vz() - (pv?pv->z():0.)) * sin(tp->theta());
    }
    // Decorate the electron
    electron.auxdata<double>("d0Sig") = el_d0Sig;
    electron.auxdata<double>("z0sintheta") = el_z0SinTheta;

    // Tracking group's recommendation.  E/gamma group will recalculate
    //  scale factors after pre-recommendations including these cuts in
    //  their selection.  Believed that current uncertainties "probably"
    //  cover these cuts.
    if(m_looseImpactCut) {
      cut_d0.setPassedIf(true);
      cut_z0.setPassedIf(true);
    } else {
      cut_d0.setPassedIf( tp && fabs(el_d0Sig)<5 );
      cut_z0.setPassedIf( tp && pv && fabs(el_z0SinTheta)<0.5 );
    }

    // Apply isolation tool
    if (m_isolationOn) {
      cut_isolationTool.setPassedIf (m_isolationTool->accept(electron));
    }

    if (m_idAna != "")
    {
      // The naming convention in derivation framework is different
      std::string idAna_DFCommon = m_idAna.find("LH") != std::string::npos ?
                                   "DFCommonElectrons" + m_idAna :
                                   "DFCommonElectronsIsEM" + m_idAna;

      // Try our custom ID decoration first.
      if (electron.isAvailable<SelectType>("QuickAna_" + m_idAna)) {
        cut_idAna.setPassedIf (electron.auxdataConst<SelectType>("QuickAna_" + m_idAna));
      }
      // Next, try the derivation framework ID result.
      else if (electron.isAvailable<int>(idAna_DFCommon)) {
        cut_idAna.setPassedIf (electron.auxdataConst<int>(idAna_DFCommon));
      }
      // Last resort: use the xAOD value. Should we allow this at all?
      else {
        cut_idAna.setPassedIf (electron.passSelection(m_idAna));
      }
    }

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // ElectronToolWeight
  //--------------------------------------------------------------------------

  // Constructor
  ElectronToolWeight ::
  ElectronToolWeight (const std::string& name)
    : AsgTool(name), AnaToolWeight<xAOD::ElectronContainer>(name),
      m_efficiencyTool_reco ("AsgElectronEfficiencyCorrectionTool_reco", this),
      m_efficiencyTool_id   ("AsgElectronEfficiencyCorrectionTool_id", this),
      m_efficiencyTool_iso  ("AsgElectronEfficiencyCorrectionTool_iso", this)
  {
    declareProperty("IDString", m_idStr);
    declareProperty("IsoString", m_isoStr);

    // For applying tight electron ID at analysis level - SUSY group recommendation
    declareProperty("IDAna", m_idAna = "");

    declareProperty("IsAF2", m_isAF2=false);
  }


  // Initialization
  StatusCode ElectronToolWeight ::
  initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );

    // ICHEP 2016 pre-recommendations come from:
    //   https://twiki.cern.ch/twiki/bin/view/AtlasProtected/LatestRecommendationsElectronIDRun2#ICHEP_2016_pre_recommendations

    // The efficiency tools use a magic integer flag for the data type
    const int effDataType = m_isAF2? 3 : 1;

    // Calib file directory
    //const std::string egMapFile = "ElectronEfficiencyCorrection/2015_2017/rel21.2/Moriond_February2018_v2/map5.txt";
    const std::string egMapFile = "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt";

    // Initialize the AsgElectronEfficiencyCorrectionTool for reco
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_efficiencyTool_reco, AsgElectronEfficiencyCorrectionTool) );
    // Reco scale factors - currently need to set this file by hand
    ATH_CHECK( m_efficiencyTool_reco.setProperty ("MapFilePath", egMapFile) );
    ATH_CHECK( m_efficiencyTool_reco.setProperty ("RecoKey", "Reconstruction") );
    ATH_CHECK( m_efficiencyTool_reco.setProperty ("CorrelationModel", "TOTAL") );
    ATH_CHECK( m_efficiencyTool_reco.setProperty ("ForceDataType", effDataType) );
    ATH_CHECK( m_efficiencyTool_reco.initialize() );
    registerTool(&*m_efficiencyTool_reco);

    // Well, this is irritating
    std::string id_ana = TString( (m_idAna!="") ? m_idAna : m_idStr ).ReplaceAll("AndBLayer", "BLayer").ReplaceAll("LLH", "").Data();

    // Initialize the AsgElectronEfficiencyCorrectionTool.
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_efficiencyTool_id, AsgElectronEfficiencyCorrectionTool) );
    // ID scale factors - currently need to set this file by hand.
    ATH_CHECK( m_efficiencyTool_id.setProperty ("MapFilePath", egMapFile) );
    ATH_CHECK( m_efficiencyTool_id.setProperty ("IdKey", id_ana) );
    ATH_CHECK( m_efficiencyTool_id.setProperty ("CorrelationModel", "TOTAL") );
    ATH_CHECK( m_efficiencyTool_id.setProperty ("ForceDataType", effDataType) );
    ATH_CHECK( m_efficiencyTool_id.initialize() );
    registerTool(&*m_efficiencyTool_id);

    // Initialize the AsgElectronEfficiencyCorrectionTool.
    if (m_isoStr != "")
    {
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_efficiencyTool_iso, AsgElectronEfficiencyCorrectionTool) );
      // Iso scale factor file. As with ID file above
      ATH_CHECK( m_efficiencyTool_iso.setProperty ("MapFilePath", egMapFile) );
      ATH_CHECK( m_efficiencyTool_iso.setProperty ("IdKey", id_ana) );
      ATH_CHECK( m_efficiencyTool_iso.setProperty ("IsoKey", m_isoStr) );
      ATH_CHECK( m_efficiencyTool_iso.setProperty ("CorrelationModel", "TOTAL") );
      ATH_CHECK( m_efficiencyTool_iso.setProperty("ForceDataType", effDataType) );
      ATH_CHECK( m_efficiencyTool_iso.initialize() );
      registerTool(&*m_efficiencyTool_iso);
    }
    return StatusCode::SUCCESS;
  }


  // Object weighting
  StatusCode ElectronToolWeight ::
  objectWeight (const xAOD::Electron& electron, float& weight)
  {
    ATH_MSG_DEBUG("objectWeight");
    // Electron efficiency SF
    weight = 1.;
    // Apply individual scale factors, and only if applicable apply them to the electron
    double reco_weight = 1. , id_weight = 1. , iso_weight = 1.;
    if (m_efficiencyTool_reco->getEfficiencyScaleFactor(electron, reco_weight)
        == CP::CorrectionCode::Ok) weight *= reco_weight;
    if (m_efficiencyTool_id->getEfficiencyScaleFactor(electron, id_weight)
        == CP::CorrectionCode::Ok) weight *=   id_weight;
    if (m_efficiencyTool_iso.isInitialized() &&
        m_efficiencyTool_iso ->getEfficiencyScaleFactor(electron,  iso_weight)
        == CP::CorrectionCode::Ok) weight *=  iso_weight;
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // Maker function for electron tools.
  //--------------------------------------------------------------------------
  StatusCode makeElectronTool(DefinitionArgs& args,
                              const std::string& id,
                              const bool isolationOn,
                              const std::string& isolationWP,
                              const std::string& idAna,
                              const bool looseImpactCut)
  {
    using namespace msgObjectDefinition;
    using CxxUtils::make_unique;

    auto config = args.configuration();

    if (args.firstWP()){
      // Set up the appropriate global config option.
      // Have to swap around the LH string.
      //   What is going on with this logic??
      std::string my_idStr = "LooseAndBLayerLLH";
      if ( (""!=idAna && idAna=="MediumLLH") ||
           (""==idAna && id=="MediumLLH") ) my_idStr = "MediumLLH";
      if ( (""!=idAna && idAna=="TightLLH") ||
           (""==idAna && id=="TightLLH") ) my_idStr = "TightLLH";
      config->setElectronWP(my_idStr);
    }

    // Do the same for the electron isolation
    if (isolationOn)
      config->setElectronIsolationWP(isolationWP);

    const std::string inputContainer = "Electrons";

    // Retrieval tool
    auto retrieveTool =
      make_unique<AnaToolRetrieve>(args.prefix() + "_retrieve", inputContainer);
    args.add( std::move(retrieveTool) );

    // Pre Correction tool
    auto precorrectTool =
      make_unique<ElectronToolPreCorrect>(args.prefix() + "_precorrect");
    args.add( std::move(precorrectTool) );

    // Correction tool
    auto correctTool =
      make_unique<ElectronToolCorrect>(args.prefix() + "_correct");
    ANA_CHECK( correctTool->setProperty("IsAF2", config->isAFII()) );
    ANA_CHECK( correctTool->setProperty("IsData", config->isData()) );
    args.add( std::move(correctTool) );

    // Selection tool
    auto selectTool = make_unique<ElectronToolSelect>(args.prefix() + "_select");
    ANA_CHECK( selectTool->setProperty("IDString", id) );
    ANA_CHECK( selectTool->setProperty("IsolationOn", isolationOn) );
    ANA_CHECK( selectTool->setProperty("IsolationWP", isolationWP) );
    ANA_CHECK( selectTool->setProperty("IDAna", idAna) );
    ANA_CHECK( selectTool->setProperty("LooseImpactCut", looseImpactCut) );
    args.add( std::move(selectTool) );

    if (!config->isData())
    {
      // Weight tool
      auto weightTool = make_unique<ElectronToolWeight>(args.prefix() + "_weight");
      ANA_CHECK( weightTool->setProperty("IDString", id) );
      if (isolationOn)
      {
        ANA_CHECK( weightTool->setProperty("IsoString", isolationWP) );
      }
      else
      {
        ANA_CHECK( weightTool->setProperty("IsoString", "") );
      }
      ANA_CHECK( weightTool->setProperty("IDAna", idAna));
      ANA_CHECK( weightTool->setProperty("IsAF2", config->isAFII()) );
      args.add( std::move(weightTool) );
    }
    return StatusCode::SUCCESS;
  }

  // Define the wrapper functions with these macros

  QUICK_ANA_ELECTRON_DEFINITION_MAKER
    ( "loose", makeElectronTool(args, "LooseAndBLayerLLH") )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER
    ( "medium", makeElectronTool(args, "MediumLLH") )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER
    ( "tight",  makeElectronTool(args, "TightLLH") )

  QUICK_ANA_ELECTRON_DEFINITION_MAKER
    ( "looseIso",  makeElectronTool(args, "LooseAndBLayerLLH", true) )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER
    ( "mediumIso", makeElectronTool(args, "MediumLLH", true) )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER
    ( "tightIso",  makeElectronTool(args, "TightLLH", true) )

  QUICK_ANA_ELECTRON_DEFINITION_MAKER (
    "loose_IsoTrkOnly",
    makeElectronTool(args, "LooseAndBLayerLLH", true, "LooseTrackOnly")
  )

  QUICK_ANA_ELECTRON_DEFINITION_MAKER (
    "SUSYIso",
    makeElectronTool(args, "LooseAndBLayerLLH", true, "GradientLoose", "TightLLH")
  )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER (
    "SUSYMediumIso",
    makeElectronTool(args, "LooseAndBLayerLLH", true, "GradientLoose", "MediumLLH")
  )
  QUICK_ANA_ELECTRON_DEFINITION_MAKER (
    "SUSYloose",
    makeElectronTool(args, "LooseAndBLayerLLH", false, "", "", true)
  )

} // namespace ana

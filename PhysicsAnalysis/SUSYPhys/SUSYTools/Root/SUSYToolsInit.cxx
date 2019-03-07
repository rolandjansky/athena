/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SUSYTools/SUSYObjDef_xAOD.h"

using namespace ST;

// For the data types to be used in configuring tools
#include "PATCore/PATCoreEnums.h"

// For the tau tool initialization
#include "TauAnalysisTools/Enums.h"

// For the struct needed for OR init
#include "AssociationUtils/OverlapRemovalInit.h"

// Abstract interface classes
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

#include "JetInterface/IJetSelector.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetModifier.h"
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"

#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"

#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"
#include "tauRecTools/ITauToolBase.h"

#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSystematicsTool.h"
#include "METInterface/IMETSignificance.h"

#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
// Can't use the abstract interface for this one (see header comment)
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "PATInterfaces/IWeightTool.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "PathResolver/PathResolver.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

#define CONFIG_EG_EFF_TOOL( TOOLHANDLE, TOOLNAME, CORRFILE )                \
  if( !TOOLHANDLE.isUserConfigured() ) {                                \
    TOOLHANDLE.setTypeAndName("AsgElectronEfficiencyCorrectionTool/"+TOOLNAME); \
    std::vector< std::string > corrFileNameList = {CORRFILE}; \
    ATH_CHECK( TOOLHANDLE.setProperty("CorrectionFileNameList", corrFileNameList) ); \
    if(!isData())                                                        \
      ATH_CHECK (TOOLHANDLE.setProperty("ForceDataType", (int) data_type) ); \
    ATH_CHECK( TOOLHANDLE.setProperty("CorrelationModel", m_EG_corrModel) ); \
    ATH_CHECK( TOOLHANDLE.setProperty("OutputLevel", this->msg().level()) ); \
    ATH_CHECK( TOOLHANDLE.initialize() );                                \
  }

#define CONFIG_EG_EFF_TOOL_KEY( TOOLHANDLE, TOOLNAME, KEYNAME, KEY )        \
  if( !TOOLHANDLE.isUserConfigured() ) {                                \
    TOOLHANDLE.setTypeAndName("AsgElectronEfficiencyCorrectionTool/"+TOOLNAME); \
    std::cout << "Will now set key \"" << KEYNAME << "\" to value \"" << KEY << "\" when configuring an AsgElectronEfficiencyCorrectionTool" << std::endl; \
    ATH_CHECK( TOOLHANDLE.setProperty(KEYNAME, KEY) );                  \
    if(!isData())                                                        \
      ATH_CHECK (TOOLHANDLE.setProperty("ForceDataType", (int) data_type) ); \
    ATH_CHECK( TOOLHANDLE.setProperty("CorrelationModel", m_EG_corrModel) ); \
    ATH_CHECK( TOOLHANDLE.setProperty("OutputLevel", this->msg().level()) ); \
    ATH_CHECK( TOOLHANDLE.initialize() );                                \
  }

#define CONFIG_TAU_TRIGEFF_TOOL(TOOLHANDLE, INDEX, TRIGGER, TAUID )         \
  if (!TOOLHANDLE.isUserConfigured() && m_tau_trig_support.size() > INDEX && !isData()) { \
    toolName = "TauTrigEffTool_" + std::to_string(TAUID) + "_" + TRIGGER; \
    TOOLHANDLE.setTypeAndName("TauAnalysisTools::TauEfficiencyCorrectionsTool/"+toolName); \
    ATH_CHECK(TOOLHANDLE.setProperty("EfficiencyCorrectionTypes", std::vector<int>({TauAnalysisTools::SFTriggerHadTau}) )); \
    ATH_CHECK(TOOLHANDLE.setProperty("TriggerName", TRIGGER));                \
    ATH_CHECK(TOOLHANDLE.setProperty("IDLevel", TAUID ));                \
    ATH_CHECK(TOOLHANDLE.setProperty("PileupReweightingTool", m_prwTool.getHandle() )); \
    ATH_CHECK(TOOLHANDLE.setProperty("OutputLevel", this->msg().level())); \
    ATH_CHECK(TOOLHANDLE.retrieve());                                        \
  }

StatusCode SUSYObjDef_xAOD::SUSYToolsInit()
{
  if (m_dataSource < 0) {
    ATH_MSG_FATAL("Data source incorrectly configured!!");
    ATH_MSG_FATAL("You must set the DataSource property to Data, FullSim or AtlfastII !!");
    ATH_MSG_FATAL("Expect segfaults if you're not checking status codes, which you should be !!");
    return StatusCode::FAILURE;
  }

  if (m_subtool_init) {
    ATH_MSG_INFO("SUSYTools subtools already created. Ignoring this call.");
    ATH_MSG_INFO("Note: No longer necessary to explicitly call SUSYToolsInit. Will avoid creating tools again.");
    return StatusCode::SUCCESS;
  }

  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise PileupReweighting Tool

  if (!m_prwTool.isUserConfigured()) {
    ATH_MSG_DEBUG("Will now init the PRW tool");
    std::vector<std::string> file_conf;
    for (UInt_t i = 0; i < m_prwConfFiles.size(); i++) {
      file_conf.push_back(m_prwConfFiles.at(i));
    }

    std::vector<std::string> file_ilumi;
    for (UInt_t i = 0; i < m_prwLcalcFiles.size(); i++) {
      ATH_MSG_INFO("Adding ilumicalc file: " << m_prwLcalcFiles.at(i));
      file_ilumi.push_back(m_prwLcalcFiles.at(i));
    }

    m_prwTool.setTypeAndName("CP::PileupReweightingTool/PrwTool");
    ATH_CHECK( m_prwTool.setProperty("ConfigFiles", file_conf) );
    ATH_CHECK( m_prwTool.setProperty("LumiCalcFiles", file_ilumi) );
    ATH_CHECK( m_prwTool.setProperty("DataScaleFactor",     m_prwDataSF) ); // 1./1.03 -> default for mc16, see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ExtendedPileupReweighting#Tool_Properties
    ATH_CHECK( m_prwTool.setProperty("DataScaleFactorUP",   m_prwDataSF_UP) ); // 1. -> old value (mc15), as the one for mc16 is still missing
    ATH_CHECK( m_prwTool.setProperty("DataScaleFactorDOWN", m_prwDataSF_DW) ); // 1./1.18 -> old value (mc15), as the one for mc16 is still missing
    ATH_CHECK( m_prwTool.setProperty("TrigDecisionTool", m_trigDecTool.getHandle()) );
    ATH_CHECK( m_prwTool.setProperty("UseRunDependentPrescaleWeight", m_runDepPrescaleWeightPRW) );
    ATH_CHECK( m_prwTool.setProperty("OutputLevel", MSG::WARNING) );
    ATH_CHECK( m_prwTool.retrieve() );
  } else {
    ATH_MSG_INFO("Using user-configured PRW tool");
  }

  std::string toolName; // to be used for tool init below, keeping explicit string constants a minimum /CO

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet calibration tool

  // pick the right config file for the JES tool : https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ApplyJetCalibrationR21
  std::string jetname("AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType)));
  std::string jetcoll(jetname + "Jets");

  if (!m_jetCalibTool.isUserConfigured()) {
    toolName = "JetCalibTool_" + jetname;
    m_jetCalibTool.setTypeAndName("JetCalibrationTool/"+toolName);
    std::string JES_config_file, calibseq;

    if (m_jetInputType == xAOD::JetInput::EMTopo) {
      JES_config_file = m_jesConfig;
      calibseq = m_jesCalibSeq;
    } else if (m_jetInputType == xAOD::JetInput::EMPFlow) {
      JES_config_file = m_jesConfigEMPFlow;
      calibseq = m_jesCalibSeqEMPFlow;
    } else if (m_jetInputType == xAOD::JetInput::LCTopo){
      ATH_MSG_WARNING("LCTopo jets are not fully supported in R21 (no in-situ calibration). Please use either PFlow or EMTopo jets.");
      JES_config_file = "JES_MC16Recommendation_28Nov2017.config"; // old config, thus hard-coded
      calibseq = m_jesCalibSeq; // no in-situ calibration for data
    } else {
      ATH_MSG_ERROR("Unknown (unsupported) jet collection is used, (m_jetInputType = " << m_jetInputType << ")");
      return StatusCode::FAILURE;
    }

    if (isAtlfast()) {
      if (m_jetInputType == xAOD::JetInput::EMTopo) {
        JES_config_file = m_jesConfigAFII;
        calibseq = m_jesCalibSeqAFII;
      } else if (m_jetInputType == xAOD::JetInput::EMPFlow) {
        JES_config_file = m_jesConfigEMPFlowAFII;
        calibseq = m_jesCalibSeqEMPFlowAFII;
      } else {
        ATH_MSG_ERROR("JES recommendations only exist for EMTopo and PFlow jets in AF-II samples (m_jetInputType = " << m_jetInputType << ")");
        return StatusCode::FAILURE;
      }
    }

    if(!m_JMScalib.empty()){ //with JMS calibration (if requested)
      JES_config_file = m_jesConfigJMS;
      calibseq = m_jesCalibSeqJMS;
      if (m_jetInputType == xAOD::JetInput::EMPFlow) {
        ATH_MSG_ERROR("JMS calibration is not supported for EMPFlow jets. Please modify your settings.");
        return StatusCode::FAILURE;
      }
      if (isAtlfast()) {
        ATH_MSG_ERROR("JMS calibration is not supported for AF-II samples. Please modify your settings.");
        return StatusCode::FAILURE;
      }
    }

    // remove Insitu if it's in the string if not data, and add _Smear if not AFII
    if (!isData()) {
      std::string insitu("_Insitu");
      auto found = calibseq.find(insitu);
      if(found != std::string::npos){
	calibseq.erase(found, insitu.length());
	if ( ! isAtlfast() ) calibseq.append("_Smear");
      }
    }

    // now instantiate the tool
    ATH_CHECK( m_jetCalibTool.setProperty("JetCollection", jetname) );
    ATH_CHECK( m_jetCalibTool.setProperty("ConfigFile", JES_config_file) );
    ATH_CHECK( m_jetCalibTool.setProperty("CalibSequence", calibseq) );
    ATH_CHECK( m_jetCalibTool.setProperty("IsData", isData()) );
    ATH_CHECK( m_jetCalibTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetCalibTool.retrieve() );
  }

  //same for fat groomed jets
  std::string fatjetcoll(m_fatJets);
  if (fatjetcoll.size()>3) fatjetcoll = fatjetcoll.substr(0,fatjetcoll.size()-4); //remove (new) suffix
  if (!m_jetFatCalibTool.isUserConfigured() && !m_fatJets.empty()) {
    toolName = "JetFatCalibTool_" + m_fatJets;
    m_jetFatCalibTool.setTypeAndName("JetCalibrationTool/"+toolName);

    // now instantiate the tool
    ATH_CHECK( m_jetFatCalibTool.setProperty("JetCollection", fatjetcoll) );
    ATH_CHECK( m_jetFatCalibTool.setProperty("ConfigFile", m_jesConfigFat) );
    ATH_CHECK( m_jetFatCalibTool.setProperty("CalibSequence", m_jesCalibSeqFat) );
    // always set to false : https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ApplyJetCalibrationR21
    ATH_CHECK( m_jetFatCalibTool.setProperty("IsData", false) );
    ATH_CHECK( m_jetFatCalibTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetFatCalibTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise Boson taggers
  if (!m_WTaggerTool.isUserConfigured() && !m_WtagConfig.empty()) {
    m_WTaggerTool.setTypeAndName("SmoothedWZTagger/WTagger");
    ATH_CHECK( m_WTaggerTool.setProperty("ConfigFile",m_WtagConfig) );
    ATH_CHECK( m_WTaggerTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_WTaggerTool.retrieve() );
  }

  if (!m_ZTaggerTool.isUserConfigured() && !m_ZtagConfig.empty()) {
    m_ZTaggerTool.setTypeAndName("SmoothedWZTagger/ZTagger");
    ATH_CHECK( m_ZTaggerTool.setProperty("ConfigFile",m_ZtagConfig) );
    ATH_CHECK( m_ZTaggerTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_ZTaggerTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet uncertainty tool
  ATH_MSG_INFO("Set up Jet Uncertainty tool...");

  if (!m_jetUncertaintiesTool.isUserConfigured()) {
    std::string jetdef("AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType)));

    if(jetdef != "AntiKt4EMTopo" && jetdef !="AntiKt4EMPFlow"){
      ATH_MSG_WARNING("Jet Uncertaintes recommendations only exist for EMTopo and PFlow jets, falling back to AntiKt4EMTopo");
      jetdef = "AntiKt4EMTopo";
    }
    toolName = "JetUncertaintiesTool_" + jetdef;

    if(!m_JMScalib.empty()){
      if(m_jetUncertaintiesConfig.find("JMS")==std::string::npos){
        ATH_MSG_ERROR("You turned on JMS calibration but your JetUncertainties config (Jet.UncertConfig=" << m_jetUncertaintiesConfig << ") isn't for JMS. Please modify your settings.");
        return StatusCode::FAILURE;
      }
      // Make sure we got a sensible value for the configuration
      if (m_JMScalib!="Frozen" && m_JMScalib!="Extrap"){
        ATH_MSG_ERROR("JMS calibration uncertainty must be either Frozen or Extrap.  " << m_JMScalib << " is not a supported value. Please modify your settings.");
        return StatusCode::FAILURE;
      }
    }
    m_jetUncertaintiesTool.setTypeAndName("JetUncertaintiesTool/"+toolName);

    // Allowing for IsData = true (pseudo-data smearing) if AllJER or FullJER config is set in m_jetUncertaintiesConfig
    bool JERUncPDsmearing = isData() ? isData() : m_jetUncertaintiesPDsmearing;
    if (m_jetUncertaintiesConfig.find("SimpleJER") != std::string::npos) JERUncPDsmearing = isData();

    ATH_CHECK( m_jetUncertaintiesTool.setProperty("JetDefinition", jetdef) );
    ATH_CHECK( m_jetUncertaintiesTool.setProperty("MCType", isAtlfast() ? "AFII" : "MC16") );
    ATH_CHECK( m_jetUncertaintiesTool.setProperty("IsData", JERUncPDsmearing) );
    //  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Summer2018SmallR
    ATH_CHECK( m_jetUncertaintiesTool.setProperty("ConfigFile", m_jetUncertaintiesConfig) );
    if (m_jetUncertaintiesCalibArea != "default") ATH_CHECK( m_jetUncertaintiesTool.setProperty("CalibArea", m_jetUncertaintiesCalibArea) );
    ATH_CHECK( m_jetUncertaintiesTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetUncertaintiesTool.retrieve() );
  }

  // Initialise jet uncertainty tool for fat jets
  if (!m_fatjetUncertaintiesTool.isUserConfigured() && !m_fatJets.empty() && !m_fatJetUncConfig.empty()) {

    toolName = "JetUncertaintiesTool_" + m_fatJets;
    m_fatjetUncertaintiesTool.setTypeAndName("JetUncertaintiesTool/"+toolName);

    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("JetDefinition", fatjetcoll) );
    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("MCType", "MC16a") );
    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("IsData", isData()) );
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Moriond2018LargeR
    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("ConfigFile", m_fatJetUncConfig) );
    if (m_jetUncertaintiesCalibArea != "default") ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("CalibArea", m_jetUncertaintiesCalibArea) );

    //Restrict variables to be shifted if (required)
    if( m_fatJetUncVars != "default" ){
      std::vector<std::string> shift_vars = {};

      std::string temp(m_fatJetUncVars);
      do {
        auto pos = temp.find(",");
        shift_vars.push_back(temp.substr(0, pos));
        if (pos == std::string::npos)
          temp = "";
        else
          temp = temp.substr(pos + 1);

      }
      while (!temp.empty() );

      ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("VariablesToShift", shift_vars) );
    }

    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_fatjetUncertaintiesTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet cleaning tools

  if (m_badJetCut!="" && !m_jetCleaningTool.isUserConfigured()) {
    toolName = "JetCleaningTool";
    m_jetCleaningTool.setTypeAndName("JetCleaningTool/"+toolName);
    ATH_CHECK( m_jetCleaningTool.setProperty("CutLevel", m_badJetCut) );
    ATH_CHECK( m_jetCleaningTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetCleaningTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise JVT tool

  if (!m_jetJvtUpdateTool.isUserConfigured()) {
    toolName = "JetVertexTaggerTool";
    m_jetJvtUpdateTool.setTypeAndName("JetVertexTaggerTool/"+toolName);
    ATH_CHECK( m_jetJvtUpdateTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetJvtUpdateTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet JVT efficiency tool

  m_applyJVTCut = m_JVT_WP!="";
  if (!m_jetJvtEfficiencyTool.isUserConfigured() && m_applyJVTCut) {
    toolName = "JVTEfficiencyTool";
    m_jetJvtEfficiencyTool.setTypeAndName("CP::JetJvtEfficiency/"+toolName);
    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("WorkingPoint",m_JVT_WP) );
    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("MaxPtForJvt",m_JvtPtMax) );
    // Set the decoration to the name we used to use
    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("ScaleFactorDecorationName","jvtscalefact") );
    // Set the jvt moment (the one we update!)
    //    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("JetJvtMomentName", "Jvt") ); //default!

    if (m_jetInputType == xAOD::JetInput::EMTopo){
      ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2018/JvtSFFile_EMTopoJets.root") );
    } else if (m_jetInputType == xAOD::JetInput::LCTopo){
      ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2018/JvtSFFile_LC.root") );
    } else if (m_jetInputType == xAOD::JetInput::EMPFlow){
      ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2018/JvtSFFile_EMPFlow.root") );
    } else {
      ATH_MSG_ERROR("Cannot configure JVT uncertainties for unsupported jet input type (neither EM nor LC)");
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetJvtEfficiencyTool.retrieve() );
  }
  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet f-JVT efficiency tool for scale factors

  if (!m_jetFJvtEfficiencyTool.isUserConfigured()) {
    toolName = m_doFwdJVT ? m_metJetSelection+"_fJVT" : m_metJetSelection+"_NOfJVT";
    m_jetFJvtEfficiencyTool.setTypeAndName("CP::JetJvtEfficiency/FJVTEfficiencyTool_"+toolName);
    if(m_fwdjetTightOp)
      ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("WorkingPoint","Tight") );
    else
      ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("WorkingPoint","Medium") );
    ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("MaxPtForJvt",m_fwdjetPtMax) );
    ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("JetfJvtMomentName","passFJvt") );
    // Set the decoration to the name we used to use
    ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("ScaleFactorDecorationName","fJVTSF") );
    ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2018/fJvtSFFile.root"));
    ATH_CHECK( m_jetFJvtEfficiencyTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetFJvtEfficiencyTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise FwdJVT tool

  if (!m_jetFwdJvtTool.isUserConfigured()) {
    toolName = m_doFwdJVT ? m_metJetSelection+"_fJVT" : m_metJetSelection+"_NOfJVT";
    m_jetFwdJvtTool.setTypeAndName("JetForwardJvtTool/FJVTTool_"+toolName);
    ATH_CHECK( m_jetFwdJvtTool.setProperty("OutputDec", "passFJvt") ); //Output decoration
    // fJVT WPs depend on the MET WP, see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsRel21p2#fJVT_and_MET
    if (m_doFwdJVT && m_metJetSelection == "Tight") {
      ATH_CHECK( m_jetFwdJvtTool.setProperty("UseTightOP", true) ); // Tight
    } else if (m_doFwdJVT && (m_metJetSelection == "Tenacious" || m_metJetSelection == "TenaciousJVT641")) {
      ATH_CHECK( m_jetFwdJvtTool.setProperty("UseTightOP", false) ); // Loose
    } else {
      ATH_CHECK( m_jetFwdJvtTool.setProperty("UseTightOP", m_fwdjetTightOp) ); // Tight or Loose
    }
    ATH_CHECK( m_jetFwdJvtTool.setProperty("EtaThresh", m_fwdjetEtaMin) );   //Eta dividing central from forward jets
    ATH_CHECK( m_jetFwdJvtTool.setProperty("ForwardMaxPt", m_fwdjetPtMax) ); //Max Pt to define fwdJets for JVT
    ATH_CHECK( m_jetFwdJvtTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_jetFwdJvtTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon calibration tool

  if (!m_muonCalibrationAndSmearingTool.isUserConfigured()) {
    m_muonCalibrationAndSmearingTool.setTypeAndName("CP::MuonCalibrationPeriodTool/MuonCalibrationAndSmearingTool");
    ATH_CHECK( m_muonCalibrationAndSmearingTool.setProperty("calibrationMode", m_muCalibrationMode) );
    ATH_CHECK( m_muonCalibrationAndSmearingTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonCalibrationAndSmearingTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon selection tool

  std::string muQualBaseline = "";
  switch (m_muIdBaseline) {
  case (int)xAOD::Muon::Quality(xAOD::Muon::VeryLoose):  muQualBaseline = "VeryLoose";
    ATH_MSG_WARNING("No muon scale factors are available for VeryLoose working point.");
    break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Loose):  muQualBaseline = "Loose";  break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Medium): muQualBaseline = "Medium"; break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Tight):  muQualBaseline = "Tight";  break;
  case 4:  muQualBaseline = "HighPt";  break;
  case 5:  muQualBaseline = "LowPt";  break;
  default:
    ATH_MSG_ERROR("Invalid muon working point provided: " << m_muIdBaseline << ". Cannot initialise!");
    return StatusCode::FAILURE;
    break;
  }

  if (!m_muonSelectionToolBaseline.isUserConfigured()) {
    toolName = "MuonSelectionTool_Baseline_" + muQualBaseline;
    m_muonSelectionToolBaseline.setTypeAndName("CP::MuonSelectionTool/"+toolName);
    if (m_muBaselineEta<m_muEta){  // Test for inconsistent configuration
      ATH_MSG_ERROR( "Requested a baseline eta cut for muons (" << m_muBaselineEta <<
                     ") that is tighter than the signal cut (" << m_muEta << ").  Please check your config." );
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "MaxEta", m_muBaselineEta) );
    //      ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "MuQuality", int(m_muIdBaseline) ) );
    ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "MuQuality", m_muIdBaseline ) );
    ATH_CHECK( m_muonSelectionToolBaseline.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonSelectionToolBaseline.retrieve() );
  }

  std::string muQual = "";
  switch (m_muId) {
  case (int)xAOD::Muon::Quality(xAOD::Muon::VeryLoose):  muQual = "VeryLoose";
    ATH_MSG_WARNING("No muon scale factors are available for VeryLoose working point.");
    break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Loose):  muQual = "Loose";  break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Medium): muQual = "Medium"; break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Tight):  muQual = "Tight";  break;
  case 4:  muQual = "HighPt";  break;
  case 5:  muQual = "LowPt";  break;
  default:
    ATH_MSG_ERROR("Invalid muon working point provided: " << m_muId << ". Cannot initialise!");
    return StatusCode::FAILURE;
    break;
  }

  if (!m_muonSelectionTool.isUserConfigured()) {
    toolName = "MuonSelectionTool_" + muQual;
    m_muonSelectionTool.setTypeAndName("CP::MuonSelectionTool/"+toolName);
    ATH_CHECK( m_muonSelectionTool.setProperty( "MaxEta", m_muEta) );
    //      ATH_CHECK( m_muonSelectionTool.setProperty( "MuQuality", int(m_muId) ) );
    ATH_CHECK( m_muonSelectionTool.setProperty( "MuQuality", m_muId ) );
    ATH_CHECK( m_muonSelectionTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonSelectionTool.retrieve() );
  }

  if (!m_muonSelectionHighPtTool.isUserConfigured()) { //Fixed to HighPt WP
    toolName = "MuonSelectionHighPtTool_" + muQual;
    m_muonSelectionHighPtTool.setTypeAndName("CP::MuonSelectionTool/"+toolName);
    ATH_CHECK( m_muonSelectionHighPtTool.setProperty( "MaxEta", m_muEta) );
    ATH_CHECK( m_muonSelectionHighPtTool.setProperty( "MuQuality", 4 ) );
    ATH_CHECK( m_muonSelectionHighPtTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonSelectionHighPtTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon efficiency tools
  if (!m_muonEfficiencySFTool.isUserConfigured() && m_muId != xAOD::Muon::VeryLoose) {
    toolName = "MuonEfficiencyScaleFactors_" + muQual;
    m_muonEfficiencySFTool.setTypeAndName("CP::MuonEfficiencyScaleFactors/"+toolName);
    ATH_CHECK( m_muonEfficiencySFTool.setProperty("WorkingPoint", muQual) );
    ATH_CHECK( m_muonEfficiencySFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonEfficiencySFTool.retrieve() );
  }

  if (!m_muonEfficiencyBMHighPtSFTool.isUserConfigured()){
    toolName = "MuonEfficiencyScaleFactorsBMHighPt_" + muQual;
    m_muonEfficiencyBMHighPtSFTool.setTypeAndName("CP::MuonEfficiencyScaleFactors/"+toolName);
    ATH_CHECK( m_muonEfficiencyBMHighPtSFTool.setProperty("WorkingPoint", "BadMuonVeto_HighPt") );
    ATH_CHECK( m_muonEfficiencyBMHighPtSFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonEfficiencyBMHighPtSFTool.retrieve() );
  }

  if (m_doTTVAsf && m_mud0sig<0 && m_muz0<0){
    ATH_MSG_WARNING("Requested TTVA SFs without d0sig and z0 cuts. Disabling scale factors as they will not make sense.");
    m_doTTVAsf=false;
  }

  if (!m_muonTTVAEfficiencySFTool.isUserConfigured()) {
    toolName = "MuonTTVAEfficiencyScaleFactors";
    m_muonTTVAEfficiencySFTool.setTypeAndName("CP::MuonEfficiencyScaleFactors/"+toolName);
    ATH_CHECK( m_muonTTVAEfficiencySFTool.setProperty("WorkingPoint", "TTVA") );
    ATH_CHECK( m_muonTTVAEfficiencySFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonTTVAEfficiencySFTool.retrieve() );
  }


  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon isolation tool
  if (!m_muonIsolationSFTool.isUserConfigured()) {
    toolName = "MuonIsolationScaleFactors_" + m_muIso_WP;

    std::string tmp_muIso_WP = m_muIso_WP;
    if ( !check_isOption(m_muIso_WP, m_mu_iso_support) ) { //check if supported
      ATH_MSG_WARNING("Your selected muon Iso WP ("
		      << m_muIso_WP
		      << ") does not have SFs defined. Will try to find an appropriate fall-back.");
      if (m_mu_iso_fallback.count(m_muIso_WP) > 0){
	tmp_muIso_WP = m_mu_iso_fallback[m_muIso_WP];
	ATH_MSG_WARNING("Your selected muon Iso WP ("
			<< m_muIso_WP
			<< " is not supported, and does not have SFs available.  Falling back to "
			<< tmp_muIso_WP
			<< " for SF determination.");
      } else {
        ATH_MSG_ERROR("***  The muon isolation WP you selected (" << m_muIso_WP << ") is not currentely supported, and no known fall-back option for SFs exists. Sorry! ***");
        return StatusCode::FAILURE;
      }
    }

    m_muonIsolationSFTool.setTypeAndName("CP::MuonEfficiencyScaleFactors/"+toolName);
    // Use for the low-pt WP a dedicated set of isolation scale-factors having an extra uncertainty in place
    ATH_CHECK( m_muonIsolationSFTool.setProperty("WorkingPoint",(m_muId == 5 ? "LowPt_" : "") + tmp_muIso_WP + "Iso") );
    ATH_CHECK( m_muonIsolationSFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonIsolationSFTool.retrieve() );

  }

  if (!m_muonHighPtIsolationSFTool.isUserConfigured()) {
    toolName = "MuonHighPtIsolationScaleFactors_" + m_muIsoHighPt_WP;

    std::string tmp_muIsoHighPt_WP = m_muIsoHighPt_WP;
    if ( !check_isOption(m_muIsoHighPt_WP, m_mu_iso_support) ) { //check if supported
      ATH_MSG_WARNING("Your selected muon high-pt Iso WP ("
		      << m_muIsoHighPt_WP
		      << ") does not have SFs defined. Will try to find an appropriate fall-back.");
      if (m_mu_iso_fallback.count(m_muIsoHighPt_WP) > 0){
	tmp_muIsoHighPt_WP = m_mu_iso_fallback[m_muIsoHighPt_WP];
	ATH_MSG_WARNING("Your selected muon high-pt Iso WP ("
			<< m_muIsoHighPt_WP
			<< " is not supported, and does not have SFs available.  Falling back to "
			<< tmp_muIsoHighPt_WP
			<< " for SF determination.");
      } else {
        ATH_MSG_ERROR("***  The muon isolation WP you selected (" << m_muIsoHighPt_WP << ") is not currentely supported, and no known fall-back option for SFs exists. Sorry! ***");
        return StatusCode::FAILURE;
      }
    }

    m_muonHighPtIsolationSFTool.setTypeAndName("CP::MuonEfficiencyScaleFactors/"+toolName);
    // Use for the low-pt WP a dedicated set of isolation scale-factors having an extra uncertainty in place
    ATH_CHECK( m_muonHighPtIsolationSFTool.setProperty("WorkingPoint",(m_muId == 5 ? "LowPt_" : "") + tmp_muIsoHighPt_WP + "Iso") );
    ATH_CHECK( m_muonHighPtIsolationSFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonHighPtIsolationSFTool.retrieve() );

  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon trigger scale factor tools

  if (!m_muonTriggerSFTool.isUserConfigured()) {
    toolName = "MuonTriggerScaleFactors_" + muQual;
    m_muonTriggerSFTool.setTypeAndName("CP::MuonTriggerScaleFactors/"+toolName);
    if ( muQual=="LowPt" ) {
      ATH_MSG_WARNING("You're using the LowPt muon selection, which is not supported yet in terms of muon trigger scale facorts. TEMPORAIRLY configuring the muonTriggerSFTool for Medium muons. Beware!");
      ATH_CHECK( m_muonTriggerSFTool.setProperty("MuonQuality", "Medium" ) );
    }
    else ATH_CHECK( m_muonTriggerSFTool.setProperty("MuonQuality", muQual) );
    //ATH_CHECK( m_muonTriggerSFTool.setProperty("Isolation", m_muIso_WP)); This property has been depreacted long time ago
    ATH_CHECK( m_muonTriggerSFTool.setProperty("AllowZeroSF", true) );
    ATH_CHECK( m_muonTriggerSFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_muonTriggerSFTool.retrieve() );
    m_muonTrigSFTools.push_back(m_muonTriggerSFTool.getHandle());
  }

  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise electron selector tools

  // Signal Electrons
  if (!m_elecSelLikelihood.isUserConfigured()) {
    toolName = "EleSelLikelihood_" + m_eleId;
    m_elecSelLikelihood.setTypeAndName("AsgElectronLikelihoodTool/"+toolName);

    if (! m_eleConfig.empty() ){
      ATH_MSG_INFO("Overriding specified Ele.Id working point in favour of configuration file");
      ATH_CHECK( m_elecSelLikelihood.setProperty("ConfigFile", m_eleConfig) );
    } else if ( !check_isOption(m_eleId, m_el_id_support) ) { //check if supported
      ATH_MSG_ERROR("Invalid electron ID selected: " << m_eleId);
      return StatusCode::FAILURE;
    }
    else if (m_eleId == "VeryLooseLLH" || m_eleId == "LooseLLH") {
      ATH_MSG_WARNING(" ****************************************************************************");
      ATH_MSG_WARNING(" CAUTION: Setting " << m_eleId << " as signal electron ID");
      ATH_MSG_WARNING(" These may be used for loose electron CRs but no scale factors are provided.");
      ATH_MSG_WARNING(" ****************************************************************************");
      ATH_CHECK( m_elecSelLikelihood.setProperty("WorkingPoint", EG_WP(m_eleId) ));
    } else {
      ATH_CHECK( m_elecSelLikelihood.setProperty("WorkingPoint", EG_WP(m_eleId) ));
    }

    ATH_CHECK( m_elecSelLikelihood.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_elecSelLikelihood.retrieve() );
  }

  // Baseline Electrons
  if (!m_elecSelLikelihoodBaseline.isUserConfigured()) {
    toolName = "EleSelLikelihoodBaseline_" + m_eleIdBaseline;
    m_elecSelLikelihoodBaseline.setTypeAndName("AsgElectronLikelihoodTool/"+toolName);

    if (! m_eleConfigBaseline.empty() ){
      ATH_MSG_INFO("Overriding specified EleBaseline.Id working point in favour of configuration file");
      ATH_CHECK( m_elecSelLikelihoodBaseline.setProperty("ConfigFile", m_eleConfigBaseline ));
    } else if ( !check_isOption(m_eleIdBaseline, m_el_id_support) ) { //check if supported
      ATH_MSG_ERROR("Invalid electron ID selected: " << m_eleIdBaseline);
      return StatusCode::FAILURE;
    } else {
      ATH_CHECK( m_elecSelLikelihoodBaseline.setProperty("WorkingPoint", EG_WP(m_eleIdBaseline)) );
    }

    ATH_CHECK( m_elecSelLikelihoodBaseline.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_elecSelLikelihoodBaseline.retrieve() );
  }


  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise photon selector tools

  if (!m_photonSelIsEM.isUserConfigured()) {
    toolName = "PhotonSelIsEM_" + m_photonId;
    m_photonSelIsEM.setTypeAndName("AsgPhotonIsEMSelector/"+toolName);

    if (!check_isOption(m_photonId, m_ph_id_support)){ //check if supported
      ATH_MSG_ERROR("Invalid photon ID selected: " << m_photonId);
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_photonSelIsEM.setProperty("WorkingPoint", m_photonId+"Photon") );
    ATH_CHECK( m_photonSelIsEM.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_photonSelIsEM.retrieve() );
  }

  if (!m_photonSelIsEMBaseline.isUserConfigured()) {
    toolName = "PhotonSelIsEMBaseline_" + m_photonIdBaseline;
    m_photonSelIsEMBaseline.setTypeAndName("AsgPhotonIsEMSelector/"+toolName);

    if(!check_isOption(m_photonIdBaseline, m_ph_id_support)){ //check if supported
      ATH_MSG_ERROR("Invalid photon ID selected: " << m_photonIdBaseline);
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_photonSelIsEMBaseline.setProperty("WorkingPoint", m_photonIdBaseline+"Photon") );
    ATH_CHECK( m_photonSelIsEMBaseline.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_photonSelIsEMBaseline.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise electron efficiency tool

  PATCore::ParticleDataType::DataType data_type(PATCore::ParticleDataType::Data);
  if (!isData()) {
    if (isAtlfast()) data_type = PATCore::ParticleDataType::Fast;
    else data_type = PATCore::ParticleDataType::Full;
    ATH_MSG_DEBUG( "Setting data type to " << data_type);
  }

  toolName = "AsgElectronEfficiencyCorrectionTool_reco";
  CONFIG_EG_EFF_TOOL_KEY(m_elecEfficiencySFTool_reco, toolName, "RecoKey", "Reconstruction");

  if (m_eleId == "VeryLooseLLH" || m_eleId == "LooseLLH" || m_eleId == "Loose" || m_eleId == "Medium" || m_eleId == "Tight") {
    ATH_MSG_WARNING("Not configuring electron ID and trigger scale factors for " << m_eleId);
  }
  else {

    // This needs to be formatted for the scale factors: no _Rel20, no LH label, etc.
    std::string eleId = TString(m_eleId).ReplaceAll("AndBLayer", "BLayer").ReplaceAll("LLH", "").Data();

    // electron id
    toolName = "AsgElectronEfficiencyCorrectionTool_id_" + m_eleId;
    CONFIG_EG_EFF_TOOL_KEY(m_elecEfficiencySFTool_id, toolName, "IdKey", eleId);

    // electron iso
    toolName = "AsgElectronEfficiencyCorrectionTool_iso_" + m_eleId + m_eleIso_WP;
    // can't do the iso tool via the macro, it needs two properties set
    if ( !m_elecEfficiencySFTool_iso.isUserConfigured() ) {

      if ( !check_isOption(m_eleIso_WP, m_el_iso_support) ) { //check if supported
	ATH_MSG_WARNING( "Your electron Iso WP: " << m_eleIso_WP
			 << " is no longer supported. This will almost certainly cause a crash now.");
      }

      m_elecEfficiencySFTool_iso.setTypeAndName("AsgElectronEfficiencyCorrectionTool/"+toolName);

      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("MapFilePath", m_eleEffMapFilePath) );
      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("IsoKey", m_eleIso_WP) );
      if (!isData()) {
        ATH_CHECK (m_elecEfficiencySFTool_iso.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("CorrelationModel", m_EG_corrModel) );
      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( m_elecEfficiencySFTool_iso.initialize() );
    }

    // electron iso high-pt
    toolName = "AsgElectronEfficiencyCorrectionTool_isoHigPt_" + m_eleId + m_eleIsoHighPt_WP;
    // can't do the iso tool via the macro, it needs two properties set
    if ( !m_elecEfficiencySFTool_isoHighPt.isUserConfigured() ) {

      if ( !check_isOption(m_eleIsoHighPt_WP, m_el_iso_support) ) { //check if supported
	ATH_MSG_WARNING( "Your electron high-pt Iso WP: " << m_eleIsoHighPt_WP
			 << " is no longer supported. This will almost certainly cause a crash now.");
      }

      m_elecEfficiencySFTool_isoHighPt.setTypeAndName("AsgElectronEfficiencyCorrectionTool/"+toolName);

      ATH_CHECK( m_elecEfficiencySFTool_isoHighPt.setProperty("MapFilePath", m_eleEffMapFilePath) );
      ATH_CHECK( m_elecEfficiencySFTool_isoHighPt.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_isoHighPt.setProperty("IsoKey", m_eleIsoHighPt_WP) );
      if (!isData()) {
        ATH_CHECK (m_elecEfficiencySFTool_isoHighPt.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_isoHighPt.setProperty("CorrelationModel", m_EG_corrModel) );
      ATH_CHECK( m_elecEfficiencySFTool_isoHighPt.setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( m_elecEfficiencySFTool_isoHighPt.initialize() );
    }

    //-- get KEYS supported by egamma SF tools
    std::vector<std::string> eSF_keys = getElSFkeys(m_eleEffMapFilePath);

    // electron triggers - first SFs (but we need to massage the id string since all combinations are not supported)

    //single lepton
    std::string triggerEleIso("");
    if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringSingle+"_"+eleId+"_"+m_eleIso_WP) != eSF_keys.end()){
      triggerEleIso   = m_eleIso_WP;
    } else if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringSingle+"_"+eleId+"_"+m_el_iso_fallback[m_eleIso_WP]) != eSF_keys.end()){
      //--- Check to see if the only issue is an unknown isolation working point
      triggerEleIso = m_el_iso_fallback[m_eleIso_WP];
      ATH_MSG_WARNING("Your selected electron Iso WP ("
		      << m_eleIso_WP
		      << ") does not have trigger SFs defined. Falling back to "
		      << triggerEleIso
		      << " for SF calculations");
    }
    else{
      ATH_MSG_ERROR("***  THE ELECTRON TRIGGER SF YOU SELECTED (" << m_electronTriggerSFStringSingle << ") GOT NO SUPPORT FOR YOUR ID+ISO WPs (" << m_eleId << "+" << m_eleIso_WP << ") ***");
      return StatusCode::FAILURE;
    }

    toolName = "AsgElectronEfficiencyCorrectionTool_trig_singleLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trig_singleLep.isUserConfigured() ) {
      m_elecEfficiencySFTool_trig_singleLep.setTypeAndName("AsgElectronEfficiencyCorrectionTool/"+toolName);
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("MapFilePath", m_eleEffMapFilePath) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("TriggerKey", m_electronTriggerSFStringSingle) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("IsoKey", triggerEleIso) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("CorrelationModel", m_EG_corrModel) );
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.initialize() );
    }

    toolName = "AsgElectronEfficiencyCorrectionTool_trigEff_singleLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trigEff_singleLep.isUserConfigured() ) {
      m_elecEfficiencySFTool_trigEff_singleLep.setTypeAndName("AsgElectronEfficiencyCorrectionTool/"+toolName);
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("MapFilePath", m_eleEffMapFilePath) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("TriggerKey", "Eff_"+m_electronTriggerSFStringSingle) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("IsoKey", triggerEleIso) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("CorrelationModel", m_EG_corrModel) );
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.initialize() );
    }

    //mixed-leptons
    std::map<std::string,std::string> electronTriggerSFMapMixedLepton {
      // legs, Trigger keys, 
      {"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose,e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", m_electronTriggerSFStringSingle},
      {"e24_lhvloose_nod0_L1EM20VH,e17_lhvloose_nod0,e12_lhloose_L1EM10VH","DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0_2017_2018_e24_lhvloose_nod0_L1EM20VH"},
      {"e26_lhmedium_nod0_L1EM22VHI,e26_lhmedium_nod0","MULTI_L_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhmedium_nod0_L1EM22VHI_2017_2018_e26_lhmedium_nod0"},
      {"e17_lhloose,e17_lhloose_nod0","MULTI_L_2015_e17_lhloose_2016_2018_e17_lhloose_nod0"},
      {"e12_lhloose,e12_lhloose_nod0","MULTI_L_2015_e12_lhloose_2016_2018_e12_lhloose_nod0"},
      {"e7_lhmedium,e7_lhmedium_nod0","MULTI_L_2015_e7_lhmedium_2016_2018_e7_lhmedium_nod0"},
      {"e9_lhloose,e9_lhloose_nod0,e12_lhvloose_nod0_L1EM10VH","TRI_E_2015_e9_lhloose_2016_e9_lhloose_nod0_2017_2018_e12_lhvloose_nod0_L1EM10VH"}
      
    };

    std::string triggerMixedEleIso("");

    for(auto const& item : electronTriggerSFMapMixedLepton){

      if (std::find(eSF_keys.begin(), eSF_keys.end(), item.second+"_"+eleId+"_"+m_eleIso_WP) != eSF_keys.end()){
        triggerMixedEleIso = m_eleIso_WP;
      } else if (std::find(eSF_keys.begin(), eSF_keys.end(), item.second+"_"+eleId+"_"+m_el_iso_fallback[m_eleIso_WP]) != eSF_keys.end()){
	//--- Check to see if the only issue is an unknown isolation working point
	triggerMixedEleIso = m_el_iso_fallback[m_eleIso_WP];
	ATH_MSG_WARNING("Your selected electron Iso WP ("
			<< m_eleIso_WP
			<< ") does not have trigger SFs defined. Falling back to "
			<< triggerMixedEleIso
			<< " for SF calculations");
      }
      else{
        ATH_MSG_ERROR("***  THE ELECTRON TRIGGER SF YOU SELECTED (" << item.second << ") GOT NO SUPPORT FOR YOUR ID+ISO WPs (" << m_eleId << "+" << m_eleIso_WP << "). The fallback options failed as well sorry! ***");
        return StatusCode::FAILURE;
      }

      ATH_MSG_VERBOSE ("Selected WP: " << item.second << "_" << eleId << "_" << triggerMixedEleIso);

      toolName = "AsgElectronEfficiencyCorrectionTool_trig_mixLep_" + (item.first).substr(0,8) + m_eleId;
      auto t_sf = m_elecEfficiencySFTool_trig_mixLep.emplace(m_elecEfficiencySFTool_trig_mixLep.end(), "AsgElectronEfficiencyCorrectionTool/"+toolName);
      ATH_CHECK( t_sf->setProperty("MapFilePath", m_eleEffMapFilePath) );
      ATH_CHECK( t_sf->setProperty("TriggerKey", item.second) );
      ATH_CHECK( t_sf->setProperty("IdKey", eleId) );
      ATH_CHECK( t_sf->setProperty("IsoKey", triggerMixedEleIso) );
      ATH_CHECK( t_sf->setProperty("CorrelationModel", m_EG_corrModel) );
      if (!isData()) {
        ATH_CHECK( t_sf->setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( t_sf->setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( t_sf->initialize() );
      m_elecTrigSFTools.push_back(t_sf->getHandle());
#ifndef XAOD_STANDALONE
      m_legsPerTool[toolName] = item.first;
#else
      m_legsPerTool["ToolSvc."+toolName] = item.first;
#endif

      toolName = "AsgElectronEfficiencyCorrectionTool_trigEff_mixLep_" + (item.first).substr(0,8) + m_eleId;
      auto t_eff = m_elecEfficiencySFTool_trigEff_mixLep.emplace(m_elecEfficiencySFTool_trigEff_mixLep.end(), "AsgElectronEfficiencyCorrectionTool/"+toolName);
      ATH_CHECK( t_eff->setProperty("MapFilePath", m_eleEffMapFilePath) );
      ATH_CHECK( t_eff->setProperty("TriggerKey", "Eff_"+item.second) );
      ATH_CHECK( t_eff->setProperty("IdKey", eleId) );
      ATH_CHECK( t_eff->setProperty("IsoKey", triggerMixedEleIso) );
      ATH_CHECK( t_eff->setProperty("CorrelationModel", m_EG_corrModel) );
      if (!isData()) {
        ATH_CHECK( t_eff->setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( t_eff->setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( t_eff->initialize() );
      m_elecTrigEffTools.push_back(t_eff->getHandle());
#ifndef XAOD_STANDALONE
      m_legsPerTool[toolName] = item.first;
#else
      m_legsPerTool["ToolSvc."+toolName] = item.first;
#endif

    }
  }

   // Electron ChargeID Selector tool SF (No SF yet for R21 as of 2019.02.17)
   // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronChargeFlipTaggerTool#Calculating_the_ECIDS_decision
   toolName = "AsgElectronEfficiencyCorrectionTool_chf_" + m_eleId + m_eleIso_WP + m_eleChID_WP;
   CONFIG_EG_EFF_TOOL(m_elecEfficiencySFTool_chf, toolName, "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/efficiencySF.ChargeID.MediumLLH_d0z0_v11_isolGradient_MediumCFT.root");
   m_runECIS = m_eleChID_WP.empty() ? false : true;

  // Electron charge mis-identification SFs
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/LatestRecommendationsElectronIDRun2#Scale_factors_for_electrons_char
  toolName = "ElectronChargeEfficiencyCorrectionTool_" + m_eleId + m_eleIso_WP;
  if ( !m_elecChargeEffCorrTool.isUserConfigured() ) {
    m_elecChargeEffCorrTool.setTypeAndName("CP::ElectronChargeEfficiencyCorrectionTool/"+toolName);

    std::string tmpIDWP = m_eleId;
    if (tmpIDWP != "MediumLLH" && tmpIDWP != "TightLLH") {
      ATH_MSG_WARNING( "** Only MediumLLH & TightLLH ID WP are supported for ChargeID SFs at the moment. Falling back to MediumLLH, be aware! **");
      tmpIDWP = "MediumLLH";
    }

    std::string tmpIsoWP = m_eleIso_WP;
    if ( !check_isOption(tmpIsoWP, m_el_iso_support) ) { //check if supported
	ATH_MSG_WARNING( "Your electron Iso WP: " << m_eleIso_WP
			 << " is no longer supported. This will almost certainly cause a crash now.");
    }

    std::string chfFile("ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/charge_misID/chargeEfficiencySF."+tmpIDWP+"_d0z0_v13_"+tmpIsoWP+".root");

    ATH_CHECK( m_elecChargeEffCorrTool.setProperty("CorrectionFileName",chfFile) );

    if (!isData()) {
      ATH_CHECK ( m_elecChargeEffCorrTool.setProperty("ForceDataType", (int) data_type) );
    }
    ATH_CHECK( m_elecChargeEffCorrTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_elecChargeEffCorrTool.initialize() );
  }

  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise photon efficiency tool

  if (!m_photonEfficiencySFTool.isUserConfigured() && !isData()) {
    m_photonEfficiencySFTool.setTypeAndName("AsgPhotonEfficiencyCorrectionTool/AsgPhotonEfficiencyCorrectionTool_" + m_photonId);

    if (m_photonId != "Tight" ) {
      ATH_MSG_WARNING( "No Photon efficiency available for " << m_photonId << ", using Tight instead..." );
    }

    ATH_CHECK( m_photonEfficiencySFTool.setProperty("ForceDataType", 1) ); // Set data type: 1 for FULLSIM, 3 for AF2
    ATH_CHECK( m_photonEfficiencySFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_photonEfficiencySFTool.retrieve() );
  }

  if (!m_photonIsolationSFTool.isUserConfigured() && !isData()) {
    m_photonIsolationSFTool.setTypeAndName("AsgPhotonEfficiencyCorrectionTool/AsgPhotonEfficiencyCorrectionTool_isol" + m_photonIso_WP);

    if (m_photonIso_WP != "FixedCutTight" && m_photonIso_WP != "FixedCutLoose" && m_photonIso_WP != "FixedCutTightCaloOnly") {
      ATH_MSG_WARNING( "No Photon efficiency available for " << m_photonIso_WP);
    }

    ATH_CHECK( m_photonIsolationSFTool.setProperty("IsoKey", m_photonIso_WP.substr(8) ));    // Set isolation WP: Loose,Tight,TightCaloOnly
    ATH_CHECK( m_photonIsolationSFTool.setProperty("ForceDataType", 1) ); // Set data type: 1 for FULLSIM, 3 for AF2
    ATH_CHECK( m_photonIsolationSFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_photonIsolationSFTool.retrieve() );
  }

  // trigger scale factors 
  if (!m_photonTriggerSFTool.isUserConfigured() && !isData()) {
    m_photonTriggerSFTool.setTypeAndName("AsgPhotonEfficiencyCorrectionTool/AsgPhotonEfficiencyCorrectionTool_trig" + m_photonTriggerName);

    // Fallback to TightCaloOnly if Tight is selected
    std::string photonIso_forTrigSF = m_photonIso_WP;
    if (m_photonIso_WP == "FixedCutTight") {
      ATH_MSG_WARNING( "No Photon trigger SF available for " << m_photonIso_WP << ", using TightCaloOnly instead... Use at your own risk" );  
      photonIso_forTrigSF = "TightCaloOnly";
    } else { //  isolation WP supported: Loose or TightCaloOnly, removing "FixedCut" suffix..
      photonIso_forTrigSF = TString(m_photonIso_WP).ReplaceAll("FixedCut","").Data();
    } 

    // "symmetric" diphoton triggers (year dependent)
    ATH_CHECK( m_photonTriggerSFTool.setProperty("IsoKey", photonIso_forTrigSF ));    // Set isolation WP: Loose,TightCaloOnly 
    ATH_CHECK( m_photonTriggerSFTool.setProperty("TriggerKey", m_photonTriggerName ));    
    ATH_CHECK( m_photonTriggerSFTool.setProperty("ForceDataType", 1) ); // Set data type: 1 for FULLSIM, 3 for AF2
    ATH_CHECK( m_photonTriggerSFTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_photonTriggerSFTool.retrieve() );

    // "asymmetric" diphoton triggers
    std::map<std::string,std::string> diphotonTriggerSFMapAsymmetric {
      // legs, Trigger keys, 
      {"g25_loose,g25_medium_L1EM20VH", "DI_PH_2015_g25_loose_2016_g25_loose_2017_g25_medium_L1EM20VH_2018_g25_medium_L1EM20VH"},
      {"g35_loose,g35_medium_L1EM20VH", "DI_PH_2015_g35_loose_2016_g35_loose_2017_g35_medium_L1EM20VH_2018_g35_medium_L1EM20VH"},
    };

    for(auto const& item : diphotonTriggerSFMapAsymmetric){

      toolName = "AsgPhotonEfficiencyCorrectionTool_trigSF_asymm_diphoton_" + (item.first).substr(0,9) + photonIso_forTrigSF;
      auto ph_trigSF = m_photonEfficiencySFTool_trigSF_AsymDiphoton.emplace(m_photonEfficiencySFTool_trigSF_AsymDiphoton.end(), "AsgPhotonEfficiencyCorrectionTool/"+toolName);
      ATH_CHECK( ph_trigSF->setProperty("IsoKey", photonIso_forTrigSF) );
      ATH_CHECK( ph_trigSF->setProperty("TriggerKey", item.second) );
      ATH_CHECK( ph_trigSF->setProperty("ForceDataType", 1) ); // Set DataType: 1 for FullSim and 3 for AFII
      ATH_CHECK( ph_trigSF->setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( ph_trigSF->initialize() );
      m_photonTrigSFTools.push_back(ph_trigSF->getHandle());
#ifndef XAOD_STANDALONE
      m_legsPerTool_ph[toolName] = item.first;
#else
      m_legsPerTool_ph["ToolSvc."+toolName] = item.first;
#endif

      toolName = "AsgPhotonEfficiencyCorrectionTool_trigEff_asymm_diphoton_" + (item.first).substr(0,9) + photonIso_forTrigSF;
      auto ph_trigEff = m_photonEfficiencySFTool_trigEff_AsymDiphoton.emplace(m_photonEfficiencySFTool_trigEff_AsymDiphoton.end(), "AsgPhotonEfficiencyCorrectionTool/"+toolName);
      ATH_CHECK( ph_trigEff->setProperty("IsoKey", photonIso_forTrigSF) );
      ATH_CHECK( ph_trigEff->setProperty("TriggerKey", "Eff_"+item.second) );
      ATH_CHECK( ph_trigEff->setProperty("ForceDataType", 1) ); // Set DataType: 1 for FullSim and 3 for AFII
      ATH_CHECK( ph_trigEff->setProperty("OutputLevel", this->msg().level()) );
      ATH_CHECK( ph_trigEff->initialize() );
      m_photonTrigEffTools.push_back(ph_trigEff->getHandle());
#ifndef XAOD_STANDALONE
      m_legsPerTool_ph[toolName] = item.first;
#else
      m_legsPerTool_ph["ToolSvc."+toolName] = item.first;
#endif

    }
  }

 ///////////////////////////////////////////////////////////////////////////////////////////
 // Initialize the MC fudge tool

  if (!m_electronPhotonShowerShapeFudgeTool.isUserConfigured()) {
    m_electronPhotonShowerShapeFudgeTool.setTypeAndName("ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool");

    int FFset = 22;
    ATH_CHECK( m_electronPhotonShowerShapeFudgeTool.setProperty("Preselection", FFset) );
    ATH_CHECK( m_electronPhotonShowerShapeFudgeTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_electronPhotonShowerShapeFudgeTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialize the EgammaAmbiguityTool

  if (!m_egammaAmbiguityTool.isUserConfigured()) {
    m_egammaAmbiguityTool.setTypeAndName("EGammaAmbiguityTool/EGammaAmbiguityTool");
    ATH_CHECK( m_egammaAmbiguityTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_egammaAmbiguityTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialize the AsgElectronChargeIDSelector

  if (!m_elecChargeIDSelectorTool.isUserConfigured()) {

    // For the selector, can use the nice function
    std::string eleId = EG_WP(m_eleId);
    m_elecChargeIDSelectorTool.setTypeAndName("AsgElectronChargeIDSelectorTool/ElectronChargeIDSelectorTool_"+eleId);
    //default cut value for https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronChargeFlipTaggerTool
    float BDTcut = -0.337671; // Loose 97%
    if (m_eleChID_WP != "Loose" && !m_eleChID_WP.empty()) {
      ATH_MSG_ERROR("Only Loose WP is supported in R21. Invalid ChargeIDSelector WP selected : " << m_eleChID_WP);
      return StatusCode::FAILURE;
    } 

    ATH_CHECK( m_elecChargeIDSelectorTool.setProperty("TrainingFile", "ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root"));
    ATH_CHECK( m_elecChargeIDSelectorTool.setProperty("CutOnBDT", BDTcut));
    ATH_CHECK( m_elecChargeIDSelectorTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_elecChargeIDSelectorTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise egamma calibration tool

  if (!m_egammaCalibTool.isUserConfigured()) {
    m_egammaCalibTool.setTypeAndName("CP::EgammaCalibrationAndSmearingTool/EgammaCalibrationAndSmearingTool");
    ATH_MSG_DEBUG( "Initialising EgcalibTool " );
    ATH_CHECK( m_egammaCalibTool.setProperty("ESModel", "es2017_R21_v1") ); //used for analysis using data processed with 21.0
    ATH_CHECK( m_egammaCalibTool.setProperty("decorrelationModel", "1NP_v1") );
    ATH_CHECK( m_egammaCalibTool.setProperty("useAFII", isAtlfast()?1:0) );
    ATH_CHECK( m_egammaCalibTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_egammaCalibTool.retrieve() );
  }


///////////////////////////////////////////////////////////////////////////////////////////
// No tau score re-decorator in R21; might come back some day, would go here

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau selection tools

  if (!m_tauSelTool.isUserConfigured()) {
    std::string inputfile = "";
    if (!m_tauConfigPath.empty() && (m_tauConfigPath!="default")) inputfile = m_tauConfigPath;
    else if (m_tauId == "VeryLoose") inputfile = "SUSYTools/tau_selection_veryloose.conf";
    else if (m_tauId == "Loose") inputfile = "SUSYTools/tau_selection_loose.conf";
    else if (m_tauId == "Medium") inputfile = "SUSYTools/tau_selection_medium.conf";
    else if (m_tauId == "Tight") inputfile = "SUSYTools/tau_selection_tight.conf";
    else {
      ATH_MSG_ERROR("Invalid tau ID selected: " << m_tauId);
      return StatusCode::FAILURE;
    }
    toolName = "TauSelectionTool_" + m_tauId;
    m_tauSelTool.setTypeAndName("TauAnalysisTools::TauSelectionTool/"+toolName);
    ATH_CHECK( m_tauSelTool.setProperty("ConfigPath", inputfile) );

    ATH_CHECK( m_tauSelTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_tauSelTool.retrieve() );
  }

  if (!m_tauSelToolBaseline.isUserConfigured()) {
    std::string inputfile = "";
    if (!m_tauConfigPathBaseline.empty() && (m_tauConfigPathBaseline!="default")) inputfile = m_tauConfigPathBaseline;
    else if (m_tauIdBaseline == "VeryLoose") inputfile = "SUSYTools/tau_selection_veryloose.conf";
    else if (m_tauIdBaseline == "Loose") inputfile = "SUSYTools/tau_selection_loose.conf";
    else if (m_tauIdBaseline == "Medium") inputfile = "SUSYTools/tau_selection_medium.conf";
    else if (m_tauIdBaseline == "Tight") inputfile = "SUSYTools/tau_selection_tight.conf";
    else {
      ATH_MSG_ERROR("Invalid baseline tau ID selected: " << m_tauIdBaseline);
      return StatusCode::FAILURE;
    }
    toolName = "TauSelectionToolBaseline_" + m_tauIdBaseline;
    m_tauSelToolBaseline.setTypeAndName("TauAnalysisTools::TauSelectionTool/"+toolName);
    ATH_CHECK( m_tauSelToolBaseline.setProperty("ConfigPath", inputfile) );

    ATH_CHECK( m_tauSelToolBaseline.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_tauSelToolBaseline.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau efficiency tool

  if (!m_tauEffTool.isUserConfigured()) {
    toolName = "TauEffTool_" + m_tauId;
    m_tauEffTool.setTypeAndName("TauAnalysisTools::TauEfficiencyCorrectionsTool/"+toolName);
    ATH_CHECK( m_tauEffTool.setProperty("PileupReweightingTool",m_prwTool.getHandle()) );

    if (!m_tauSelTool.empty()) {
      ATH_CHECK( m_tauEffTool.setProperty("TauSelectionTool", m_tauSelTool.getHandle()) );
    }
    ATH_CHECK( m_tauEffTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_tauEffTool.retrieve() );
  }

  // TODO: add SF tool for baseline tau id as well? /CO

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau trigger efficiency tool(s)

  int iTauID = (int) TauAnalysisTools::JETIDNONEUNCONFIGURED;
  if (m_tauId == "VeryLoose")   iTauID = (int) TauAnalysisTools::JETIDBDTVERYLOOSE;
  else if (m_tauId == "Loose")  iTauID = (int) TauAnalysisTools::JETIDBDTLOOSE;
  else if (m_tauId == "Medium") iTauID = (int) TauAnalysisTools::JETIDBDTMEDIUM;
  else if (m_tauId == "Tight")  iTauID = (int) TauAnalysisTools::JETIDBDTTIGHT;
  else {
    ATH_MSG_ERROR("Invalid tau ID selected: " << m_tauId); // we shouldn't get here but ok...
    return StatusCode::FAILURE;
  }

  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool0, 0, m_tau_trig_support[0], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool1, 1, m_tau_trig_support[1], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool2, 2, m_tau_trig_support[2], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool3, 3, m_tau_trig_support[3], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool4, 4, m_tau_trig_support[4], iTauID);

  ///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau smearing tool

  if (!m_tauSmearingTool.isUserConfigured()) {
    m_tauSmearingTool.setTypeAndName("TauAnalysisTools::TauSmearingTool/TauSmearingTool");
    ATH_MSG_INFO("'TauMVACalibration' is the default procedure in R21");
    ATH_CHECK( m_tauSmearingTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_tauSmearingTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau truth matching tool

  if (!m_tauTruthMatch.isUserConfigured() && m_tauDoTTM) {
    m_tauTruthMatch.setTypeAndName("TauAnalysisTools::TauTruthMatchingTool/TauTruthMatch");
    ATH_CHECK( m_tauTruthMatch.setProperty("WriteTruthTaus", true) );
    ATH_CHECK( m_tauTruthMatch.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_tauTruthMatch.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise TauOverlappingElectronLLHDecorator tool

  if (!m_tauElORdecorator.isUserConfigured()) {
    m_tauElORdecorator.setTypeAndName("TauAnalysisTools::TauOverlappingElectronLLHDecorator/TauEleORDecorator");
    ATH_CHECK( m_tauElORdecorator.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_tauElORdecorator.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise B-tagging tools

  // btagSelectionTool
  std::string jetcollBTag = jetcoll;
  if (jetcoll == "AntiKt4LCTopoJets") {
    ATH_MSG_WARNING("  *** HACK *** Treating LCTopoJets jets as EMTopo -- use at your own risk!");
    jetcollBTag = "AntiKt4EMTopoJets";
  }

  if (!m_btagSelTool.isUserConfigured() && !m_BtagWP.empty()) {
    if (jetcoll != "AntiKt4EMTopoJets" && jetcoll != "AntiKt4EMPFlowJets") {
      ATH_MSG_WARNING("** Only AntiKt4EMTopoJets and AntiKt4EMPFlowJets are supported with FTAG scale factors!");
        return StatusCode::FAILURE;
    }

    toolName = "BTagSel_" + jetcollBTag + m_BtagTagger + m_BtagWP;
    m_btagSelTool.setTypeAndName("BTaggingSelectionTool/"+toolName);
    ATH_CHECK( m_btagSelTool.setProperty("TaggerName",     m_BtagTagger ) );
    ATH_CHECK( m_btagSelTool.setProperty("OperatingPoint", m_BtagWP  ) );
    ATH_CHECK( m_btagSelTool.setProperty("JetAuthor",      jetcollBTag   ) );
    ATH_CHECK( m_btagSelTool.setProperty("FlvTagCutDefinitionsFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagSelTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_btagSelTool.retrieve() );
  }

  if (!m_btagSelTool_OR.isUserConfigured() && !m_orBtagWP.empty()) {
    if (jetcoll != "AntiKt4EMTopoJets" && jetcoll != "AntiKt4EMPFlowJets") {
      ATH_MSG_WARNING("** Only AntiKt4EMTopoJets and AntiKt4EMPFlowJets are supported with FTAG scale factors!");
        return StatusCode::FAILURE;
    }

    toolName = "BTagSelOR_" + jetcollBTag + m_orBtagWP;
    m_btagSelTool_OR.setTypeAndName("BTaggingSelectionTool/"+toolName);
    ATH_CHECK( m_btagSelTool_OR.setProperty("TaggerName",     m_BtagTagger  ) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("OperatingPoint", m_orBtagWP  ) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("JetAuthor",      jetcollBTag   ) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("FlvTagCutDefinitionsFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_btagSelTool_OR.retrieve() );
  }

  std::string trkjetcoll = m_defaultTrackJets;
  if (!m_btagSelTool_trkJet.isUserConfigured() && !m_BtagWP_trkJet.empty()) {
    if (trkjetcoll != "AntiKt2PV0TrackJets" && trkjetcoll != "AntiKtVR30Rmax4Rmin02TrackJets") {
      ATH_MSG_WARNING("** Only AntiKt2PV0TrackJets and AntiKtVR30Rmax4Rmin02TrackJets are supported with FTAG scale factors!");
        return StatusCode::FAILURE;
    }

    toolName = "BTagSel_" + trkjetcoll + m_BtagWP_trkJet;
    m_btagSelTool_trkJet.setTypeAndName("BTaggingSelectionTool/"+toolName);
    ATH_CHECK( m_btagSelTool_trkJet.setProperty("TaggerName",     m_BtagTagger_trkJet ) );
    ATH_CHECK( m_btagSelTool_trkJet.setProperty("OperatingPoint", m_BtagWP_trkJet  ) );
    ATH_CHECK( m_btagSelTool_trkJet.setProperty("MinPt",          m_BtagMinPt_trkJet  ) );
    ATH_CHECK( m_btagSelTool_trkJet.setProperty("JetAuthor",      trkjetcoll   ) );
    ATH_CHECK( m_btagSelTool_trkJet.setProperty("FlvTagCutDefinitionsFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagSelTool_trkJet.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_btagSelTool_trkJet.retrieve() );
  }

  // Set MCshowerType for FTAG MC/MC SFs
  std::string MCshowerID = "410470";                 // Powheg+Pythia8 (default)
  if (m_showerType == 1) MCshowerID = "410558";      // Powheg+Herwig7
  else if (m_showerType == 2) MCshowerID = "426131"; // Sherpa 2.1
  else if (m_showerType == 3) MCshowerID = "410250"; // Sherpa 2.2

  // btagEfficiencyTool
  if (!m_btagEffTool.isUserConfigured() && !m_BtagWP.empty()) {
    if (jetcoll != "AntiKt4EMTopoJets" && jetcoll != "AntiKt4EMPFlowJets") {
      ATH_MSG_WARNING("** Only AntiKt4EMTopoJets and AntiKt4EMPFlowJets are supported with FTAG scale factors!");
        return StatusCode::FAILURE;
    }

    // AntiKt4EMPFlowJets MC/MC SF isn't complete yet
    if (jetcollBTag == "AntiKt4EMPFlowJets" && MCshowerID == "426131") { // sherpa 2.1 isn't available
      ATH_MSG_WARNING ("MC/MC SFs for AntiKt4EMPFlowJets are not available yet! Falling back to AntiKt4EMTopoJets for the SFs.");
      jetcollBTag = "AntiKt4EMTopoJets";
    }

    toolName = "BTagSF_" + jetcollBTag + m_BtagTagger + m_BtagWP;
    m_btagEffTool.setTypeAndName("BTaggingEfficiencyTool/"+toolName);
    ATH_CHECK( m_btagEffTool.setProperty("TaggerName",     m_BtagTagger ) );
    ATH_CHECK( m_btagEffTool.setProperty("ScaleFactorFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagEffTool.setProperty("OperatingPoint", m_BtagWP ) );
    ATH_CHECK( m_btagEffTool.setProperty("JetAuthor",      jetcollBTag ) );
    ATH_CHECK( m_btagEffTool.setProperty("SystematicsStrategy", m_BtagSystStrategy ) );
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyBCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyCCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyTCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyLightCalibrations", MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_btagEffTool.retrieve() );
  }

  if (!m_btagEffTool_trkJet.isUserConfigured() && !m_BtagWP_trkJet.empty()) {
    if (trkjetcoll != "AntiKt2PV0TrackJets" && trkjetcoll != "AntiKtVR30Rmax4Rmin02TrackJets") {
      ATH_MSG_WARNING("** Only AntiKt2PV0TrackJets and AntiKtVR30Rmax4Rmin02TrackJets are supported with FTAG scale factors!");
        return StatusCode::FAILURE;
    }

    toolName = "BTagSF_" + trkjetcoll;
    m_btagEffTool_trkJet.setTypeAndName("BTaggingEfficiencyTool/"+toolName);
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("TaggerName",     m_BtagTagger_trkJet ) );
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("ScaleFactorFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("OperatingPoint", m_BtagWP_trkJet ) );
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("JetAuthor",      trkjetcoll ) );
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("SystematicsStrategy", m_BtagSystStrategy ) );
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("EfficiencyBCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("EfficiencyCCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("EfficiencyTCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("EfficiencyLightCalibrations", MCshowerID   ));
    ATH_CHECK( m_btagEffTool_trkJet.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_btagEffTool_trkJet.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise MET tools

  if (!m_metMaker.isUserConfigured()) {
    toolName = m_doFwdJVT ? m_metJetSelection+"_fJVT" : m_metJetSelection+"_NOfJVT";
    m_metMaker.setTypeAndName("met::METMaker/METMaker_ST_"+toolName);

    ATH_CHECK( m_metMaker.setProperty("ORCaloTaggedMuons", m_metRemoveOverlappingCaloTaggedMuons) );
    ATH_CHECK( m_metMaker.setProperty("DoSetMuonJetEMScale", m_metDoSetMuonJetEMScale) );
    ATH_CHECK( m_metMaker.setProperty("DoRemoveMuonJets", m_metDoRemoveMuonJets) );
    ATH_CHECK( m_metMaker.setProperty("UseGhostMuons", m_metUseGhostMuons) );
    ATH_CHECK( m_metMaker.setProperty("DoMuonEloss", m_metDoMuonEloss) );
    ATH_CHECK( m_metMaker.setProperty("GreedyPhotons", m_metGreedyPhotons) );
    ATH_CHECK( m_metMaker.setProperty("VeryGreedyPhotons", m_metVeryGreedyPhotons) );

    // set the jet selection if default empty string is overridden through config file
    if (m_metJetSelection.size()) {
      ATH_CHECK( m_metMaker.setProperty("JetSelection", m_metJetSelection) );
    }
    if (m_doFwdJVT || m_metJetSelection == "Tenacious" || m_metJetSelection == "TenaciousJVT641") {
      ATH_CHECK( m_metMaker.setProperty("JetRejectionDec", "passFJvt") );
    }
    if (m_jetInputType == xAOD::JetInput::EMPFlow) {
      ATH_CHECK( m_metMaker.setProperty("DoPFlow", true) );
    }

    ATH_CHECK( m_metMaker.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_metMaker.retrieve() );
  }

  if (!m_metSystTool.isUserConfigured()) {
    m_metSystTool.setTypeAndName("met::METSystematicsTool/METSystTool");
    ATH_CHECK( m_metSystTool.setProperty("ConfigPrefix", m_metsysConfigPrefix) );

    if (m_trkMETsyst && m_caloMETsyst){
      ATH_MSG_ERROR( "Can only have CST *or* TST configured for MET maker.  Please unset either METDoCaloSyst or METDoTrkSyst in your config file" );
      return StatusCode::FAILURE;
    }

    if (m_trkMETsyst) {
      ATH_CHECK( m_metSystTool.setProperty("ConfigSoftCaloFile", "") );
      ATH_CHECK( m_metSystTool.setProperty("ConfigSoftTrkFile", "TrackSoftTerms.config") );
      if (m_jetInputType == xAOD::JetInput::EMPFlow) {
        ATH_CHECK( m_metSystTool.setProperty("ConfigSoftTrkFile", "TrackSoftTerms-pflow.config") );
      }
      if (isAtlfast()) {
        ATH_CHECK( m_metSystTool.setProperty("ConfigSoftTrkFile", "TrackSoftTerms_AFII.config") );
      }
    }

    if (m_caloMETsyst) {
      ATH_MSG_WARNING( "CST is no longer recommended by Jet/MET group");
      ATH_CHECK( m_metSystTool.setProperty("ConfigSoftTrkFile", "") );
      // Recommendations from a thread with TJ.  CST is not officially supported, but might be used for cross-checks
      ATH_CHECK( m_metSystTool.setProperty("DoIsolMuonEloss",true) );
      ATH_CHECK( m_metSystTool.setProperty("DoMuonEloss",true) );
      if ("AntiKt4EMTopoJets"==jetcoll) {
        // Recommendation from TJ: if we are using EM topo jets, make sure the clusters are considered at LC scale
        ATH_CHECK( m_metSystTool.setProperty("JetConstitScaleMom","JetLCScaleMomentum") );
      }
    }

    if (m_trkJetsyst) {
      ATH_CHECK( m_metSystTool.setProperty("ConfigJetTrkFile", "JetTrackSyst.config") );
    }

    ATH_CHECK( m_metSystTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_metSystTool.retrieve());
  }

  if (!m_metSignif.isUserConfigured()) {
    // See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MetSignificance 
    m_metSignif.setTypeAndName("met::METSignificance/metSignificance_"+jetname);
    ATH_CHECK( m_metSignif.setProperty("SoftTermParam", m_softTermParam) );
    ATH_CHECK( m_metSignif.setProperty("TreatPUJets", m_treatPUJets) );
    ATH_CHECK( m_metSignif.setProperty("DoPhiReso", m_doPhiReso) );
    ATH_CHECK( m_metSignif.setProperty("IsAFII", isAtlfast()) ); 
    if(jetname == "AntiKt4EMTopo" || jetname =="AntiKt4EMPFlow"){
      ATH_CHECK( m_metSignif.setProperty("JetCollection", jetname) );
    } else {
      ATH_MSG_WARNING("Object-based METSignificance recommendations only exist for EMTopo and PFlow, falling back to AntiKt4EMTopo");
      ATH_CHECK( m_metSignif.setProperty("JetCollection", "AntiKt4EMTopo") );
    }
    ATH_CHECK( m_metSignif.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_metSignif.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise trigger tools

  if (!m_trigDecTool.isUserConfigured()) {
    m_trigConfTool.setTypeAndName("TrigConf::xAODConfigTool/xAODConfigTool");
    ATH_CHECK(m_trigConfTool.retrieve() );

    // The decision tool
    m_trigDecTool.setTypeAndName("Trig::TrigDecisionTool/TrigDecisionTool");
    ATH_CHECK( m_trigDecTool.setProperty("ConfigTool", m_trigConfTool.getHandle()) );
    ATH_CHECK( m_trigDecTool.setProperty("TrigDecisionKey", "xTrigDecision") );
    ATH_CHECK( m_trigDecTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_trigDecTool.retrieve() );
  }

  if (!m_trigMatchingTool.isUserConfigured()) {
    m_trigMatchingTool.setTypeAndName("Trig::MatchingTool/TrigMatchingTool");
    ATH_CHECK( m_trigMatchingTool.setProperty("TrigDecisionTool", m_trigDecTool.getHandle()) );
    ATH_CHECK( m_trigMatchingTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_trigMatchingTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise trigGlobalEfficiencyCorrection tool

  if (!m_trigGlobalEffCorrTool_diLep.isUserConfigured()) {
    m_trigGlobalEffCorrTool_diLep.setTypeAndName("TrigGlobalEfficiencyCorrectionTool/TrigGlobal_diLep");
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("ElectronEfficiencyTools", m_elecTrigEffTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("ElectronScaleFactorTools", m_elecTrigSFTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("MuonTools", m_muonTrigSFTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("TriggerCombination2015", m_trig2015combination_diLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("TriggerCombination2016", m_trig2016combination_diLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("TriggerCombination2017", m_trig2017combination_diLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("TriggerCombination2018", m_trig2018combination_diLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("TriggerMatchingTool", m_trigMatchingTool.getHandle()) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("ListOfLegsPerTool", m_legsPerTool) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("NumberOfToys", 250) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diLep.initialize() );
  }

  if (!m_trigGlobalEffCorrTool_multiLep.isUserConfigured()) {
    m_trigGlobalEffCorrTool_multiLep.setTypeAndName("TrigGlobalEfficiencyCorrectionTool/TrigGlobal_multiLep");
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("ElectronEfficiencyTools", m_elecTrigEffTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("ElectronScaleFactorTools", m_elecTrigSFTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("MuonTools", m_muonTrigSFTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("TriggerCombination2015", m_trig2015combination_multiLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("TriggerCombination2016", m_trig2016combination_multiLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("TriggerCombination2017", m_trig2017combination_multiLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("TriggerCombination2018", m_trig2018combination_multiLep) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("TriggerMatchingTool", m_trigMatchingTool.getHandle()) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("ListOfLegsPerTool", m_legsPerTool) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("NumberOfToys", 250) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_trigGlobalEffCorrTool_multiLep.initialize() );
  }

  if (!m_trigGlobalEffCorrTool_diPhoton.isUserConfigured()) {
    m_trigGlobalEffCorrTool_diPhoton.setTypeAndName("TrigGlobalEfficiencyCorrectionTool/TrigGlobal_diPhoton");
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("PhotonEfficiencyTools", m_photonTrigEffTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("PhotonScaleFactorTools", m_photonTrigSFTools) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("TriggerCombination2015", m_trig2015combination_diPhoton) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("TriggerCombination2016", m_trig2016combination_diPhoton) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("TriggerCombination2017", m_trig2017combination_diPhoton) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("TriggerCombination2018", m_trig2018combination_diPhoton) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("ListOfLegsPerTool", m_legsPerTool_ph) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("NumberOfToys", 250) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_trigGlobalEffCorrTool_diPhoton.initialize() );
  }

// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise Isolation Correction Tool

  if ( !m_isoCorrTool.isUserConfigured() ) {
    m_isoCorrTool.setTypeAndName("CP::IsolationCorrectionTool/IsoCorrTool");
    ATH_CHECK( m_isoCorrTool.setProperty( "IsMC", !isData()) );
    ATH_CHECK( m_isoCorrTool.setProperty( "AFII_corr", isAtlfast()) );
    ATH_CHECK( m_isoCorrTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_isoCorrTool.retrieve() );
  }

// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise Isolation Tool
  if (!m_isoTool.isUserConfigured()) {
    m_isoTool.setTypeAndName("CP::IsolationSelectionTool/IsoTool");
    ATH_CHECK( m_isoTool.setProperty("ElectronWP", m_eleIso_WP) );
    ATH_CHECK( m_isoTool.setProperty("MuonWP",     m_muIso_WP ) );
    ATH_CHECK( m_isoTool.setProperty("PhotonWP",   m_photonIso_WP ) );
    ATH_CHECK( m_isoTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_isoTool.retrieve() );
  }

  if (!m_isoBaselineTool.isUserConfigured()) {
    m_isoBaselineTool.setTypeAndName("CP::IsolationSelectionTool/IsoBaselineTool");
    ATH_CHECK( m_isoBaselineTool.setProperty("ElectronWP", m_eleBaselineIso_WP.empty()    ? "Gradient" : m_eleBaselineIso_WP    ) );
    ATH_CHECK( m_isoBaselineTool.setProperty("MuonWP",     m_muBaselineIso_WP.empty()     ? "FCLoose" : m_muBaselineIso_WP     ) );
    ATH_CHECK( m_isoBaselineTool.setProperty("PhotonWP",   m_photonBaselineIso_WP.empty() ? "FixedCutTight" : m_photonBaselineIso_WP ) );
    ATH_CHECK( m_isoBaselineTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_isoBaselineTool.retrieve() );
  }

  if (!m_isoHighPtTool.isUserConfigured()) {
    m_isoHighPtTool.setTypeAndName("CP::IsolationSelectionTool/IsoHighPtTool");
    ATH_CHECK( m_isoHighPtTool.setProperty("ElectronWP", m_eleIsoHighPt_WP) );
    ATH_CHECK( m_isoHighPtTool.setProperty("MuonWP",     m_muIsoHighPt_WP ) );
    ATH_CHECK( m_isoHighPtTool.setProperty("PhotonWP",   m_photonIso_WP ) );
    ATH_CHECK( m_isoHighPtTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_isoHighPtTool.retrieve() );
  }

// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise IsolationCloseByCorrectionTool Tool
  if (!m_isoCloseByTool.isUserConfigured()) {
    m_isoCloseByTool.setTypeAndName("CP::IsolationCloseByCorrectionTool/IsoCloseByTool");
    // Actually we could debate about what is the proper tool to choose if the users have different baseline & signal islation WP's
    ATH_CHECK( m_isoCloseByTool.setProperty("IsolationSelectionTool", m_useSigLepForIsoCloseByOR ? m_isoTool : m_isoBaselineTool));
    ATH_CHECK( m_isoCloseByTool.setProperty("PassoverlapDecorator", m_IsoCloseByORpassLabel) );
    ATH_CHECK( m_isoCloseByTool.setProperty("SelectionDecorator", m_useSigLepForIsoCloseByOR ? "signal" : "baseline") );
    // Make this propery configurable as well?
    ATH_CHECK( m_isoCloseByTool.setProperty("BackupPrefix", "ORIG") );
    // The isolation selection decorator is updated as well by the tool
    ATH_CHECK( m_isoCloseByTool.setProperty("IsolationSelectionDecorator", "isol") );

    ATH_CHECK( m_isoCloseByTool.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_isoCloseByTool.retrieve() );
  }

// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise Overlap Removal Tool
  if ( m_orToolbox.masterTool.empty() ){

    // set up the master tool
    std::string suffix = "";
    if (m_orDoTau) suffix += "Tau";
    if (m_orDoPhoton) suffix += "Gamma";
    if (m_orDoBjet) suffix += "Bjet";
    std::string toolName = "ORTool" + suffix;
    ATH_MSG_INFO("SUSYTools: Autoconfiguring " << toolName);

    std::string bJetLabel = "";
    //overwrite lepton flags if the global is false (yes?)
    if (!m_orDoBjet || !m_useBtagging) {
      m_orDoElBjet = false;
      m_orDoMuBjet = false;
      m_orDoTauBjet = false;
    }
    if (m_orDoElBjet || m_orDoMuBjet || m_orDoTauBjet) {
      bJetLabel = "bjet_loose";
    }

    // Set the generic flags
    ORUtils::ORFlags orFlags(toolName, m_orInputLabel, "passOR");
    orFlags.bJetLabel      = bJetLabel;
    orFlags.boostedLeptons = (m_orDoBoostedElectron || m_orDoBoostedMuon);
    orFlags.outputPassValue = true;
    orFlags.linkOverlapObjects = m_orLinkOverlapObjects;
    orFlags.doEleEleOR = false;
    orFlags.doElectrons = true;
    orFlags.doMuons = true;
    orFlags.doJets = true;
    orFlags.doTaus = m_orDoTau;
    orFlags.doPhotons = m_orDoPhoton;
    orFlags.doFatJets = m_orDoFatjets;

    //set up all recommended tools
    ATH_CHECK( ORUtils::recommendedTools(orFlags, m_orToolbox));

    // We don't currently have a good way to determine here which object
    // definitions are disabled, so we currently just configure all overlap
    // tools and disable the pointer safety checks
    ATH_CHECK( m_orToolbox.masterTool.setProperty("RequireExpectedPointers", false) );

    // Override boosted OR sliding cone options
    ATH_CHECK( m_orToolbox.eleJetORT.setProperty("UseSlidingDR", m_orDoBoostedElectron) );
    ATH_CHECK( m_orToolbox.muJetORT.setProperty("UseSlidingDR", m_orDoBoostedMuon) );

    //add custom tau-jet OR tool
    if(m_orDoTau){
      m_orToolbox.tauJetORT.setTypeAndName("ORUtils::TauJetOverlapTool/" + orFlags.masterName + ".TauJetORT");
      ATH_CHECK( m_orToolbox.tauJetORT.setProperty("BJetLabel", m_orDoTauBjet?bJetLabel:"") );
    }

    // override sliding cone params if sliding dR is on and the user-provided parameter values are non-negative
    if (m_orDoBoostedElectron) {
      if (m_orBoostedElectronC1 > 0) ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRC1", m_orBoostedElectronC1) );
      if (m_orBoostedElectronC2 > 0) ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRC2", m_orBoostedElectronC2) );
      if (m_orBoostedElectronMaxConeSize > 0) ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRMaxCone", m_orBoostedElectronMaxConeSize) );
    }
    if (m_orDoBoostedMuon) {
      if (m_orBoostedMuonC1 > 0) ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRC1", m_orBoostedMuonC1) );
      if (m_orBoostedMuonC2 > 0) ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRC2", m_orBoostedMuonC2) );
      if (m_orBoostedMuonMaxConeSize > 0) ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRMaxCone", m_orBoostedMuonMaxConeSize) );
    }

    // and switch off lep-bjet check if not requested
    if (!m_orDoElBjet) {
      ATH_CHECK(m_orToolbox.eleJetORT.setProperty("BJetLabel", ""));
    }
    if (!m_orDoMuBjet) {
      ATH_CHECK(m_orToolbox.muJetORT.setProperty("BJetLabel", ""));
    }

    // propagate the mu-jet ghost-association option which might be set by the user (default is true)
    ATH_CHECK(m_orToolbox.muJetORT.setProperty("UseGhostAssociation", m_orDoMuonJetGhostAssociation));

    // propagate mu-jet OR settings if requested
    ATH_CHECK(m_orToolbox.muJetORT.setProperty("ApplyRelPt", m_orApplyRelPt) );
    if(m_orApplyRelPt){
      if (m_orMuJetPtRatio > 0)    ATH_CHECK(m_orToolbox.muJetORT.setProperty("MuJetPtRatio", m_orMuJetPtRatio) );
      if (m_orMuJetTrkPtRatio > 0) ATH_CHECK(m_orToolbox.muJetORT.setProperty("MuJetTrkPtRatio", m_orMuJetTrkPtRatio) );
    }
    if (m_orMuJetInnerDR > 0)    ATH_CHECK(m_orToolbox.muJetORT.setProperty("InnerDR", m_orMuJetInnerDR) );

    // propagate the calo muon setting for EleMuORT
    ATH_CHECK(m_orToolbox.eleMuORT.setProperty("RemoveCaloMuons", m_orRemoveCaloMuons) );

    // propagate the fatjets OR settings
    if(m_orDoFatjets){
      if(m_EleFatJetDR>0) ATH_CHECK(m_orToolbox.eleFatJetORT.setProperty("DR", m_EleFatJetDR));
      if(m_JetFatJetDR>0) ATH_CHECK(m_orToolbox.jetFatJetORT.setProperty("DR", m_JetFatJetDR));
    }

    // Make sure that we deal with prorities correctly
    ATH_CHECK(m_orToolbox.eleJetORT.setProperty("EnableUserPriority", true));
    ATH_CHECK(m_orToolbox.muJetORT.setProperty("EnableUserPriority", true));
    if (m_orDoTau) ATH_CHECK(m_orToolbox.tauJetORT.setProperty("EnableUserPriority", true));
    if (m_orDoPhoton) ATH_CHECK(m_orToolbox.phoJetORT.setProperty("EnableUserPriority", true));

    if (!m_orDoEleJet){
      // Disable the electron removal part of e-j overlap removal
      ATH_CHECK( m_orToolbox.eleJetORT.setProperty("OuterDR",-1.) );
      ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRMaxCone",-1.) );
    }
    if (!m_orDoMuonJet){
      // Disable the muon removal part of m-j overlap removal
      ATH_CHECK( m_orToolbox.muJetORT.setProperty("OuterDR",-1.) );
      ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRMaxCone",-1.) );
    }

    ATH_CHECK( m_orToolbox.initialize() );

  } // Done with the OR toolbox setup!

// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise PMG Tools
  if (!m_pmgSHnjetWeighter.isUserConfigured()) {
    m_pmgSHnjetWeighter.setTypeAndName("PMGTools::PMGSherpa22VJetsWeightTool/PMGSHVjetReweighter");
    ATH_CHECK( m_pmgSHnjetWeighter.setProperty( "TruthJetContainer", "AntiKt4TruthJets"));
    ATH_CHECK( m_pmgSHnjetWeighter.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_pmgSHnjetWeighter.retrieve());
  }

  if (!m_pmgSHnjetWeighterWZ.isUserConfigured()) {
    m_pmgSHnjetWeighterWZ.setTypeAndName("PMGTools::PMGSherpa22VJetsWeightTool/PMGSHVjetReweighterWZ");
    ATH_CHECK( m_pmgSHnjetWeighterWZ.setProperty( "TruthJetContainer", "AntiKt4TruthWZJets"));
    ATH_CHECK( m_pmgSHnjetWeighterWZ.setProperty("OutputLevel", this->msg().level()) );
    ATH_CHECK( m_pmgSHnjetWeighterWZ.retrieve() );
  }

  // prevent these initialiation snippets from being run again
  m_subtool_init = true;

  ATH_MSG_INFO("Done initialising SUSYTools");

  return StatusCode::SUCCESS;
}

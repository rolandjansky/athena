 /*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopJetMETCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Jet include(s):
#include "JetCalibTools/JetCalibrationTool.h"
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetForwardJvtTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetJvtEfficiency/JetJvtEfficiency.h"
#include "JetSelectorTools/EventCleaningTool.h"

// MET include(s):
#include "METUtilities/METMaker.h"
#include "METUtilities/METSystematicsTool.h"

namespace top {

JetMETCPTools::JetMETCPTools(const std::string& name) :
    asg::AsgTool(name),

    m_jetJVT_ConfigFile("JVTlikelihood_20140805.root"),

    // Calibration strings for R21 only
    m_jetAntiKt4_Data_ConfigFile("JES_data2017_2016_2015_Consolidated_EMTopo_2018_Rel21.config"),
    m_jetAntiKt4_Data_CalibSequence("JetArea_Residual_EtaJES_GSC_Insitu"),

    m_jetAntiKt4_MCFS_ConfigFile("JES_data2017_2016_2015_Consolidated_EMTopo_2018_Rel21.config"),
    m_jetAntiKt4_MCFS_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear"),

    // Not recommended to use JER smearing (derived from FS on AF2)
    m_jetAntiKt4_MCAFII_ConfigFile("JES_MC16Recommendation_AFII_EMTopo_April2018_rel21.config"),
    m_jetAntiKt4_MCAFII_CalibSequence("JetArea_Residual_EtaJES_GSC"),
    
    // Not recommended to use JER smearing (derived from FS on AF2) 
    m_jetAntiKt4_MCAFII_PFlow_ConfigFile("JES_MC16Recommendation_AFII_PFlow_April2018_rel21.config"),
    m_jetAntiKt4_MCAFII_PFlow_CalibSequence("JetArea_Residual_EtaJES_GSC"),

    m_jetAntiKt4_PFlow_MCFS_ConfigFile("JES_data2017_2016_2015_Consolidated_PFlow_2018_Rel21.config"),
    m_jetAntiKt4_PFlow_MCFS_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear"),

    m_jetAntiKt4_Data_PFlow_ConfigFile("JES_data2017_2016_2015_Consolidated_PFlow_2018_Rel21.config"),
    m_jetAntiKt4_Data_PFlow_CalibSequence("JetArea_Residual_EtaJES_GSC_Insitu")
  {  

  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "JetCalibrationTool" , m_jetCalibrationTool );
  declareProperty( "JetCalibrationToolLargeR" , m_jetCalibrationToolLargeR );

  declareProperty( "JetUncertaintiesTool" , m_jetUncertaintiesTool);
  declareProperty( "JetUncertaintiesToolFrozenJMS" , m_jetUncertaintiesToolFrozenJMS);
  declareProperty( "JetUncertaintiesToolReducedNPScenario1" , m_jetUncertaintiesToolReducedNPScenario1 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario2" , m_jetUncertaintiesToolReducedNPScenario2 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario3" , m_jetUncertaintiesToolReducedNPScenario3 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario4" , m_jetUncertaintiesToolReducedNPScenario4 );

  declareProperty( "JetCleaningToolLooseBad" , m_jetCleaningToolLooseBad );
  declareProperty( "JetCleaningToolTightBad" , m_jetCleaningToolTightBad );

  declareProperty( "JetEventCleaningToolLooseBad" , m_jetEventCleaningToolLooseBad );
  declareProperty( "JetEventCleaningToolTightBad" , m_jetEventCleaningToolTightBad );

  declareProperty( "JetUpdateJvtTool" , m_jetUpdateJvtTool );

  declareProperty( "JES_data2016_data2015_Recommendation_Dec2016.config", m_jetAntiKt4_MCFS_ConfigFile);
  declareProperty( "JetCalibrationSequenceFS", m_jetAntiKt4_MCFS_CalibSequence );

  declareProperty( "JES_MC15cRecommendation_PFlow_Aug2016.config", m_jetAntiKt4_PFlow_MCFS_ConfigFile);
  declareProperty( "JetArea_Residual_EtaJES_GSC", m_jetAntiKt4_PFlow_MCFS_CalibSequence );

  declareProperty( "met_maker" , m_met_maker );
  declareProperty( "met_systematics" , m_met_systematics );
}

StatusCode JetMETCPTools::initialize() {
  ATH_MSG_INFO("top::JetMETCPTools initialize...");
  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::JetMETCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (m_config->useJets()) {
    if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
      top::check(setupJetsCalibration(), "Failed to setup jet calibration tools");
    }
    // JVT tool needed for both MC and data (for both selection and SFs)
    top::check(setupJetsScaleFactors(), "Failed to setup jet scale-factor tools");
  }

  if (m_config->useLargeRJets())
    top::check(setupLargeRJetsCalibration(), "Failed to setup large-R jet calibration");

  if (m_config->makeAllCPTools()) {// MET not needed on mini-xAODs
    top::check(setupMET(), "Failed to setup MET tools");
  }

  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupJetsCalibration() {

  // Release 21 specific
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetEtmissRecommendationsR21
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ApplyJetCalibrationR21

  // Get jet calibration name and erase "Jets" from the end
  std::string caloJets_collection = m_config->sgKeyJets();
  std::string jetCalibrationName;
  if (caloJets_collection == "AntiKt4EMTopoNoElJets"){
    jetCalibrationName="AntiKt4EMTopoJets";
  }
  else{
    jetCalibrationName = caloJets_collection ;
  }
  jetCalibrationName.erase(jetCalibrationName.length() - 4);

  // In case of using JMS calibration
  if ( m_config->jetCalibSequence() == "JMS" ) {
    ATH_MSG_ERROR("Unable to calibrate jets with JMS calib sequence in release 21: No recommendations! ");
    return StatusCode::FAILURE;

    // If this is functional, remove the error above and reconfigure the calibration sequence
    m_jetAntiKt4_MCFS_ConfigFile.erase(m_jetAntiKt4_MCFS_ConfigFile.length() - 7);//erase ".config" at the end
    m_jetAntiKt4_MCFS_ConfigFile += "_JMS.config";
    m_jetAntiKt4_MCFS_CalibSequence += "_JMS";
  }

  ///-- Calibration --///
  if (asg::ToolStore::contains<IJetCalibrationTool>("JetCalibrationTool")) {
    m_jetCalibrationTool = asg::ToolStore::get<IJetCalibrationTool>("JetCalibrationTool");
  } else {
    std::string calibConfig, calibSequence;
    if (m_config->isMC()) {
      // AFII
      if (m_config->isAFII()) {
        if(m_config->useParticleFlowJets()){
	  calibConfig   = m_jetAntiKt4_MCAFII_PFlow_ConfigFile;
          calibSequence = m_jetAntiKt4_MCAFII_PFlow_CalibSequence; 
        }
	else{
          calibConfig   = m_jetAntiKt4_MCAFII_ConfigFile;
          calibSequence = m_jetAntiKt4_MCAFII_CalibSequence;
        }
      }
      // FS - PFlow
      else if (m_config->useParticleFlowJets()) {
	calibConfig   = m_jetAntiKt4_PFlow_MCFS_ConfigFile;
	calibSequence = m_jetAntiKt4_PFlow_MCFS_CalibSequence;
      }
      // FS
      else {
        calibConfig   = m_jetAntiKt4_MCFS_ConfigFile;
        calibSequence = m_jetAntiKt4_MCFS_CalibSequence;
      }
    } 
    else {
      if (m_config->useParticleFlowJets()) {
        calibConfig   = m_jetAntiKt4_Data_PFlow_ConfigFile;
        calibSequence = m_jetAntiKt4_Data_PFlow_CalibSequence;        
      }else{
        calibConfig   = m_jetAntiKt4_Data_ConfigFile;
        calibSequence = m_jetAntiKt4_Data_CalibSequence;
      }
    }

    // Print out some information
    ATH_MSG_INFO("Configuration JES tools with ");
    ATH_MSG_INFO("JES Calibration Configuration : " << calibConfig);
    ATH_MSG_INFO("JES Calibration Sequence      : " << calibSequence);

    JetCalibrationTool* jetCalibrationTool = new JetCalibrationTool("JetCalibrationTool");
    top::check(asg::setProperty(jetCalibrationTool, "JetCollection", jetCalibrationName),
                "Failed to set JetCollection " + jetCalibrationName);
    top::check(asg::setProperty(jetCalibrationTool, "ConfigFile", calibConfig),
                "Failed to set ConfigFile " + calibConfig);
    top::check(asg::setProperty(jetCalibrationTool, "CalibSequence", calibSequence),
                "Failed to set CalibSequence " + calibSequence);
    top::check(asg::setProperty(jetCalibrationTool, "IsData", !m_config->isMC()),
                "Failed to set IsData " + !m_config->isMC());

    top::check(jetCalibrationTool->initializeTool(jetCalibrationName),
                "Failed to initialize JetCalibrationTool");
    m_jetCalibrationTool = jetCalibrationTool;
  }

  ///-- Update JVT --///
  const std::string jvt_update_name = "JetUpdateJvtTool";
  if (asg::ToolStore::contains<IJetUpdateJvt>(jvt_update_name)) {
    m_jetUpdateJvtTool = asg::ToolStore::get<IJetUpdateJvt>(jvt_update_name);
  } else {
    IJetUpdateJvt* jetUpdateJvtTool = new JetVertexTaggerTool(jvt_update_name);
    top::check(asg::setProperty(jetUpdateJvtTool, "JVTFileName", "JetMomentTools/" + m_jetJVT_ConfigFile),
                "Failed to set JVTFileName for JetUpdateJvtTool");
    top::check(jetUpdateJvtTool->initialize(), "Failed to initialize");
    m_jetUpdateJvtTool = jetUpdateJvtTool;
  }

  ///-- Update fJVT --///
  const std::string fjvt_tool_name = "fJVTTool";
  if (asg::ToolStore::contains<IJetModifier>(fjvt_tool_name)) {
    m_fjvtTool = asg::ToolStore::get<IJetModifier>(fjvt_tool_name);
  } else {
    IJetModifier *fJVTTool = new JetForwardJvtTool(fjvt_tool_name);
    top::check(asg::setProperty(fJVTTool, "JvtMomentName", "AnalysisTop_JVT"),
               "Failed to set JvtMomentName for JetForwardJvtTool");
    // following instructions from:
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/METUtilities#MET_with_forward_JVT
    if (m_config->fwdJetAndMET() == "fJVT")
    {
      top::check(asg::setProperty(fJVTTool, "CentralMaxPt", 60e3),
                 "Failed to set CentralMaxPt for JetForwardJvtTool");
    }
    if (m_config->fwdJetAndMET() == "fJVTTight")
    {
      top::check(asg::setProperty(fJVTTool, "CentralMaxPt", 60e3),
                 "Failed to set CentralMaxPt for JetForwardJvtTool");
      top::check(asg::setProperty(fJVTTool, "OutputDec", "passFJVTTight"),
                 "Failed to set OutputDec for JetForwardJvtTool");
      top::check(asg::setProperty(fJVTTool, "UseTightOP", true),
                 "Failed to set UseTightOP for JetForwardJvtTool");
    }
    top::check(fJVTTool->initialize(), "Failed to initialize " + fjvt_tool_name);
    m_fjvtTool = fJVTTool;
  }

  ///-- Jet Cleaning Tools --///
  m_jetCleaningToolLooseBad = setupJetCleaningTool("LooseBad");
  m_jetCleaningToolTightBad = setupJetCleaningTool("TightBad");

  m_jetEventCleaningToolLooseBad = setupJetEventCleaningTool("LooseBad", m_jetCleaningToolLooseBad);
  m_jetEventCleaningToolTightBad = setupJetEventCleaningTool("TightBad", m_jetCleaningToolTightBad);

  // Uncertainties
  // Is our MC full or fast simulation?
  std::string MC_type = (m_config->isAFII()) ? "AFII" : "MC16";

  // Moriond2018 - AF2 JES
  // Fall2018 - JES/JER update
  std::string conference = "Fall2018";

  // If JMS is allowed then there should also be extrapolation and frozen uncertainties configured
  std::string JMS_Uncertainty = "";
  if ( m_config->jetCalibSequence() == "JMS" )
   JMS_Uncertainty = "_JMSExtrap";
  
  // By setting calib_area to "None" we pick up the default from the JES group
  std::string calib_area = "None";
  
  // JER string option configuration 
  bool JERisMC = m_config->isMC();
  std::string JERSmearModel = m_config->jetJERSmearingModel();
  // Any PseudoData option (smear MC as data)
  if (JERSmearModel == "Full_PseudoData") {
    if (JERisMC) JERisMC = false;
    JERSmearModel = "Full";
    ATH_MSG_INFO("JER PseudoData option provided - Treating MC as if it is data for JER uncertainty");
  }
  if (JERSmearModel == "All_PseudoData") {
    if (JERisMC) JERisMC = false;
    JERSmearModel = "All";
    ATH_MSG_INFO("JER PseudoData option provided - Treating MC as if it is data for JER uncertainty");
  }
  
  // Strings need to be defined clearly for jet tool
  if(      JERSmearModel == "All")    JERSmearModel = "_AllJERNP";
  else if( JERSmearModel == "Full")   JERSmearModel = "_FullJER";
  else if( JERSmearModel == "Simple") JERSmearModel = "_SimpleJER";
  else {
    ATH_MSG_ERROR("Incorrect JER option: All, All_PseudoData, Full, Full_PseudoData, Simple");
    return StatusCode::FAILURE;
  }

  // Are we doing multiple JES for the reduced NP senarios?
  if (!m_config->doMultipleJES()) {
    m_jetUncertaintiesTool =  setupJetUncertaintiesTool("JetUncertaintiesTool",
							jetCalibrationName, 
							MC_type,
							JERisMC,
							"rel21/" + conference 
							+ "/R4_"  + m_config->jetUncertainties_NPModel() 
							+ JMS_Uncertainty
							+ JERSmearModel
							+ ".config",
							nullptr,
							m_config->jetUncertainties_QGFracFile(),
							calib_area
							);

    // Implement additional tool for frozen config when using JMS
    if (JMS_Uncertainty == "_JMSExtrap"){

      JMS_Uncertainty = "_JMSFrozen";
      m_jetUncertaintiesToolFrozenJMS = setupJetUncertaintiesTool("JetUncertaintiesToolFrozenJMS",
								  jetCalibrationName, 
								  MC_type,
								  JERisMC,
								  "rel21/" + conference
								  + "/R4_"  + m_config->jetUncertainties_NPModel()
								  + JMS_Uncertainty
								  + ".config",
								  nullptr,
								  m_config->jetUncertainties_QGFracFile(),
								  calib_area
								  );
    }

  } 
  else {

    // Strong reductions now enabled. If you want to run a single scenario please note the new config file names R4_SR_Scenario*_SimpleJER
    m_jetUncertaintiesToolReducedNPScenario1
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario1",
                                  jetCalibrationName, 
                                  MC_type,
                                  JERisMC,
                                  "rel21/"
                                  + conference
                                  + "/R4_SR_Scenario1_SimpleJER.config",
                                  nullptr,
                                  m_config->jetUncertainties_QGFracFile(),
                                  calib_area);
    m_jetUncertaintiesToolReducedNPScenario2
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario2",
                                  jetCalibrationName,
                                  MC_type,
                                  JERisMC,
                                  "rel21/"
                                  + conference
                                  + "/R4_SR_Scenario2_SimpleJER.config",
                                  nullptr,
                                  m_config->jetUncertainties_QGFracFile(),
                                  calib_area);
    m_jetUncertaintiesToolReducedNPScenario3
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario3",
                                  jetCalibrationName,
                                  MC_type,
                                  JERisMC,
                                  "rel21/"
                                  + conference
                                  + "/R4_SR_Scenario3_SimpleJER.config",
                                  nullptr,
                                  m_config->jetUncertainties_QGFracFile(),
                                  calib_area);
    m_jetUncertaintiesToolReducedNPScenario4
      = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario4",
                                  jetCalibrationName,
                                  MC_type,
                                  JERisMC,
                                  "rel21/"
                                  + conference
                                  + "/R4_SR_Scenario4_SimpleJER.config",
                                  nullptr,
                                  m_config->jetUncertainties_QGFracFile(),
                                  calib_area);
  }

  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupLargeRJetsCalibration() {
  std::string jetCalibrationNameLargeR = m_config->sgKeyLargeRJets();
  // erase "Jets" from the end
  jetCalibrationNameLargeR.erase(jetCalibrationNameLargeR.length() - 4);

  if (asg::ToolStore::contains<IJetCalibrationTool>("JetCalibrationToolLargeR")) {
    m_jetCalibrationToolLargeR = asg::ToolStore::get<IJetCalibrationTool>("JetCalibrationToolLargeR");
  } else {
    // Only a single calib config/sequence for MC and data
    // so just put it here for now.
    std::string calibConfigLargeR = "";
    const std::string calibChoice = m_config->largeRJESJMSConfig(); 
    if (calibChoice == "CombMass") {
      //calibConfigLargeR = "JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights_rel21.config";
      //calibConfigLargeR = "JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config";
      calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config";
    }
    else if (calibChoice == "TAMass") {
      //calibConfigLargeR = "JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config";
      calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_TA_12Oct2018.config";
    }
    else if (calibChoice == "CaloMass") {
      //calibConfigLargeR = "JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config";
      calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_calo_12Oct2018.config";
    }
    else {
      ATH_MSG_ERROR("Unknown largeRJESJMSConfig (Available options: TAMass, CaloMass and CombMass) : "+calibChoice);
      return StatusCode::FAILURE;
    }
    const std::string calibSequenceLargeR = "EtaJES_JMS";
    const std::string calibAreaLargeR = "00-04-82";
    JetCalibrationTool* jetCalibrationToolLargeR
      = new JetCalibrationTool("JetCalibrationToolLargeR");
    top::check(asg::setProperty(jetCalibrationToolLargeR, "JetCollection", jetCalibrationNameLargeR),
                "Failed to set JetCollection " + jetCalibrationNameLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "ConfigFile", calibConfigLargeR),
                "Failed to set ConfigFile " + calibConfigLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "CalibSequence", calibSequenceLargeR),
                "Failed to set CalibSequence " + calibSequenceLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "CalibArea", calibAreaLargeR),
                "Failed to set CalibArea " + calibAreaLargeR);
    top::check(asg::setProperty(jetCalibrationToolLargeR, "IsData", !m_config->isMC()),
                "Failed to set IsData " + !m_config->isMC());    
    top::check(jetCalibrationToolLargeR->initializeTool(jetCalibrationNameLargeR),
                "Failed to initialize JetCalibrationToolLargeR");
    m_jetCalibrationToolLargeR = jetCalibrationToolLargeR;
  }

  // Moriond2017 uncertainty recommendations:
  // names = "UJ_2016/Moriond2017/UJ2016_CaloMass_strong.config"     // strong,medium,weak    
  // names = "UJ_2016/Moriond2017/UJ2016_CombinedMass_strong.config" // strong,medium,weak

  std::string conference("");
  std::string configDir("");
  std::vector<std::string>* variables = nullptr;
  std::string largeRJES_config = m_config->largeRJESUncertaintyConfig();
  std::string calibArea  = "None"; // Take the default JetUncertainties CalibArea tag
  std::string MC_type = "MC16";

  conference = "Moriond2018";
  configDir  = "rel21";
  MC_type += "a";

  bool JERisMC = m_config->isMC();

  variables = new std::vector<std::string>;
  variables->push_back("pT");
  std::string variable;
  size_t pos_end = 0;
  while( (pos_end = largeRJES_config.find(",")) != std::string::npos) {
    variable = largeRJES_config.substr(0,pos_end);
    variables->push_back(variable);
    largeRJES_config.erase(0,pos_end+1);
  }
  variables->push_back(largeRJES_config);

  largeRJES_config = m_config->largeRJESJMSConfig();
  //if (largeRJES_config.find("UJ2016_") != 0) largeRJES_config.insert(0, "UJ2016_");

  m_jetUncertaintiesToolLargeR_strong
    = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR_Strong",
                                jetCalibrationNameLargeR, MC_type, JERisMC,
                                configDir+"/"+conference
                                + "/R10_"+largeRJES_config+"_strong.config",variables,"",calibArea);
  m_jetUncertaintiesToolLargeR_medium
    = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR_Medium",
                                jetCalibrationNameLargeR, MC_type, JERisMC,
                                configDir+"/"+conference
                                + "/R10_"+largeRJES_config+"_medium.config",variables,"",calibArea);
  m_jetUncertaintiesToolLargeR_weak
    = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR_Weak",
                                jetCalibrationNameLargeR, MC_type, JERisMC,
                                configDir+"/"+conference
                                + "/R10_"+largeRJES_config+"_weak.config",variables,"",calibArea);

  if (variables) delete variables;
  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupJetsScaleFactors() {
  // <tom.neep@cern.ch> Added 16th Feb 2016.
  // Jet JVT tool - can be used for both selection and for SFs
  // Since we use this for jet selection we also need it for data
  const std::string jvt_tool_name = "JetJvtEfficiencyTool";
  const std::string JVT_SFFile =
    (m_config->sgKeyJets()=="AntiKt4LCTopoJets")?
    "JetJvtEfficiency/Moriond2018/JvtSFFile_LC.root":      // LC jets
    (m_config->useParticleFlowJets())? 
    "JetJvtEfficiency/Moriond2018/JvtSFFile_EMPFlow.root": // pflow jets
    "JetJvtEfficiency/Moriond2018/JvtSFFile_EMTopoJets.root";      // default is EM jets

  if (asg::ToolStore::contains<CP::IJetJvtEfficiency>(jvt_tool_name)) {
    m_jetJvtTool = asg::ToolStore::get<CP::IJetJvtEfficiency>(jvt_tool_name);
  } else {
    CP::JetJvtEfficiency* jetJvtTool = new CP::JetJvtEfficiency(jvt_tool_name);
    // Medium WP default for EM or LC jets or PFlow jets (no longer special option)
    top::check(jetJvtTool->setProperty("WorkingPoint", "Medium"),
                "Failed to set JVT WP");
    top::check(jetJvtTool->setProperty("SFFile", JVT_SFFile),
                "Failed to set JVT SFFile name");
    top::check(jetJvtTool->setProperty("JetJvtMomentName", "AnalysisTop_JVT"),
                "Failed to set JVT decoration name");
    top::check(jetJvtTool->setProperty("TruthLabel", "AnalysisTop_isHS"),
                "Failed to set JVT TruthLabel decoration name");
    top::check(jetJvtTool->initialize(), "Failed to initialize JVT tool");
    m_jetJvtTool = jetJvtTool;
  }
  return StatusCode::SUCCESS;
}

StatusCode JetMETCPTools::setupMET()
{
  // See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EtmissRecommendationsRel21p2
  // METMaker tool
  if ( asg::ToolStore::contains<IMETMaker>("met::METMaker") ) {
    m_met_maker = asg::ToolStore::get<IMETMaker>("met::METMaker");
  } 
  else {
    met::METMaker *metMaker = new met::METMaker("met::METMaker");
    top::check(metMaker->setProperty("JetJvtMomentName", "AnalysisTop_JVT"), "Failed to set METMaker JVT moment name");
    if (m_config->fwdJetAndMET() == "Tight")
    {
      top::check(metMaker->setProperty("JetSelection", "Tight"), "Failed to set METMaker JetSelection to Tight");
    }
    else if (m_config->fwdJetAndMET() == "fJVT")
    {
      ATH_MSG_WARNING(" option fJVT no longer recommended, please use  fJVTTight. Option to be removed.");
      top::check(metMaker->setProperty("JetRejectionDec", "passFJVT"), "Failed to set METMaker JetRejectionDec to passFJVT");
    }
    else if ((m_config->fwdJetAndMET() == "fJVTTight"))
    {
      ATH_MSG_INFO("JetRejectionDec set to passFJVTTight");
      top::check(metMaker->setProperty("JetRejectionDec", "passFJVTTight"), "Failed to set METMaker JetRejectionDec to passFJVTTight");
    }
    top::check(metMaker->initialize(), "Failed to initialize");
    metMaker->msg().setLevel(MSG::INFO);
    m_met_maker = metMaker;
  }

  // MET Systematics tool
  if ( asg::ToolStore::contains<IMETSystematicsTool>("met::METSystematicsTool") ) {
    m_met_systematics = asg::ToolStore::get<IMETSystematicsTool>("met::METSystematicsTool");
  } 
  else {    
    met::METSystematicsTool* metSyst = new met::METSystematicsTool("met::METSystematicsTool");    
    // TST (Track soft terms)
    if (m_config->useParticleFlowJets()){
      top::check( metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms-pflow.config"), "Failed to set property");
    }
    else{
      if(m_config->isAFII()){
  top::check( metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms_AFII.config"), "Failed to set property" );
      }
      else{
  top::check( metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms.config"), "Failed to set property" );
      }
    }
    // Deactivate CST terms
    top::check( metSyst->setProperty("ConfigSoftCaloFile", "" ), "Failed to set property" );
    top::check( metSyst->initialize() , "Failed to initialize" );
    m_met_systematics = metSyst;
  }
  
  return StatusCode::SUCCESS;
}

ICPJetUncertaintiesTool*
JetMETCPTools::setupJetUncertaintiesTool(const std::string& name,
					 const std::string& jet_def,
					 const std::string& mc_type,
					 bool  isMC,
					 const std::string& config_file,
					 std::vector<std::string>* variables,
					 const std::string& analysis_file,
					 const std::string& calib_area
					 ) {
  ICPJetUncertaintiesTool* tool = nullptr;
  if (asg::ToolStore::contains<ICPJetUncertaintiesTool>(name)) {
    tool = asg::ToolStore::get<ICPJetUncertaintiesTool>(name);
  } else {
    tool = new JetUncertaintiesTool(name);
    top::check(asg::setProperty(tool, "JetDefinition", jet_def),
                "Failed to set JetDefinition for " + name);
    top::check(asg::setProperty(tool, "MCType", mc_type),
                "Failed to set MCType for " + name);
    top::check(asg::setProperty(tool, "IsData", !isMC),
	       "Failed to set IsData (for JER only)");
    top::check(asg::setProperty(tool, "ConfigFile", config_file),
                "Failed to set ConfigFile for " + name);
    if (variables != nullptr){
      top::check(asg::setProperty(tool, "VariablesToShift", *variables),
          "Failed to set VariablesToShift for LargeR Jes Uncertainty "+ name);
    }
    if (analysis_file != "None") {
        if (m_config->jetUncertainties_QGHistPatterns().size()==0 || analysis_file == "") { // no histogram pattern to look for, or empty analysis_file argument
            top::check(asg::setProperty(tool, "AnalysisFile", analysis_file),
                    "Failed to set AnalysisFile for " + name);
        }
        else if (m_config->jetUncertainties_QGHistPatterns().size()==1) { // a single pattern was specified - let's use it for all DSIDs
            top::check(asg::setProperty(tool, "AnalysisFile", analysis_file),
                    "Failed to set AnalysisFile for " + name);
            top::check(asg::setProperty(tool, "AnalysisHistPattern", m_config->jetUncertainties_QGHistPatterns()[0]),
                    "Failed to set AnalysisHistPattern for " + name);
        }
        else { // a list of DSIDs was specified
            int DSID = m_config->getDSID();
            for (auto s : m_config->jetUncertainties_QGHistPatterns()) {
                if (std::atoi(s.c_str()) == DSID) {
                    top::check(asg::setProperty(tool, "AnalysisFile", analysis_file),
                        "Failed to set AnalysisFile for " + name);
                    top::check(asg::setProperty(tool, "AnalysisHistPattern", s),
                        "Failed to set AnalysisHistPattern for " + name);
                    break;
                }
            }// if the DSID is not found in the list, we don't try to use the AnalysisFile, so we get the default 50 +/- 50%
        }
    }
    if (calib_area != "None"){
     top::check(asg::setProperty(tool, "CalibArea", calib_area),
                "Failed to set CalibArea " + calib_area);
    }
    top::check(tool->initialize(), "Failed to initialize " + name);
  }
  return tool;
}

IJetSelector* JetMETCPTools::setupJetCleaningTool(const std::string& WP) {
  IJetSelector* tool = nullptr;
  std::string name  = "JetCleaningTool" + WP;
  if (asg::ToolStore::contains<IJetSelector>(name)) {
    tool = asg::ToolStore::get<IJetSelector>(name);
  } else {
    tool = new JetCleaningTool(name);
      top::check(asg::setProperty(tool, "CutLevel", WP),
                  "Failed to set CutLevel to " + WP + " for " + name);
      top::check(asg::setProperty(tool, "DoUgly", false),
                  "Failed to set DoUgly for " + name);
      top::check(asg::setProperty(tool, "UseDecorations", true),
		 "Failed to set UserDecorations for " + name );
      top::check(tool->initialize(), "Failed to initialize " + name);
    }
  return tool;
}

ECUtils::IEventCleaningTool* JetMETCPTools::setupJetEventCleaningTool(const std::string& WP, ToolHandle<IJetSelector> JetCleaningToolHandle) {
  ECUtils::IEventCleaningTool* tool = nullptr;
  std::string name = "JetEventCleaningTool" + WP;
  if (asg::ToolStore::contains<ECUtils::IEventCleaningTool>(name)){
    tool = asg::ToolStore::get<ECUtils::IEventCleaningTool>(name);
  }
  else {
    tool = new ECUtils::EventCleaningTool(name);
    top::check(asg::setProperty(tool, "PtCut", std::to_string(m_config->jetPtcut())),
         "Failed to set jet pt cut in JetEventCleaningTool");
    top::check(asg::setProperty(tool, "EtaCut", std::to_string(m_config->jetEtacut())),
         "Failed to set jet eta cut in JetEventCleaningTool");
    top::check(asg::setProperty(tool, "JvtDecorator", "passJVT"),
         "Failed to set JVT property in JetEventCleaningTool");
    std::string OrDecorator = "";
    if (m_config->doLooseEvents()) 
      OrDecorator = "ORToolDecorationLoose";
    else 
      OrDecorator = "ORToolDecoration";
    top::check(asg::setProperty(tool, "OrDecorator", OrDecorator),
         "Failed to set jet OR decoration in JetEventCleaningTool");
    top::check(asg::setProperty(tool, "CleaningLevel", WP),
         "Failed to set jet WP "+ WP + " in JetEventCleaningTool");
    top::check(asg::setProperty(tool, "JetCleaningTool",JetCleaningToolHandle),
         "Failed to associate the JetCleaningTool object to JetEventCleaningTool");
    top::check(tool->initialize(), "Failed to initialize " + name);
  }

  return tool;

}


}  // namespace top

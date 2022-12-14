/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "JetUncertainties/FFJetSmearingTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetForwardJvtTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetJvtEfficiency/JetJvtEfficiency.h"
#include "JetSelectorTools/EventCleaningTool.h"
#include "JetUncertainties/FFJetSmearingTool.h"


// MET include(s):
#include "METUtilities/METMaker.h"
#include "METUtilities/METSystematicsTool.h"
#include "METUtilities/METSignificance.h"
#include "METInterface/IMETSignificance.h"

namespace top {
  JetMETCPTools::JetMETCPTools(const std::string& name) :
    asg::AsgTool(name),

    m_jetJVT_ConfigFile("JVTlikelihood_20140805.root"),

    // Calibration strings for R21 only
    m_jetAntiKt4_Data_ConfigFile("JES_MC16Recommendation_Consolidated_EMTopo_Apr2019_Rel21.config"),
    m_jetAntiKt4_Data_CalibSequence("JetArea_Residual_EtaJES_GSC_Insitu"),

    m_jetAntiKt4_MCFS_ConfigFile("JES_MC16Recommendation_Consolidated_EMTopo_Apr2019_Rel21.config"),
    m_jetAntiKt4_MCFS_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear"),

    m_jetAntiKt4_Data_JMS_ConfigFile("JES_JMS_MC16Recommendation_Consolidated_data_only_EMTopo_Sep2019_Rel21.config"),
    m_jetAntiKt4_Data_JMS_CalibSequence("JetArea_Residual_EtaJES_GSC_JMS_Insitu"),

    m_jetAntiKt4_MCFS_JMS_ConfigFile("JES_JMS_MC16Recommendation_Consolidated_MC_only_EMTopo_July2019_Rel21.config"),
    m_jetAntiKt4_MCFS_JMS_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear_JMS"),

    m_jetAntiKt4_MCAFII_ConfigFile("JES_MC16Recommendation_AFII_EMTopo_Apr2019_Rel21.config"),
    m_jetAntiKt4_MCAFII_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear"),

    m_jetAntiKt4_MCAFII_PFlow_ConfigFile("JES_MC16Recommendation_AFII_PFlow_Apr2019_Rel21.config"),
    m_jetAntiKt4_MCAFII_PFlow_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear"),

    m_jetAntiKt4_Data_PFlow_ConfigFile("JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config"),
    m_jetAntiKt4_Data_PFlow_CalibSequence("JetArea_Residual_EtaJES_GSC_Insitu"),

    m_jetAntiKt4_PFlow_MCFS_ConfigFile("JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config"),
    m_jetAntiKt4_PFlow_MCFS_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear"),

    m_jetAntiKt4_Data_PFlow_JMS_ConfigFile("JES_JMS_MC16Recommendation_Consolidated_data_only_PFlow_July2019_Rel21.config"),
    m_jetAntiKt4_Data_PFlow_JMS_CalibSequence("JetArea_Residual_EtaJES_GSC_JMS_Insitu"),

    m_jetAntiKt4_PFlow_MCFS_JMS_ConfigFile("JES_JMS_MC16Recommendation_Consolidated_MC_only_PFlow_July2019_Rel21.config"),
    m_jetAntiKt4_PFlow_MCFS_JMS_CalibSequence("JetArea_Residual_EtaJES_GSC_Smear_JMS") {
    declareProperty("config", m_config);

    declareProperty("JetCalibrationTool", m_jetCalibrationTool);
    declareProperty("JetCalibrationToolLargeR", m_jetCalibrationToolLargeR);

    declareProperty("JetUncertaintiesToolLargeR", m_jetUncertaintiesToolLargeR);
    declareProperty("JetUncertaintiesToolLargeRPseudoData", m_jetUncertaintiesToolLargeRPseudoData);

    declareProperty("JetUncertaintiesTool", m_jetUncertaintiesTool);
    declareProperty("JetUncertaintiesToolPseudoData", m_jetUncertaintiesToolPseudoData);
    declareProperty("FFJetSmearingTool", m_FFJetSmearingTool);
    declareProperty("JetUncertaintiesToolReducedNPScenario1", m_jetUncertaintiesToolReducedNPScenario1);
    declareProperty("JetUncertaintiesToolReducedNPScenario2", m_jetUncertaintiesToolReducedNPScenario2);
    declareProperty("JetUncertaintiesToolReducedNPScenario3", m_jetUncertaintiesToolReducedNPScenario3);
    declareProperty("JetUncertaintiesToolReducedNPScenario4", m_jetUncertaintiesToolReducedNPScenario4);

    declareProperty("JetCleaningToolLooseBad", m_jetCleaningToolLooseBad);
    declareProperty("JetCleaningToolTightBad", m_jetCleaningToolTightBad);

    declareProperty("JetEventCleaningToolLooseBad", m_jetEventCleaningToolLooseBad);
    declareProperty("JetEventCleaningToolTightBad", m_jetEventCleaningToolTightBad);

    declareProperty("JetUpdateJvtTool", m_jetUpdateJvtTool);
    declareProperty("JetSelectfJvtTool", m_jetSelectfJvtTool);
    declareProperty("TruthJetCollectionForHSTagging", m_truthJetCollForHS = "AntiKt4TruthJets");

    declareProperty("JES_data2016_data2015_Recommendation_Dec2016.config", m_jetAntiKt4_MCFS_ConfigFile);
    declareProperty("JetCalibrationSequenceFS", m_jetAntiKt4_MCFS_CalibSequence);

    declareProperty("JES_MC15cRecommendation_PFlow_Aug2016.config", m_jetAntiKt4_PFlow_MCFS_ConfigFile);
    declareProperty("JetArea_Residual_EtaJES_GSC", m_jetAntiKt4_PFlow_MCFS_CalibSequence);

    declareProperty("met_maker", m_met_maker);
    declareProperty("met_systematics", m_met_systematics);
    declareProperty("metSignif", m_metSignif);
  }

  StatusCode JetMETCPTools::initialize() {
    ATH_MSG_INFO("top::JetMETCPTools initialize...");
    if (m_config->isTruthDxAOD()) {
      ATH_MSG_INFO("top::JetMETCPTools: no need to initialise anything on truth DxAOD");
      return StatusCode::SUCCESS;
    }

    if (m_config->getDerivationStream() == "PHYS") {
      m_truthJetCollForHS = "AntiKt4TruthDressedWZJets";
    }

    if (m_config->useJets()) {
      if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
        top::check(setupJetsCalibration(), "Failed to setup jet calibration tools");
      }
      // JVT tool needed for both MC and data (for both selection and SFs)
      top::check(setupJetsScaleFactors(), "Failed to setup jet scale-factor tools");
    }

    if (m_config->useLargeRJets()) top::check(setupLargeRJetsCalibration(), "Failed to setup large-R jet calibration");

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
    const std::string caloJets_type = m_config->sgKeyJets();
    std::string jetCalibrationName;
    if (caloJets_type == "AntiKt4EMTopoNoElJets") {
      jetCalibrationName = "AntiKt4EMTopoJets";
    } else {
      jetCalibrationName = caloJets_type;
    }
    jetCalibrationName.erase(jetCalibrationName.length() - 4);

    ///-- Calibration --///
    if (asg::ToolStore::contains<IJetCalibrationTool>("JetCalibrationTool")) {
      m_jetCalibrationTool = asg::ToolStore::get<IJetCalibrationTool>("JetCalibrationTool");
    } else {
      std::string calibConfig, calibSequence;
      if (m_config->isMC()) { // MC calibration and sequence
        if (m_config->isAFII()) { // AFII calibrations, if supported
          if (m_config->jetCalibSequence() == "JMS") {
            if (m_config->allowSmallRJMSforAFII()) {
              ATH_MSG_WARNING("You enabled support for JMS calibration on AFII samples for small-R jets! NOTE that if you want to use this in an analysis, you must get back to JetEtMiss to demonstrate that AFII use is justifiable in your analysis.");
            } else {
              ATH_MSG_ERROR("The JMS calibration for small-R jets is  not supported for AFII samples! If you want to use this in an analysis, you must get back to JetEtMiss to demonstrate that AFII use is justifiable in your analysis. Set config option AllowJMSforAFII to True if you insist on running JMS on AFII.");
              return StatusCode::FAILURE;
            }
          }
          if (m_config->useParticleFlowJets()) {
            calibConfig = m_jetAntiKt4_MCAFII_PFlow_ConfigFile;
            calibSequence = m_jetAntiKt4_MCAFII_PFlow_CalibSequence;
          } else {
            calibConfig = m_jetAntiKt4_MCAFII_ConfigFile;
            calibSequence = m_jetAntiKt4_MCAFII_CalibSequence;
          }
        } else { // FullSim calibrations
          if (m_config->jetCalibSequence() == "JMS") {
            if (m_config->useParticleFlowJets()) { // PFlow
              calibConfig = m_jetAntiKt4_PFlow_MCFS_JMS_ConfigFile;
              calibSequence = m_jetAntiKt4_PFlow_MCFS_JMS_CalibSequence;
            } else { // EMTopo
              calibConfig = m_jetAntiKt4_MCFS_JMS_ConfigFile;
              calibSequence = m_jetAntiKt4_MCFS_JMS_CalibSequence;
            }
          } else { // GSC, no JMS calibration, fullsim
            if (m_config->useParticleFlowJets()) { // PFlow
              calibConfig = m_jetAntiKt4_PFlow_MCFS_ConfigFile;
              calibSequence = m_jetAntiKt4_PFlow_MCFS_CalibSequence;
            } else { // EMTopo
              calibConfig = m_jetAntiKt4_MCFS_ConfigFile;
              calibSequence = m_jetAntiKt4_MCFS_CalibSequence;
            }
          }
        }
      } else { // data calibration config & sequence
        if (m_config->jetCalibSequence() == "JMS") {
          if (m_config->useParticleFlowJets()) {
            calibConfig = m_jetAntiKt4_Data_PFlow_JMS_ConfigFile;
            calibSequence = m_jetAntiKt4_Data_PFlow_JMS_CalibSequence;
          } else {
            calibConfig = m_jetAntiKt4_Data_JMS_ConfigFile;
            calibSequence = m_jetAntiKt4_Data_JMS_CalibSequence;
          }
        } else { // GSC, no JMS calibration, data
          if (m_config->useParticleFlowJets()) {
            calibConfig = m_jetAntiKt4_Data_PFlow_ConfigFile;
            calibSequence = m_jetAntiKt4_Data_PFlow_CalibSequence;
          } else {
            calibConfig = m_jetAntiKt4_Data_ConfigFile;
            calibSequence = m_jetAntiKt4_Data_CalibSequence;
          }
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
                 "Failed to set IsData " + std::to_string (!m_config->isMC()));

      top::check(jetCalibrationTool->initialize(),
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
      top::check(asg::setProperty(jetUpdateJvtTool, "JetContainer", m_config->sgKeyJets()),
                 "Failed to set JetContainer for JetUpdateJvtTool");
      top::check(jetUpdateJvtTool->initialize(), "Failed to initialize");
      m_jetUpdateJvtTool = jetUpdateJvtTool;
    }
    
    ///-- Calculate fJVT --///
    //Only setup fJVT tool if user actually wants it
    if (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None") {
    
      const std::string fjvt_tool_name = "JetSelectfJvtTool";
      if (asg::ToolStore::contains<IJetModifier>(fjvt_tool_name)) {
        m_jetSelectfJvtTool = asg::ToolStore::get<IJetModifier>(fjvt_tool_name);
      } else {
        IJetModifier* JetSelectfJvtTool = new JetForwardJvtTool(fjvt_tool_name);
        top::check(asg::setProperty(JetSelectfJvtTool, "JvtMomentName", "AnalysisTop_JVT"), //fJVT uses JVT decision
        	   "Failed to set JvtMomentName for JetForwardJvtTool");
        
        //Default fJVT WP is medium but this can't be used with default Tight MET WP
        //MET WP takes precidence so making ATop default fJVT=Tight 
        if (m_config->getfJVTWP() != "Medium"){ 
          top::check(asg::setProperty(JetSelectfJvtTool, "UseTightOP", true),
                           "Failed to set UseTightOP for JetForwardJvtTool");
        }
        top::check(asg::setProperty(JetSelectfJvtTool, "OutputDec", "AnalysisTop_fJVTdecision"), //Adds custom decorator, 'AnalysisTop_fJVTdecision', to all jets
        	   "Failed to set OutputDec for JetForwardJvtTool");
  
        top::check(JetSelectfJvtTool->initialize(), "Failed to initialize " + fjvt_tool_name);
        m_jetSelectfJvtTool = JetSelectfJvtTool;
      }
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
    // Summer2019 - JES/JER update
    std::string conference = "Summer2019";

    // By setting calib_area to "None" we pick up the default from the JES group
    std::string calib_area = "None";

    // JER string option configuration
    const bool JERisPseudoData = (m_config->jetJERSmearingModel() == "Full_PseudoData") || (m_config->jetJERSmearingModel() == "All_PseudoData");
    std::string JERSmearModel = m_config->jetJERSmearingModel();
    std::string JMSOption = m_config->jetJMSOption();
    if (JMSOption != "None") {
      conference = "Spring2021"; // Updated files using the JMS option are in Spring2021
      if (JMSOption == "JMS_frozen") JMSOption = "_JMS_frozen";
      else if (JMSOption == "JMS_scaled") JMSOption = "_JMS_scaled";
      else {
        ATH_MSG_ERROR("Incorrect JMS option: None, JMS_frozen, JMS_scaled");
        return StatusCode::FAILURE;
      }
    }
    else JMSOption = ""; // Default JMSOption
    // Any PseudoData Option (Smear MC as data)
    if (JERSmearModel == "Full_PseudoData") {
      JERSmearModel = "Full";
      ATH_MSG_INFO("JER PseudoData option provided - Treating MC as if it is data for JER uncertainty");
    }
    if (JERSmearModel == "All_PseudoData") {
      JERSmearModel = "All";
      ATH_MSG_INFO("JER PseudoData option provided - Treating MC as if it is data for JER uncertainty");
    }

    // Strings need to be defined clearly for jet tool
    if (JERSmearModel == "All") JERSmearModel = "_AllJERNP";
    else if (JERSmearModel == "Full") JERSmearModel = "_FullJER";
    else if (JERSmearModel == "Simple") JERSmearModel = "_SimpleJER";
    else {
      ATH_MSG_ERROR("Incorrect JER option: All, All_PseudoData, Full, Full_PseudoData, Simple");
      return StatusCode::FAILURE;
    }

    // Are we doing multiple JES for the reduced NP senarios?
    if (!m_config->doMultipleJES()) {
      m_jetUncertaintiesTool = setupJetUncertaintiesTool("JetUncertaintiesTool",
                                                         jetCalibrationName,
                                                         MC_type,
                                                         m_config->isMC(),
                                                         "rel21/" + conference
                                                         + "/R4_" + m_config->jetUncertainties_NPModel()
                                                         + JERSmearModel
                                                         + JMSOption
                                                         + ".config",
                                                         nullptr,
                                                         m_config->jetUncertainties_QGFracFile(),
                                                         calib_area
                                                         );
      // setup the pseudodata tool when required
      if (JERisPseudoData) {
        m_jetUncertaintiesToolPseudoData = setupJetUncertaintiesTool("JetUncertaintiesToolPseudoData",
                                                                     jetCalibrationName,
                                                                     MC_type,
                                                                     false, // treat MC as data
                                                                     "rel21/" + conference
                                                                     + "/R4_" + m_config->jetUncertainties_NPModel()
                                                                     + JERSmearModel
                                                                     + JMSOption
                                                                     + ".config",
                                                                     nullptr,
                                                                     m_config->jetUncertainties_QGFracFile(),
                                                                     calib_area
                                                                     );

      }
    
    } else {
      // Strong reductions now enabled. If you want to run a single scenario please note the new config file names
      // R4_SR_Scenario*_SimpleJER
      m_jetUncertaintiesToolReducedNPScenario1
        = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario1",
                                    jetCalibrationName,
                                    MC_type,
                                    m_config->isMC(),
                                    "rel21/"
                                    + conference
                                    + "/R4_SR_Scenario1_SimpleJER"
                                    + JMSOption
                                    + ".config",
                                    nullptr,
                                    m_config->jetUncertainties_QGFracFile(),
                                    calib_area);
      m_jetUncertaintiesToolReducedNPScenario2
        = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario2",
                                    jetCalibrationName,
                                    MC_type,
                                    m_config->isMC(),
                                    "rel21/"
                                    + conference
                                    + "/R4_SR_Scenario2_SimpleJER"
                                    + JMSOption
                                    + ".config",
                                    nullptr,
                                    m_config->jetUncertainties_QGFracFile(),
                                    calib_area);
      m_jetUncertaintiesToolReducedNPScenario3
        = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario3",
                                    jetCalibrationName,
                                    MC_type,
                                    m_config->isMC(),
                                    "rel21/"
                                    + conference
                                    + "/R4_SR_Scenario3_SimpleJER"
                                    + JMSOption
                                    + ".config",
                                    nullptr,
                                    m_config->jetUncertainties_QGFracFile(),
                                    calib_area);
      m_jetUncertaintiesToolReducedNPScenario4
        = setupJetUncertaintiesTool("JetUncertaintiesToolReducedNPScenario4",
                                    jetCalibrationName,
                                    MC_type,
                                    m_config->isMC(),
                                    "rel21/"
                                    + conference
                                    + "/R4_SR_Scenario4_SimpleJER"
                                    + JMSOption
                                    + ".config",
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

    // Only a single calib config/sequence for MC and data
    // so just put it here for now.
    std::string calibConfigLargeR = "";
    const std::string calibChoice = m_config->largeRJESJMSConfig();
    if (asg::ToolStore::contains<IJetCalibrationTool>("JetCalibrationToolLargeR")) {
      m_jetCalibrationToolLargeR = asg::ToolStore::get<IJetCalibrationTool>("JetCalibrationToolLargeR");
    } else {
      if (m_config->isMC()) {
        if (calibChoice == "CombMass") {
          calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config";
        } else if (calibChoice == "TAMass") {
          calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_TA_12Oct2018.config";
        } else if (calibChoice == "CaloMass") {
          calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_calo_12Oct2018.config";
        } else if (calibChoice == "TCCMass") {
          calibConfigLargeR = "JES_MC16recommendation_FatJet_TCC_JMS_calo_30Oct2018.config";
	} else if (calibChoice == "UFOSDMass") {
	  calibConfigLargeR = "JES_MC16recommendation_R10_UFO_CSSK_SoftDrop_JMS_01April2020.config";
        } else {
          ATH_MSG_ERROR(
            "Unknown largeRJESJMSConfig (Available options: TAMass, CaloMass, CombMass, TCCMass and UFOSDMass)) : " + calibChoice);
          return StatusCode::FAILURE;
        }
      } else { //Insitu calibration for Data
        if ((calibChoice == "CombMass") || (calibChoice == "TAMass") || (calibChoice == "CaloMass")) {
          calibConfigLargeR = "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_March2021.config"; //Data has only one
                                                                                                  // config file
        } else if (calibChoice == "TCCMass") {
          calibConfigLargeR = "JES_MC16recommendation_FatJet_TCC_JMS_calo_30Oct2018.config"; //There's no insitu
                                                                                             // calibration yet
        } else if (calibChoice == "UFOSDMass") {
	  calibConfigLargeR = "JES_MC16recommendation_R10_UFO_CSSK_SoftDrop_JMS_01April2020.config"; //There's no insitu
	                                                                                     // calibration yet
	} else {
          ATH_MSG_ERROR(
            "Unknown largeRJESJMSConfig (Available options: TAMass, CaloMass, CombMass, TCCMass and UFOSDMass) : " + calibChoice);
          return StatusCode::FAILURE;
        }
      }
      std::string calibSequenceLargeR = "EtaJES_JMS";
      if ((!m_config->isMC()) &&
          (calibChoice != "TCCMass") &&
	  (calibChoice != "UFOSDMass")) calibSequenceLargeR = "EtaJES_JMS_Insitu_InsituCombinedMass"; //For data, there's
                                                                                                    // is insitu
                                                                                                    // calibration for
                                                                                                    // lc-topo jets
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
                 "Failed to set IsData " + std::to_string (!m_config->isMC()));
      top::check(jetCalibrationToolLargeR->initialize(),
                 "Failed to initialize JetCalibrationToolLargeR");
      m_jetCalibrationToolLargeR = jetCalibrationToolLargeR;
    }

    // Moriond2017 uncertainty recommendations:
    // names = "UJ_2016/Moriond2017/UJ2016_CaloMass_strong.config"     // strong,medium,weak
    // names = "UJ_2016/Moriond2017/UJ2016_CombinedMass_strong.config" // strong,medium,weak

    std::string configDir("");
    std::string largeRJESJERJMS_unc_config = m_config->largeRJetUncertainties_NPModel();
    std::string largeRJMR_unc_config = m_config->largeRJetUncertainties_JMR_NPModel();
    std::string calibArea = "None"; // Take the default JetUncertainties CalibArea tag
    std::string MC_type = "MC16";

    configDir = m_config->largeRJetUncertaintiesConfigDir(); 
   
    if(m_config->largeRJESJMSConfig() != "UFOSDMass"){

      m_jetUncertaintiesToolLargeR
        = setupJetUncertaintiesTool("JetUncertaintiesToolLargeR",
				    jetCalibrationNameLargeR, 
                                    MC_type, 
                                    m_config->isMC(),
                                    configDir + "/R10_" + largeRJESJERJMS_unc_config + ".config",
                                    nullptr,
                                    "",
                                    calibArea);
      
      // setup the large-R pseudodata tool when required
      if (m_config->isMC() && m_config->doLargeRPseudodataJER()) {
        m_jetUncertaintiesToolLargeRPseudoData
          = setupJetUncertaintiesTool("JetUncertaintiesToolLargeRPseudoData",
                                      jetCalibrationNameLargeR,
                                      MC_type,
                                      false, // treat MC as data
                                      configDir + "/R10_" + largeRJESJERJMS_unc_config + ".config",
                                      nullptr,
                                      "",
                                      calibArea);      
      } 

      if (!m_config->isSystNominal(m_config->systematics()))
        m_FFJetSmearingTool = setupFFJetSmearingTool(calibChoice,configDir + "/R10_" + largeRJMR_unc_config + ".config");

    }

    return StatusCode::SUCCESS;
  }

  ICPJetCorrectionTool * JetMETCPTools::setupFFJetSmearingTool(const std::string& mass_def, const std::string& config) {
    // <cgarner@physics.utoronto.ca> Added Apr. 13th, 2021
    // Function to setup FFJetSmearingTool needed for consolidated JMR uncertainties
   
    const std::string ff_tool_name = "FFJetSmearingTool";
    // erase "Mass" from the end
    const std::string mass_def_name = mass_def.substr(0, mass_def.size()-4);

    ICPJetCorrectionTool * tool = nullptr;

    if (asg::ToolStore::contains<ICPJetCorrectionTool>(ff_tool_name)) {
      tool = asg::ToolStore::get<ICPJetCorrectionTool>(ff_tool_name);
    } else { 
      tool = new CP::FFJetSmearingTool(ff_tool_name);
      top::check(asg::setProperty( tool, "MassDef", mass_def_name ), "Failed to set proper MassDef for FFJetSmearingTool" );
      top::check(asg::setProperty( tool, "ConfigFile", config ), "Failed to set proper config file for FFJetSmearingTool" );
      top::check(tool->initialize(), "Failed to initialize " + ff_tool_name );       
    }

    return tool;
  }

  StatusCode JetMETCPTools::setupJetsScaleFactors() {
    // <tom.neep@cern.ch> Added 16th Feb 2016.
    // Jet JVT tool - can be used for both selection and for SFs
    // Since we use this for jet selection we also need it for data
    const std::string jvt_tool_name = "JetJvtEfficiencyTool";
    const std::string JVT_SFFile =
      (m_config->sgKeyJets() == "AntiKt4LCTopoJets") ?
      "JetJvtEfficiency/Moriond2018/JvtSFFile_LC.root" :      // LC jets
      (m_config->useParticleFlowJets()) ?
      "JetJvtEfficiency/Moriond2018/JvtSFFile_EMPFlow.root" : // pflow jets
      "JetJvtEfficiency/Moriond2018/JvtSFFile_EMTopoJets.root";      // default is EM jets

    const std::string JVT_WP = m_config->getJVTWP();

    if (asg::ToolStore::contains<CP::IJetJvtEfficiency>(jvt_tool_name)) {
      m_jetJvtTool = asg::ToolStore::get<CP::IJetJvtEfficiency>(jvt_tool_name);
    } else {
      CP::JetJvtEfficiency* jetJvtTool = new CP::JetJvtEfficiency(jvt_tool_name);
      top::check(jetJvtTool->setProperty("WorkingPoint", JVT_WP),
                 "Failed to set JVT WP");
      top::check(jetJvtTool->setProperty("SFFile", JVT_SFFile),
                 "Failed to set JVT SFFile name");
      top::check(jetJvtTool->setProperty("JetJvtMomentName", "AnalysisTop_JVT"),
                 "Failed to set JVT decoration name");
      top::check(jetJvtTool->setProperty("TruthLabel", "AnalysisTop_isHS"),
                 "Failed to set JVT TruthLabel decoration name");
      top::check(jetJvtTool->setProperty("TruthJetContainerName", m_truthJetCollForHS),
                 "Failed to set JVT TruthJetContainerName decoration name");
      top::check(jetJvtTool->initialize(), "Failed to initialize JVT tool");
      m_jetJvtTool = jetJvtTool;
    }
 
    // <jonathan.jamieson@cern.ch> Added 9th June 2020.
    // Jet fJVT tool - uses same tool as for JVT,
    // Only setup fJVT Efficiency tool if user actually wants it
    if (m_config->getfJVTWP() != "None") {
      const std::string fjvt_tool_name = "JetForwardJvtEfficiencyTool";
      const std::string fJVT_SFFile =
	(m_config->useParticleFlowJets()) ?
	"JetJvtEfficiency/May2020/fJvtSFFile.EMPFlow.root" : // pflow jets
	"JetJvtEfficiency/May2020/fJvtSFFile.EMtopo.root";      // default is EM jets

      std::string fJVT_WP = m_config->getfJVTWP();
      if (fJVT_WP == "Medium"){
	fJVT_WP = "Loose"; //Efficiency tool still uses old WP names...
      }

      if (asg::ToolStore::contains<CP::IJetJvtEfficiency>(fjvt_tool_name)) {
	m_jetfJvtTool = asg::ToolStore::get<CP::IJetJvtEfficiency>(fjvt_tool_name);
      } else {
	CP::JetJvtEfficiency* jetfJvtTool = new CP::JetJvtEfficiency(fjvt_tool_name);
	top::check(jetfJvtTool->setProperty("WorkingPoint", fJVT_WP),
		   "Failed to set fJVT WP");
	top::check(jetfJvtTool->setProperty("SFFile", fJVT_SFFile),
		   "Failed to set fJVT SFFile name");
	top::check(jetfJvtTool->setProperty("UseMuSFFormat", true),
		   "Failed to set fJVT SFFile to updated mu binning");
	top::check(jetfJvtTool->setProperty("ScaleFactorDecorationName", "fJVTSF"),
		   "Failed to set fJVT SF decoration name");
	top::check(jetfJvtTool->setProperty("JetfJvtMomentName", "AnalysisTop_fJVTdecision"),
		   "Failed to set fJVT pass/fail decoration name");
	top::check(jetfJvtTool->setProperty("TruthLabel", "AnalysisTop_isHS"),
		   "Failed to set fJVT TruthLabel decoration name");
	top::check(jetfJvtTool->setProperty("TruthJetContainerName", m_truthJetCollForHS),
		   "Failed to set fJVT TruthJetContainerName decoration name");
	top::check(jetfJvtTool->initialize(), "Failed to initialize fJVT Efficiency tool");
	m_jetfJvtTool = jetfJvtTool;
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode JetMETCPTools::setupMET() {
    // See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EtmissRecommendationsRel21p2
    // METMaker tool
    if (asg::ToolStore::contains<IMETMaker>("met::METMaker")) {
      m_met_maker = asg::ToolStore::get<IMETMaker>("met::METMaker");
    } else {
      met::METMaker* metMaker = new met::METMaker("met::METMaker");
      top::check(metMaker->setProperty("JetJvtMomentName", "AnalysisTop_JVT"),
                 "Failed to set METMaker JVT moment name");

      if (m_config->useParticleFlowJets()) {
        top::check(metMaker->setProperty("DoPFlow", true), "Failed to set METMaker DoPFlow to true");
      }
     
      if (m_config->doForwardJVTinMET()) { 
        if (m_config->getfJVTWP() == "Medium") {
          top::check(metMaker->setProperty("JetSelection", "Tenacious"), "Failed to set METMaker JetSelection to Tenacious");
        }
        top::check(metMaker->setProperty("JetRejectionDec","AnalysisTop_fJVTdecision"), "Failed to set METMaker JetRejectionDec to AnalysisTop_fJVTdecision");
      }
      top::check(metMaker->initialize(), "Failed to initialize");
      metMaker->msg().setLevel(MSG::INFO);
      m_met_maker = metMaker;
    }

    // MET Systematics tool
    if (asg::ToolStore::contains<IMETSystematicsTool>("met::METSystematicsTool")) {
      m_met_systematics = asg::ToolStore::get<IMETSystematicsTool>("met::METSystematicsTool");
    } else {
      met::METSystematicsTool* metSyst = new met::METSystematicsTool("met::METSystematicsTool");
      
      std::string METconfigDir = m_config->METUncertaintiesConfigDir();
      if (METconfigDir == "Latest") METconfigDir = "METUtilities/run2_13TeV/"; //Find calib files here: /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/METUtilities/
      top::check(metSyst->setProperty("ConfigPrefix", METconfigDir), "Failed to set METsyst calibration area"); //Set calib area explicitly - removes potential for crash when default area is changed in METSyst code
      // TST (Track soft terms)
      if (m_config->useParticleFlowJets()) {
        top::check(metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms-pflow.config"), "Failed to set property");
      } else {
        if (m_config->isAFII() && METconfigDir == "METUtilities/data17_13TeV/prerec_Jan16/") {
          top::check(metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms_AFII.config"), "Failed to set property");
        } else {
          top::check(metSyst->setProperty("ConfigSoftTrkFile", "TrackSoftTerms.config"), "Failed to set property");
        }
      }
      // Deactivate CST terms
      top::check(metSyst->setProperty("ConfigSoftCaloFile", ""), "Failed to set property");
      top::check(metSyst->initialize(), "Failed to initialize");
      m_met_systematics = metSyst;
    }
    // MET significance tool
    if (asg::ToolStore::contains<IMETSignificance>("metSignificance")) {
      m_metSignif = asg::ToolStore::get<IMETSignificance>("metSignificance");
    } else {
      met::METSignificance* metSignificance = new met::METSignificance("metSignificance");
      met::SoftTermParams softTerm;
      if (m_config->METSignifSoftTermParam().compare("Random")==0) {
        softTerm = met::Random;
      } else if (m_config->METSignifSoftTermParam().compare("PthardParam")==0) {
        softTerm = met::PthardParam;
      } else if (m_config->METSignifSoftTermParam().compare("TSTParam")==0) {
        softTerm = met::TSTParam;
      } else {
        ATH_MSG_ERROR("top::JetMETCPTools: Unkown SoftTermParam for MetSignificance");
        // TODO actually throw some error here, though we should not get here since config restricts this to one of the three options above
      }
      top::check(metSignificance->setProperty("SoftTermParam", softTerm),   "Failed to set MetSignificance::SoftTermParam!");
      top::check(metSignificance->setProperty("DoPhiReso",     true),                   "Failed to set MetSignificance::DoPhiReso!");
      top::check(metSignificance->setProperty("IsDataJet",     false),                  "Failed to set MetSignificance::IsDataJet!");
      top::check(metSignificance->setProperty("IsAFII",        m_config->isAFII()), "Failed to set MetSignificance::IsAFII!");
      //chopping off the "Jets" part from the jet container name for correct initialization of the METSigTool.
      std::string jetCollectionName = m_config->sgKeyJets();
      std::string deleteString = "Jets";
      std::string::size_type index = jetCollectionName.find(deleteString);
      if(index != std::string::npos) jetCollectionName.erase(index, (jetCollectionName.length() - index));
      top::check(metSignificance->setProperty("JetCollection", jetCollectionName),       "Failed to set MetSignificance::JetCollection!");
      //disable TreatPUJets when using EMTopoJets and fjvt not active
      top::check(metSignificance->setProperty("TreatPUJets",(jetCollectionName != "AntiKt4EMTopo") || (m_config->doForwardJVTinMET()) ),"Failed to set MetSignificance::TreatPUJets!");
      top::check(metSignificance->initialize(), "Failed to initialize METSignificanceTool");
      m_metSignif = metSignificance;
    }

    return StatusCode::SUCCESS;
  }

  ICPJetUncertaintiesTool*
  JetMETCPTools::setupJetUncertaintiesTool(const std::string& name,
                                           const std::string& jet_def,
                                           const std::string& mc_type,
                                           bool isMC,
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
      if (variables != nullptr) {
        top::check(asg::setProperty(tool, "VariablesToShift", *variables),
                   "Failed to set VariablesToShift for LargeR Jes Uncertainty " + name);
      }
      if (analysis_file != "None") {
        if (m_config->jetUncertainties_QGHistPatterns().size() == 0 || analysis_file == "") { // no histogram pattern to look for, or empty analysis_file argument
          top::check(asg::setProperty(tool, "AnalysisFile", analysis_file),
                     "Failed to set AnalysisFile for " + name);
        } else if (m_config->jetUncertainties_QGHistPatterns().size() == 1) { // a single pattern was specified - let's use it for all DSIDs
          top::check(asg::setProperty(tool, "AnalysisFile", analysis_file),
                     "Failed to set AnalysisFile for " + name);
          top::check(asg::setProperty(tool, "AnalysisHistPattern", m_config->jetUncertainties_QGHistPatterns()[0]),
                     "Failed to set AnalysisHistPattern for " + name);
        } else { // a list of DSIDs was specified
          int DSID = m_config->getDSID();
          for (auto s : m_config->jetUncertainties_QGHistPatterns()) {
            if (std::atoi(s.c_str()) == DSID) {
              top::check(asg::setProperty(tool, "AnalysisFile", analysis_file),
                         "Failed to set AnalysisFile for " + name);
              top::check(asg::setProperty(tool, "AnalysisHistPattern", s),
                         "Failed to set AnalysisHistPattern for " + name);
              break;
            }
          }// if the DSID is not found in the list, we don't try to use the AnalysisFile, so we get the default 50 +/-
           // 50%
        }
      }
      if (calib_area != "None") {
        top::check(asg::setProperty(tool, "CalibArea", calib_area),
                   "Failed to set CalibArea " + calib_area);
      }
      top::check(tool->initialize(), "Failed to initialize " + name);
    }
    return tool;
  }

  IJetSelector* JetMETCPTools::setupJetCleaningTool(const std::string& WP) {
    IJetSelector* tool = nullptr;

    std::string name = "JetCleaningTool" + WP;
    if (asg::ToolStore::contains<IJetSelector>(name)) {
      tool = asg::ToolStore::get<IJetSelector>(name);
    } else {
      tool = new JetCleaningTool(name);
      top::check(asg::setProperty(tool, "CutLevel", WP),
                 "Failed to set CutLevel to " + WP + " for " + name);
      top::check(asg::setProperty(tool, "DoUgly", false),
                 "Failed to set DoUgly for " + name);
      top::check(asg::setProperty(tool, "UseDecorations", true),
                 "Failed to set UserDecorations for " + name);
      top::check(tool->initialize(), "Failed to initialize " + name);
    }
    return tool;
  }

  ECUtils::IEventCleaningTool* JetMETCPTools::setupJetEventCleaningTool(const std::string& WP,
                                                                        ToolHandle<IJetSelector> JetCleaningToolHandle)
  {
    ECUtils::IEventCleaningTool* tool = nullptr;
    std::string name = "JetEventCleaningTool" + WP;
    if (asg::ToolStore::contains<ECUtils::IEventCleaningTool>(name)) {
      tool = asg::ToolStore::get<ECUtils::IEventCleaningTool>(name);
    } else {
      tool = new ECUtils::EventCleaningTool(name);
      top::check(asg::setProperty(tool, "JetContainer", m_config->sgKeyJets()),
                 "Failed to set jet container name for JetEventCleaningTool");
      top::check(asg::setProperty(tool, "PtCut", std::to_string(m_config->jetPtcut())),
                 "Failed to set jet pt cut in JetEventCleaningTool");
      top::check(asg::setProperty(tool, "EtaCut", std::to_string(m_config->jetEtacut())),
                 "Failed to set jet eta cut in JetEventCleaningTool");
      top::check(asg::setProperty(tool, "JvtDecorator", "passJVT"),
                 "Failed to set JVT property in JetEventCleaningTool");
      std::string OrDecorator = "";
      if (m_config->doLooseEvents()) OrDecorator = "ORToolDecorationLoose";
      else OrDecorator = "ORToolDecoration";
      top::check(asg::setProperty(tool, "OrDecorator", OrDecorator),
                 "Failed to set jet OR decoration in JetEventCleaningTool");
      top::check(asg::setProperty(tool, "CleaningLevel", WP),
                 "Failed to set jet WP " + WP + " in JetEventCleaningTool");
      top::check(asg::setProperty(tool, "JetCleaningTool", JetCleaningToolHandle),
                 "Failed to associate the JetCleaningTool object to JetEventCleaningTool");
      top::check(tool->initialize(), "Failed to initialize " + name);
    }

    return tool;
  }
}  // namespace top

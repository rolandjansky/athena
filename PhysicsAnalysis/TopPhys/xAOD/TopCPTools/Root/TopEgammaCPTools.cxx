/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopEgammaCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Egamma include(s):
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"

namespace top {

EgammaCPTools::EgammaCPTools(const std::string& name) :
    asg::AsgTool(name),
    m_electronEffTriggerFile("SetMe"),
    m_electronEffTriggerLooseFile("SetMe"),
    m_electronEffSFTriggerFile("SetMe"),
    m_electronEffSFTriggerLooseFile("SetMe"),
    m_electronEffSFRecoFile("SetMe"),
    m_electronEffSFIDFile("SetMe"),
    m_electronEffSFIDLooseFile("SetMe"),
    m_electronEffSFIsoFile("SetMe"),
    m_electronEffSFIsoLooseFile("SetMe"),
    m_electronEffSFChargeIDFile("SetMe") {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "EgammaCalibrationAndSmearingTool" , m_egammaCalibrationAndSmearingTool );
  declareProperty( "ElectronEffTrigger" , m_electronEffTrigger );
  declareProperty( "ElectronEffTriggerLoose" , m_electronEffTriggerLoose );
  declareProperty( "ElectronEffSFTrigger" , m_electronEffSFTrigger );
  declareProperty( "ElectronEffSFTriggerLoose" , m_electronEffSFTriggerLoose );
  declareProperty( "ElectronEffReco" , m_electronEffSFReco );
  declareProperty( "ElectronEffID" , m_electronEffSFID );
  declareProperty( "ElectronEffIDLoose" , m_electronEffSFIDLoose );
  declareProperty( "ElectronEffIso" , m_electronEffSFIso );
  declareProperty( "ElectronEffIsoLoose" , m_electronEffSFIsoLoose );
  declareProperty( "ElectronEffChargeID" , m_electronEffSFChargeID );

  declareProperty( "PhotonIsEMSelectorLoose" ,  m_photonLooseIsEMSelector);
  declareProperty( "PhotonIsEMSelectorMedium" , m_photonMediumIsEMSelector);
  declareProperty( "PhotonIsEMSelectorTight" ,  m_photonTightIsEMSelector);
}

StatusCode EgammaCPTools::initialize() {
  ATH_MSG_INFO("top::EgammaCPTools initialize...");

  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::EgammaCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (m_config->usePhotons() || m_config->useElectrons()) {
    if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
      top::check(setupCalibration(), "Failed to setup Egamma calibration tools");
    }
    if (m_config->isMC()) {// scale-factors are only for MC
      top::check(setupScaleFactors(), "Failed to setup Egamma scale-factor tools");
    }
  }
  else {
    ATH_MSG_INFO("top::EgammaCPTools: no need to initialise anything since using neither electrons nor photons");
  }
  return StatusCode::SUCCESS;
}


StatusCode EgammaCPTools::setupCalibration() {
  // Setup electron and photon calibration tools
  // List of tools include:
  // - EgammaCalibrationAndSmearingTool
  // - Electron Charge ID Selector tool
  // - Photon shower shape fudge tool
  // - Photon efficiency correction tool
  using IEgammaCalibTool = CP::IEgammaCalibrationAndSmearingTool;
  const std::string egamma_calib_name = "CP::EgammaCalibrationAndSmearingTool";
  if (asg::ToolStore::contains<IEgammaCalibTool>(egamma_calib_name)) {
    m_egammaCalibrationAndSmearingTool = asg::ToolStore::get<IEgammaCalibTool>(egamma_calib_name);
  } else {
    IEgammaCalibTool* egammaCalibrationAndSmearingTool = new CP::EgammaCalibrationAndSmearingTool(egamma_calib_name);
    top::check(asg::setProperty(egammaCalibrationAndSmearingTool,
                                  "ESModel", "es2016data_mc15c"),
                  "Failed to set ESModel for " + egamma_calib_name);
    top::check(asg::setProperty(egammaCalibrationAndSmearingTool,
                                "decorrelationModel",
                                m_config->egammaSystematicModel()),
                "Failed to set decorrelationModel for " + egamma_calib_name);
    
    if(m_config->isAFII())
      top::check(asg::setProperty(egammaCalibrationAndSmearingTool,
                                  "useAFII", 1),
                  "Failed to useAFII to true for" + egamma_calib_name);

    top::check(egammaCalibrationAndSmearingTool->initialize(),
                "Failed to initialize " + egamma_calib_name);
    m_egammaCalibrationAndSmearingTool = egammaCalibrationAndSmearingTool;
  }

  // - Electron Charge ID Selector Tool
  // The only supported working point is Medium with 97% integrated efficiency
  // Note: this working point is to be applied on top of MediumLLH + d0z0 cuts + isolFixedCutTight
  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronChargeFlipTaggerTool
  std::string toolName="ECIDS_medium"; 
  AsgElectronChargeIDSelectorTool* electronChargeIDSelectorTool = new AsgElectronChargeIDSelectorTool(toolName);
  std::string trainingfile="ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root";
  float BDT_OP=-0.28087; //Set your operating point with the table above.
  top::check( electronChargeIDSelectorTool->setProperty("TrainingFile",trainingfile) , "Failed to setProperty" );
  top::check( electronChargeIDSelectorTool->setProperty("CutOnBDT",BDT_OP) , "Failed to setProperty" );
  top::check( electronChargeIDSelectorTool->initialize() , "Failed to initialize" );
  
  // The terribly named ElectronPhotonShowerShapeFudgeTool...
  // We apply this only to photons to correct the shower shape
  // This should only be applied on MC
  using IFudgeTool = IElectronPhotonShowerShapeFudgeTool;
  const std::string fudgeName = "PhotonFudgeTool";
  if (asg::ToolStore::contains<IFudgeTool>(fudgeName)) {
    m_photonFudgeTool = asg::ToolStore::get<IFudgeTool>(fudgeName);
  } else {
    IFudgeTool* fudge_tool = new ElectronPhotonShowerShapeFudgeTool(fudgeName);
    // Set Preselection to 21 (was 16) according to http://cern.ch/go/trk9
    // for MC15 samples, which are based on a geometry derived from GEO-21 from 2015+2016 data
    top::check(asg::setProperty(fudge_tool, "Preselection", 21),
                "Failed to set " + fudgeName + " property: Preselection");
    top::check(fudge_tool->initialize(),
                "Failed to initialize PhotonFudgeTool");
    m_photonFudgeTool = fudge_tool;
  }

  // The photon efficiency SF tool
  bool af2 = m_config->isAFII();
  int data_type = 0; // Data
  if (m_config->isMC()) {
    data_type = 1; // Full sim
    if (af2) {
      data_type = 3; // AF2
    }
  }

  // The file to make the tool with.
  std::string file_base = "PhotonEfficiencyCorrection/";
  std::string file_map = file_base + "2015_2016/rel20.7/Moriond2017_v1/map0.txt";
  file_map = PathResolverFindCalibFile(file_map);
  std::string file_con = "";
  std::string file_unc = "";
  if (af2) {
    // for AFII we keep using the previous recommendations
    file_base += "v1/efficiencySF.offline.Tight.2015.13TeV.rel20.AFII.";
    file_con = file_base; // Conv file
    file_unc = file_base; // Unconv file
    file_con += "con.v01.root";
    file_con = PathResolverFindCalibFile(file_con);
    file_unc += "unc.v01.root";
    file_unc = PathResolverFindCalibFile(file_unc);
  }

  using IPhotonEffTool = IAsgPhotonEfficiencyCorrectionTool;
  const std::string photonSFName = "AsgPhotonEfficiencyCorrectionTool"; // to retrieve ID Eff scale factors
  if (asg::ToolStore::contains<IPhotonEffTool>(photonSFName)) {
    m_photonEffSF = asg::ToolStore::get<IPhotonEffTool>(photonSFName);
  } else {
    if (m_config->isMC()) {  // Seem to only be able to setup the tool for MC
      IPhotonEffTool* photonEffSF = new AsgPhotonEfficiencyCorrectionTool(photonSFName);
      if (!af2) {
        top::check(asg::setProperty(photonEffSF,
                                    "MapFilePath",
                                    file_map),
                    "Failed to set MapFilePath for " + photonSFName);
      } else {
        top::check(asg::setProperty(photonEffSF,
                                    "CorrectionFileNameConv",
                                    file_con),
                    "Failed to set CorrectionFileNameConv for " + photonSFName);
        top::check(asg::setProperty(photonEffSF,
                                    "CorrectionFileNameUnconv",
                                    file_unc),
                    "Failed to set CorrectionFileNameUnconv for " + photonSFName);
      }
      top::check(asg::setProperty(photonEffSF, "ForceDataType", data_type),
                  "Failed to set ForceDataType for " + photonSFName);
      top::check(photonEffSF->initialize(),
                  "Failed to initialize " + photonSFName);
      m_photonEffSF = photonEffSF;
    }
  }

  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/IsolationSF2016Moriond#Radiative_Z_low_ET
  // To retrieve Isolation Eff scale factors
  // N.B. Naming for isolation working points for AsgPhotonEfficiencyCorrectionTool isolation scale factors
  // are different than those for the IsolationCorrectionTool (preceded by FixedCut)
  std::set<std::string> photon_isolations = {"TightCaloOnly",
                                              "Tight",
                                              "Loose"};
  for (const std::string& isoWP : photon_isolations) {
    std::string photonIsoSFName = "AsgPhotonEfficiencyCorrectionTool_IsoSF" + isoWP;
    if (!asg::ToolStore::contains<IPhotonEffTool>(photonIsoSFName)) {
      if (m_config->isMC() && !af2) { // only available for full simulation
        IPhotonEffTool* photonIsoSFTool = new AsgPhotonEfficiencyCorrectionTool(photonIsoSFName);
        top::check(asg::setProperty(photonIsoSFTool,
                                    "MapFilePath",
                                    file_map),
                    "Failed to set MapFilePath for " + photonIsoSFName);
        top::check(asg::setProperty(photonIsoSFTool, "ForceDataType", data_type),
                    "Failed to set ForceDataType for " + photonIsoSFName);
        top::check(asg::setProperty(photonIsoSFTool, "UseRadiativeZSF_mediumPT", m_config->photonUseRadiativeZ()),
                    "Failed to set useRadiativeZSF_mediumPT for " + photonIsoSFName);
        top::check(asg::setProperty(photonIsoSFTool, "IsoWP", isoWP),
                    "Failed to set IsoWP for " + photonIsoSFName);
        top::check(photonIsoSFTool->initialize(),
                    "Failed to initialize " + photonIsoSFName);
        m_photonIsoSFTools.push_back(photonIsoSFTool);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode EgammaCPTools::setupScaleFactors() {
  // Don't need for data, return SUCCESS straight away
  if (!m_config->isMC()) return StatusCode::SUCCESS;
  ///-- Scale factors --///

  std::string electron_data_dir = "ElectronEfficiencyCorrection/";
  ///-- Reco SFs doesn't depend on WP --///
  std::string electronID = m_config->electronID();
  electronID.replace(electronID.find("LH"), 2, "LLH");  // that way people do not have to change their cuts file
  std::string electronIDLoose = m_config->electronIDLoose();
  electronIDLoose.replace(electronIDLoose.find("LH"), 2, "LLH"); // that way people do not have to change their cuts file 

  m_electronEffSFRecoFile = electronSFFilePath("reco");
  m_electronEffSFIDFile = electronSFFilePath("ID", electronID);
  m_electronEffSFTriggerFile = electronSFFilePath("triggerSF", electronID,
                                                  m_config->electronIsolation());
  if (m_electronEffSFTriggerFile.empty()) {
    m_electronEffSFTriggerFile = electronSFFilePath("triggerSF", electronID,
                                                    "None");
  }
  m_electronEffTriggerFile = electronSFFilePath("triggerEff", electronID,
                                                m_config->electronIsolation());
  if (m_electronEffTriggerFile.empty()) {
    m_electronEffTriggerFile = electronSFFilePath("triggerEff", electronID,
                                                  "None");
  }
  if (m_config->electronIsolation() != "None") {
    m_electronEffSFIsoFile = electronSFFilePath("isolation", electronID,
                                                m_config->electronIsolation());
  }
  // Loose SFs
  m_electronEffSFIDLooseFile = electronSFFilePath("ID", electronIDLoose);
  m_electronEffSFTriggerLooseFile = electronSFFilePath("triggerSF", electronIDLoose,
                                                        m_config->electronIsolationLoose());
  if (m_electronEffSFTriggerLooseFile.empty()) {
    m_electronEffSFTriggerLooseFile = electronSFFilePath("triggerSF", electronIDLoose,
                                                          "None");
  }
  m_electronEffTriggerLooseFile = electronSFFilePath("triggerEff", electronIDLoose,
                                                      m_config->electronIsolationLoose());
  if (m_electronEffTriggerLooseFile.empty()) {
    m_electronEffTriggerLooseFile = electronSFFilePath("triggerEff", electronIDLoose,
                                                        "None");
  }
  if (m_config->electronIsolationLoose() != "None") {
    m_electronEffSFIsoLooseFile = electronSFFilePath("isolation", electronIDLoose,
                                                      m_config->electronIsolationLoose());
  }
  // for the moment only for MediumLH and FixedCutTight isolation
  // either at Tight or Loose level
  if ( ( electronIDLoose == "MediumLLH" && m_config->electronIsolationLoose() == "FixedCutTight" )
    || ( electronID == "MediumLLH" && m_config->electronIsolation() == "FixedCutTight" ) ) {
    m_electronEffSFChargeIDFile = electronSFFilePath("ChargeID", "MediumLLH",
                                              "FixedCutTight");
  }

  // The tools want the files in vectors: remove this with function
  std::vector<std::string> inRecoSF {m_electronEffSFRecoFile};
  std::vector<std::string> inIDSF {m_electronEffSFIDFile};
  std::vector<std::string> inTriggerSF {m_electronEffSFTriggerFile};
  std::vector<std::string> inTrigger {m_electronEffTriggerFile};

  std::vector<std::string> inIDSFLoose {m_electronEffSFIDLooseFile};
  std::vector<std::string> inTriggerSFLoose {m_electronEffSFTriggerLooseFile};
  std::vector<std::string> inTriggerLoose {m_electronEffTriggerLooseFile};
  std::vector<std::string> inChargeID {m_electronEffSFChargeIDFile};

  /**********************************************************************
    * Electron Isolation
    * Some WPs aren't supported at the moment...
    * As people want to use e.g. LooseLH electrons, we will allow
    * everything to procede without the isolation SFs, although they
    * should be careful and have to set ElectronIsoSFs False in their config
    **********************************************************************/
  std::vector< std::string > inIso;
  std::vector< std::string > inIsoLoose;

  if (m_config->electronIsolation() != "None") {
    if (m_electronEffSFIsoFile.empty()) {
      ATH_MSG_WARNING("Electron isolation configuration not found");
      if (m_config->electronIsoSFs()) {
        ATH_MSG_WARNING("If you really want to run with this electron "
                        "ID/Isolation setup then you can add:"
                        "\tElectronIsoSFs False\tto your config file");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Isolation SFs will be set to 1.0");
    } else {
      inIso.push_back( m_electronEffSFIsoFile );
    }
  }
  if (m_config->electronIsolationLoose() != "None") {
    if (m_electronEffSFIsoLooseFile.empty()) {
      ATH_MSG_WARNING("Loose Electron isolation configuration not found");
      if (m_config->electronIsoSFs()) {
        ATH_MSG_WARNING("If you really want to run with this (loose) "
                        "electron ID/Isolation setup then you can add:"
                        "\tElectronIsoSFs False\tto your config file");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Loose Isolation SFs will be set to 1.0");
    } else {
      inIsoLoose.push_back( m_electronEffSFIsoLooseFile );
    }
  }

  // data type
  // 0 : Data
  // 1 : MC FullSim
  // 3 : MC AFII
  int dataType(0);
  if (m_config->isMC()) {
    dataType = (m_config->isAFII()) ? 3 : 1;
  }
  
  const std::string elSFPrefix = "AsgElectronEfficiencyCorrectionTool_";

  // Reco SFs
  m_electronEffSFReco = setupElectronSFTool(elSFPrefix + "Reco", inRecoSF, dataType);
  // Trigger SFs
  m_electronEffSFTrigger = setupElectronSFTool(elSFPrefix + "TriggerSF", inTriggerSF, dataType);
  m_electronEffSFTriggerLoose = setupElectronSFTool(elSFPrefix + "TriggerSFLoose", inTriggerSFLoose, dataType);
  // Trigger Efficiencies (wow- this is bad naming!)
  m_electronEffTrigger = setupElectronSFTool(elSFPrefix + "Trigger", inTrigger, dataType);
  m_electronEffTriggerLoose = setupElectronSFTool(elSFPrefix + "TriggerLoose", inTriggerLoose, dataType);
  // ID SFs
  m_electronEffSFID = setupElectronSFTool(elSFPrefix + "ID", inIDSF, dataType);
  m_electronEffSFIDLoose = setupElectronSFTool(elSFPrefix + "IDLoose", inIDSFLoose, dataType);
  // Isolation SFs
  m_electronEffSFIso = setupElectronSFTool(elSFPrefix + "Iso", inIso, dataType);
  m_electronEffSFIsoLoose = setupElectronSFTool(elSFPrefix + "IsoLoose", inIsoLoose, dataType);
  
  // ChargeID SFs (if using Electron Charge ID Selector Tool)
  // for the moment only for MediumLH and FixedCutTight isolation
  // either at Tight or Loose level
  if ( ( electronIDLoose == "MediumLLH" && m_config->electronIsolationLoose() == "FixedCutTight" )
    || ( electronID == "MediumLLH" && m_config->electronIsolation() == "FixedCutTight" ) ) {
    // Charge Id efficiency scale factor
    m_electronEffSFChargeID = setupElectronSFTool(elSFPrefix + "ChargeID", inChargeID, dataType);

    // Charge flip correction: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaChargeMisIdentificationTool
    CP::ElectronChargeEfficiencyCorrectionTool* ChargeMisIDCorrections = new CP::ElectronChargeEfficiencyCorrectionTool("ElectronChargeEfficiencyCorrection");
    //top::check( ChargeMisIDCorrections->setProperty("OutputLevel",  MSG::VERBOSE ) , "Failed to setProperty" );
    top::check( ChargeMisIDCorrections->setProperty("CorrectionFileName", "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/ChargeCorrectionSF.Medium_FixedCutTightIso_CFTMedium.root") , "Failed to setProperty" );
    top::check( ChargeMisIDCorrections->initialize() , "Failed to setProperty" );
  }
  return StatusCode::SUCCESS;
}


IAsgElectronEfficiencyCorrectionTool*
EgammaCPTools::setupElectronSFTool(const std::string& name, const std::vector<std::string>& file_list, int data_type) {
  IAsgElectronEfficiencyCorrectionTool* tool = nullptr;
  if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool>(name)) {
    tool = asg::ToolStore::get<IAsgElectronEfficiencyCorrectionTool>(name);
  } else {
    if (!file_list.empty()) {  // If the file list is empty do nothing
      tool = new AsgElectronEfficiencyCorrectionTool(name);
      top::check(asg::setProperty(tool, "CorrectionFileNameList", file_list),
                  "Failed to set CorrectionFileNameList to " + name);
      top::check(asg::setProperty(tool, "ForceDataType", data_type),
                  "Failed to set ForceDataType to " + name);
      top::check(asg::setProperty(tool, "CorrelationModel", "TOTAL"),
                  "Failed to set CorrelationModel to " + name);
      top::check(tool->initialize(), "Failed to initialize " + name);
    }
  }
  return tool;
}

std::string EgammaCPTools::electronSFFilePath(const std::string& type, const std::string& ID, const std::string& isolation) {
  const std::string el_calib_path
      = "ElectronEfficiencyCorrection/" 
      "2015_2016/rel20.7/Moriond_February2017_v1/";

  const std::string ridiculous_trigger_string
    = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_"
    "OR_e60_lhmedium_"
    "OR_e120_lhloose_"
    "2016_e26_lhtight_nod0_ivarloose_"
    "OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0";

  std::string new_iso = (isolation == "None") ? "" : "_isol" + isolation;

  std::string file_path;
  
  if (type == "reco") {
    file_path = "offline/efficiencySF.offline.RecoTrk.root";
  } else if (type == "ID") {
    file_path = "offline/efficiencySF.offline." + ID + "_d0z0_v11.root";
  } else if (type == "isolation") {
    file_path = "isolation/efficiencySF.Isolation."
      + ID + "_d0z0_v11" + new_iso + ".root";
  } else if (type == "triggerSF") {
    file_path = "trigger/efficiencySF." + ridiculous_trigger_string
      + "." + ID + "_d0z0_v11" + new_iso + ".root";
  } else if (type == "triggerEff") {
    file_path = "trigger/efficiency." + ridiculous_trigger_string
      + "." + ID + "_d0z0_v11" + new_iso + ".root";
  } else if (type == "ChargeID") {
    if (ID != "MediumLLH") ATH_MSG_WARNING("Only Medium WP available at the moment " + ID);
    file_path = "charge_misID/efficiencySF.ChargeID.MediumLLH_d0z0_v11_isolFixedCutTight_MediumCFT.root";
  } else {
    ATH_MSG_ERROR("Unknown electron SF type");
  }
  return PathResolverFindCalibFile(el_calib_path + file_path);
}

}  // namespace top

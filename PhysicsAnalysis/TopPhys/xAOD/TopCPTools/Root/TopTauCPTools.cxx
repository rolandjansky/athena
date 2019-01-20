/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopTauCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Tau include(s):
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/Enums.h"

namespace top {

TauCPTools::TauCPTools(const std::string& name) :
    asg::AsgTool(name),
    m_pileupReweightingTool("CP::PileupReweightingTool"){
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "TauSmearingTool" , m_tauSmearingTool );
}

StatusCode TauCPTools::initialize() {
  ATH_MSG_INFO("top::TauCPTools initialize...");

  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::TauCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (!m_config->useTaus()) {
    ATH_MSG_INFO("top::TauCPTools: no need to initialise anything since not using taus");
    return StatusCode::SUCCESS;
  }

  if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
    top::check(setupCalibration(), "Failed to setup Tau calibration tools");
  }
  if (m_config->isMC()) {// scale-factors are only for MC
    top::check(setupScaleFactors(), "Failed to setup Tau scale-factor tools");
  }
  return StatusCode::SUCCESS;
}

StatusCode TauCPTools::setupCalibration() {
  /************************************************************
    *
    * Setup the tools needed for Tau Analysis
    * ----------------------------------------
    *
    * TauSelectionTool:
    *     to select taus
    * TauEfficiencyCorrectionTool:
    *     to apply efficiency corrections
    *     (this is setup with the selection tool)
    * TauSmearingTool:
    *     to calibrate and smear taus
    *
    ************************************************************/

  //============================================================
  // Convert tau jet IDWP string to int
  // see: [http://cern.ch/go/z6cq]
  // Updated: using enums from TauAnalysisTools package 
  //============================================================

  auto tau_JetIDWP_to_enum = [](const std::string& s) {
    if (s == "None") return TauAnalysisTools::JETIDNONE;
    if (s == "Loose") return TauAnalysisTools::JETIDBDTLOOSE;
    if (s == "Medium") return TauAnalysisTools::JETIDBDTMEDIUM;
    if (s == "Tight") return TauAnalysisTools::JETIDBDTTIGHT;
    if (s == "LooseNotMedium") return TauAnalysisTools::JETIDBDTLOOSENOTMEDIUM;
    if (s == "LooseNotTight") return TauAnalysisTools::JETIDBDTLOOSENOTTIGHT;
    if (s == "MediumNotTight") return TauAnalysisTools::JETIDBDTMEDIUMNOTTIGHT;
    if (s == "NotLoose") return TauAnalysisTools::JETIDBDTNOTLOOSE;

    // If we haven't found the correct WP, then return -1
    return TauAnalysisTools::JETIDNONEUNCONFIGURED;
  };

  // convert taujetIDWP from string to int and check it's valid
  int tauJetIDWP = tau_JetIDWP_to_enum(m_config->tauJetIDWP());
  top::check(tauJetIDWP >= 0, m_config->tauJetIDWP() + " is not a valid tau WP");

  // convert taujetIDWPLoose from string to int and check it's valid
  int tauJetIDWPLoose = tau_JetIDWP_to_enum(m_config->tauJetIDWPLoose());
  top::check(tauJetIDWPLoose >= 0, m_config->tauJetIDWPLoose() + " is not a valid tau WP");

  auto tau_EleBDTWP_to_enum = [](const std::string& s) {
    if (s == "None") return TauAnalysisTools::ELEIDNONE;
    if (s == "Loose") return TauAnalysisTools::ELEIDBDTLOOSE;
    if (s == "Medium") return TauAnalysisTools::ELEIDBDTMEDIUM;
    if (s == "Tight") return TauAnalysisTools::ELEIDBDTTIGHT;

    // If we haven't found the correct WP, then return -1
    return TauAnalysisTools::ELEIDNONEUNCONFIGURED;
  };

  // convert tauEleBDTWP from string to int and check it's valid
  int tauEleBDTWP = tau_EleBDTWP_to_enum(m_config->tauEleBDTWP());
  top::check(tauEleBDTWP >= 0, m_config->tauEleBDTWP() + " is not a valid tau WP");

  // convert tauEleBDTWPLoose from string to int and check it's valid
  int tauEleBDTWPLoose = tau_EleBDTWP_to_enum(m_config->tauEleBDTWPLoose());
  top::check(tauEleBDTWPLoose >= 0, m_config->tauEleBDTWPLoose() + " is not a valid tau WP");

  const std::vector<float> absEtaRegion = {0., 1.37, 1.37, 1.52, 1.52, 2.5};
  const double absCharge = 1.;
  const std::vector<size_t> nTracks = {1,3};

  //============================================================
  // PathResolve tau config files. If set to default then
  // set to empty string.
  //============================================================

  std::string tauJetConfigFile = m_config->tauJetConfigFile();
  if (tauJetConfigFile != "Default")
    tauJetConfigFile = PathResolverFindCalibFile(tauJetConfigFile);
  else
    tauJetConfigFile.clear();
  std::string tauJetConfigFileLoose = m_config->tauJetConfigFileLoose();
  if (tauJetConfigFileLoose != "Default")
    tauJetConfigFileLoose = PathResolverFindCalibFile(tauJetConfigFileLoose);
  else
    tauJetConfigFileLoose.clear();

  // bitmap in case not using the config files
  //WARNING: if we add more configurable cuts, they need to be added in this list
  // a priori we alway apply these cuts below
  int iSelectionCuts = 0;
  iSelectionCuts |= TauAnalysisTools::CutPt;
  iSelectionCuts |= TauAnalysisTools::CutAbsEta;
  iSelectionCuts |= TauAnalysisTools::CutAbsCharge;
  iSelectionCuts |= TauAnalysisTools::CutNTrack;
  iSelectionCuts |= TauAnalysisTools::CutJetIDWP;
  iSelectionCuts |= TauAnalysisTools::CutEleBDTWP;
  
  //WARNING the CutEleOLR is special, and has to be added only in the map if needed
  int iSelectionCutsLoose = iSelectionCuts;
  if (m_config->tauEleOLR()) iSelectionCuts |= TauAnalysisTools::CutEleOLR;
  if (m_config->tauEleOLRLoose()) iSelectionCutsLoose |= TauAnalysisTools::CutEleOLR;

  //============================================================
  // Nominal Tau Selection
  //============================================================


  // Alias long tool names
  using ITauSelTool = TauAnalysisTools::ITauSelectionTool;
  using ITauEffCorrTool = TauAnalysisTools::ITauEfficiencyCorrectionsTool;
  // Names of tools here for a little clarity
  std::string tauSelName = "TauAnalysisTools::TauSelectionTool";
  std::string tauEffCorrName = "TauAnalysisTools::TauEfficiencyCorrectionsTool";

  ///-- Setup the tau selection tool --///
  if (asg::ToolStore::contains<ITauSelTool>(tauSelName)) {
    m_tauSelectionTool = asg::ToolStore::get<ITauSelTool>(tauSelName);
  } else {
    ITauSelTool* tauSelectionTool = new TauAnalysisTools::TauSelectionTool(tauSelName);
    if (!tauJetConfigFile.empty()) {
      top::check(asg::setProperty(tauSelectionTool, "ConfigPath",tauJetConfigFile),
                  "Failed to set tau selection tool configuration path");
      top::check(asg::setProperty(tauSelectionTool, "OutputLevel", MSG::DEBUG),
                  "Failed to set tau OutputLevel");
    } else {
      // set the ConfigPath to empty value is no file was provided (otherwise it creates conflicts)
      top::check(asg::setProperty(tauSelectionTool, "ConfigPath", ""),
                  "Failed to set tau selection tool configuration path");
      top::check(asg::setProperty(tauSelectionTool, "OutputLevel", MSG::DEBUG),
                  "Failed to set tau OutputLevel");
      //WARNING: if we add more configurable cuts, they need to be added in the iSelectionCuts (see above)
      top::check(asg::setProperty(tauSelectionTool, "SelectionCuts", iSelectionCuts),
                  "Failed to set tau SelectionCuts");
      top::check(asg::setProperty(tauSelectionTool, "PtMin", static_cast<double>(m_config->tauPtcut()/1e3)),
                  "Failed to set tau pT cut");
      top::check(asg::setProperty(tauSelectionTool, "AbsEtaRegion", absEtaRegion),
                  "Failed to set tau AbsEtaRegion");
      top::check(asg::setProperty(tauSelectionTool, "AbsCharge", absCharge),
                  "Failed to set tau AbsCharge");
      top::check(asg::setProperty(tauSelectionTool, "NTracks", nTracks),
                  "Failed to set tau NTracks");
      top::check(asg::setProperty(tauSelectionTool, "JetIDWP", tauJetIDWP),
                  "Failed to set tau JetIDWP");
      top::check(asg::setProperty(tauSelectionTool, "EleBDTWP", tauEleBDTWP),
                  "Failed to set tau EleBDTWP");
      top::check(asg::setProperty(tauSelectionTool, "EleOLR", m_config->tauEleOLR()),
                  "Failed to set tau-electron overlap removal in tau selection tool");
    }
    top::check(tauSelectionTool->initialize(), "Failed to initialize tauSelectionTool");
    m_tauSelectionTool = tauSelectionTool;

    ///-- Setup the EfficiencyCorrectionsTool while seting up the selection tool --///
    if (asg::ToolStore::contains<ITauEffCorrTool>(tauEffCorrName)) {
      m_tauEffCorrTool = asg::ToolStore::get<ITauEffCorrTool>(tauEffCorrName);
    } else {
      ITauEffCorrTool* tauEffCorrTool
        = new TauAnalysisTools::TauEfficiencyCorrectionsTool(tauEffCorrName);
      top::check( m_pileupReweightingTool.retrieve(), "Failed to retireve pileup reweighting tool" );
      top::check(asg::setProperty(tauEffCorrTool, "PileupReweightingTool", m_pileupReweightingTool),
                  "Failed to set PileupReweightingTool for " + tauEffCorrName);
      top::check(asg::setProperty(tauEffCorrTool, "TauSelectionTool", m_tauSelectionTool),
                  "Failed to set TauSelectionTool for " + tauEffCorrName);
      top::check(tauEffCorrTool->initialize() , "Failed to initialize");
      m_tauEffCorrTool = tauEffCorrTool;
    }
  }

  //============================================================
  // 'Loose' Tau Selection
  //============================================================

  ///-- Names of tools here for a little clarity --///
  std::string tauSelNameLoose = "TauAnalysisTools::TauSelectionToolLoose";
  std::string tauEffCorrNameLoose = "TauAnalysisTools::TauEfficiencyCorrectionsToolLoose";

  ///-- Setup the tau selection tool --///
  if (asg::ToolStore::contains<ITauSelTool>(tauSelNameLoose)) {
    m_tauSelectionToolLoose = asg::ToolStore::get<ITauSelTool>(tauSelNameLoose);
  } else {
    ITauSelTool* tauSelectionTool = new TauAnalysisTools::TauSelectionTool(tauSelNameLoose);
    if (!tauJetConfigFileLoose.empty()) {
      top::check(asg::setProperty(tauSelectionTool, "ConfigPath",tauJetConfigFileLoose),
                  "Failed to set tau selection tool configuration path");
      top::check(asg::setProperty(tauSelectionTool, "OutputLevel", MSG::DEBUG),
                  "Failed to set tau OutputLevel");
    } else {
      // set the ConfigPath to empty value is no file was provided (otherwise it creates conflicts)
      top::check(asg::setProperty(tauSelectionTool, "ConfigPath", ""),
                  "Failed to set tau selection tool configuration path");
      top::check(asg::setProperty(tauSelectionTool, "OutputLevel", MSG::DEBUG),
                  "Failed to set tau OutputLevel");
      //WARNING: if we add more configurable cuts, they need to be added in the iSelectionCuts (see above)
      top::check(asg::setProperty(tauSelectionTool, "SelectionCuts", iSelectionCutsLoose),
                  "Failed to set tau SelectionCuts");
      top::check(asg::setProperty(tauSelectionTool, "PtMin", static_cast<double>(m_config->tauPtcut()/1e3)),
                  "Failed to set tau pT cut");
      top::check(asg::setProperty(tauSelectionTool, "AbsEtaRegion", absEtaRegion),
                  "Failed to set loose tau AbsEtaRegion");
      top::check(asg::setProperty(tauSelectionTool, "AbsCharge", absCharge),
                  "Failed to set loose tau AbsCharge");
      top::check(asg::setProperty(tauSelectionTool, "NTracks", nTracks),
                  "Failed to set loose tau NTracks");
      top::check(asg::setProperty(tauSelectionTool, "JetIDWP", tauJetIDWPLoose),
                  "Failed to set loose tau JetIDWP");
      top::check(asg::setProperty(tauSelectionTool, "EleBDTWP", tauEleBDTWPLoose),
                  "Failed to set loose tau EleBDTWP");
      top::check(asg::setProperty(tauSelectionTool, "EleOLR", m_config->tauEleOLRLoose()),
                  "Failed to set tau-electron overlap removal in loose tau selection tool");
    }
    top::check(tauSelectionTool->initialize(), "Failed to initialize tauSelectionTool");
    m_tauSelectionToolLoose = tauSelectionTool;

    ///-- Setup the EfficiencyCorrectionsTool while seting up the selection tool --///
    if (asg::ToolStore::contains<ITauEffCorrTool>(tauEffCorrNameLoose)) {
      m_tauEffCorrTool = asg::ToolStore::get<ITauEffCorrTool>(tauEffCorrNameLoose);
    } else {
      ITauEffCorrTool* tauEffCorrTool
        =  new TauAnalysisTools::TauEfficiencyCorrectionsTool(tauEffCorrNameLoose);
      top::check( m_pileupReweightingTool.retrieve(), "Failed to retireve pileup reweighting tool" );
      top::check(asg::setProperty(tauEffCorrTool, "PileupReweightingTool", m_pileupReweightingTool),
                  "Failed to set PileupReweightingTool for " + tauEffCorrNameLoose);
      top::check(asg::setProperty(tauEffCorrTool, "TauSelectionTool", m_tauSelectionToolLoose),
                  "Failed to set TauSelectionTool for " + tauEffCorrNameLoose);
      top::check(tauEffCorrTool->initialize() , "Failed to initialize");
      m_tauEffCorrToolLoose = tauEffCorrTool;
    }
  }

  ///-- Calibration and smearing --///
  std::string tauSmearName = "TauAnalysisTools::TauSmearingTool";
  if (asg::ToolStore::contains<TauAnalysisTools::ITauSmearingTool>(tauSmearName)) {
    m_tauSmearingTool = asg::ToolStore::get<TauAnalysisTools::ITauSmearingTool>(tauSmearName);
  } else {
    TauAnalysisTools::TauSmearingTool* tauSmearingTool =  new TauAnalysisTools::TauSmearingTool(tauSmearName);
    top::check(tauSmearingTool->initialize() , "Failed to initialize");
    m_tauSmearingTool = tauSmearingTool;
  }

  return StatusCode::SUCCESS;
}

StatusCode TauCPTools::setupScaleFactors() {
  /**
    *
    * Fill this in at some point...
    *
    * ...or maybe we don't need to
    *
    **/
  return StatusCode::SUCCESS;
}

}  // namespace top

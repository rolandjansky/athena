/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopMuonCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Muon include(s):
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"

namespace top {

MuonCPTools::MuonCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "MuonCalibrationAndSmearingTool" , m_muonCalibrationAndSmearingTool );
  declareProperty( "MuonSelectionTool" , m_muonSelectionTool );
  declareProperty( "MuonSelectionToolLoose" , m_muonSelectionToolLoose );
  declareProperty( "MuonSelectionToolVeryLooseVeto" , m_muonSelectionToolVeryLooseVeto );

  declareProperty( "MuonEfficiencyCorrectionsTool" , m_muonEfficiencyCorrectionsTool );
  declareProperty( "MuonEfficiencyCorrectionsToolLoose" , m_muonEfficiencyCorrectionsToolLoose );
  declareProperty( "MuonEfficiencyCorrectionsToolIso" , m_muonEfficiencyCorrectionsToolIso );
  declareProperty( "MuonEfficiencyCorrectionsToolLooseIso" , m_muonEfficiencyCorrectionsToolLooseIso );
}

StatusCode MuonCPTools::initialize() {
  ATH_MSG_INFO("top::MuonCPTools initialize...");

  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::MuonCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (!m_config->useMuons()) {
    ATH_MSG_INFO("top::MuonCPTools: no need to initialise anything since not using muons");
    return StatusCode::SUCCESS;
  }

  if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
    top::check(setupCalibration(), "Failed to setup Muon calibration tools");
  }
  if (m_config->isMC()) {// scale-factors are only for MC
    top::check(setupScaleFactors(), "Failed to setup Muon scale-factor tools");
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonCPTools::setupCalibration() {
  ///-- Calibration and smearing --///
  using IMuCalibSmearTool = CP::IMuonCalibrationAndSmearingTool;
  const std::string mu_calib_smearing_name = "CP::MuonCalibrationAndSmearingTool";
  if (asg::ToolStore::contains<IMuCalibSmearTool>(mu_calib_smearing_name)) {
    m_muonCalibrationAndSmearingTool = asg::ToolStore::get<IMuCalibSmearTool>(mu_calib_smearing_name);
  } 
  else {
    IMuCalibSmearTool* muonCalibrationAndSmearingTool = new CP::MuonCalibrationAndSmearingTool(mu_calib_smearing_name);
    
    // Adding Release 21 scrutiny settings (https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AtlasRelease21Scrutiny#Temporary_Recommendations_for_Co)
    if(m_release_series == 25){
      ATH_MSG_INFO( "CP::MuonCalibrationAndSmearingTool will be configured for Rel21 scrutiny exercise" );
      ATH_MSG_INFO( "Setting CP::MuonCalibrationAndSmearingTool -> StatComb to false" );
      top::check(asg::setProperty(muonCalibrationAndSmearingTool, "StatComb", false),
                 "Unable to change StatComb property in " + mu_calib_smearing_name);
    }
    
    top::check(muonCalibrationAndSmearingTool->initialize(),
	       "Failed to initialize " + mu_calib_smearing_name);
    
    m_muonCalibrationAndSmearingTool = muonCalibrationAndSmearingTool;
  }

  ///-- Selection --///
  m_muonSelectionTool = setupMuonSelectionTool("CP::MuonSelectionTool",
                                                m_config->muonQuality(),
                                                m_config->muonEtacut());
  m_muonSelectionToolLoose = setupMuonSelectionTool("CP::MuonSelectionToolLoose",
                                                    m_config->muonQualityLoose(),
                                                    m_config->muonEtacut());
  // the following is needed to make sure all muons for which d0sig is calculated are at least Loose
  m_muonSelectionToolVeryLooseVeto = setupMuonSelectionTool("CP::MuonSelectionToolVeryLooseVeto",
                                                    "Loose",
                                                    2.5);
  return StatusCode::SUCCESS;
}

StatusCode MuonCPTools::setupScaleFactors() {
  // Setup muon SF tools
  // However if we are running on data- we don't need these,
  // so carry on.
  if (!m_config->isMC())
    return StatusCode::SUCCESS;
  /************************************************************
    *
    * Muon Scale Factors:
    *    muonSF = trigSF*effSF*isoSF*TTVASF
    *
    ************************************************************/

  // Muon SF tools now require you to have setup an instance of
  // the pileup reweighting tool!
  // If we haven't set it up then tell the user this and exit.
  if (!m_config->doPileupReweighting()) {
    ATH_MSG_ERROR("\nThe Muon SF tools now require that you have"
                  " previously setup an instance of "
                  "the pileup reweighting tool.\n\n"
                  "To do this set the options:\n\n\t"
                  "PRWLumiCalcFiles\n and \n\tPRWConfigFiles \n\n"
                  "in your config file.");
    return StatusCode::FAILURE;
  }

  /************************************************************
    * Trigger Scale Factors:
    *    setup trigger SFs for nominal and 'loose' muon WPs
    *    recommendation for EOYE not to pass any isolation to tool
    *    as SFs very similar for all WPs.
  ************************************************************/

  if(m_config->getReleaseSeries() == 24){
    m_muonTriggerScaleFactors_2015
      = setupMuonTrigSFTool("CP::MuonTriggerScaleFactors_2015",
			    m_config->muonQuality(), "2015");
    m_muonTriggerScaleFactorsLoose_2015
      = setupMuonTrigSFTool("CP::MuonTriggerScaleFactorsLoose_2015",
			    m_config->muonQualityLoose(), "2015");
    m_muonTriggerScaleFactors_2016
      = setupMuonTrigSFTool("CP::MuonTriggerScaleFactors_2016",
			    m_config->muonQuality(), "2016");
    m_muonTriggerScaleFactorsLoose_2016
      = setupMuonTrigSFTool("CP::MuonTriggerScaleFactorsLoose_2016",
			    m_config->muonQualityLoose(), "2016");
  }
  // In R21 now, we only need one instance of the tool 
  // and do not need to set the year as it is handled
  // internally with PRW tool
  if(m_config->getReleaseSeries() == 25){
    m_muonTriggerScaleFactors_R21
      = setupMuonTrigSFTool("CP::MuonTriggerScaleFactors_R21",
                            m_config->muonQuality(), "");
    m_muonTriggerScaleFactorsLoose_R21
      = setupMuonTrigSFTool("CP::MuonTriggerScaleFactorsLoose_R21",
                            m_config->muonQualityLoose(), "");
  }
  /************************************************************
    * Efficiency Scale Factors:
    *    setup muon efficiency SFs for the nominal and 
    *    'loose' muon WPs.
  ************************************************************/
  m_muonEfficiencyCorrectionsTool
    = setupMuonSFTool("CP::MuonEfficiencyScaleFactorsTool",
                      m_config->muonQuality());

  m_muonEfficiencyCorrectionsToolLoose
    = setupMuonSFTool("CP::MuonEfficiencyScaleFactorsToolLoose",
                      m_config->muonQualityLoose());
  
  /************************************************************
    * Isolation Scale Factors:
    *    setup muon isolation SFs for the nominal and 'loose' 
    *    muons
    *
    *    Note: if isolation WP is None, then don't setup the tool
    ************************************************************/
  // If we don't want isolation then we don't need the tool
  if (m_config->muonIsolation() != "None") {
    // Add iso as a suffix (see above for consistency between tools :) )
    std::string muon_isolation = m_config->muonIsolation();
    muon_isolation += "Iso";
    m_muonEfficiencyCorrectionsToolIso = 
      setupMuonSFTool("CP::MuonEfficiencyScaleFactorsToolIso",
                        muon_isolation);
  }

  // Do we have isolation on our loose muons? If not no need for the tool...
  if (m_config->muonIsolationLoose() != "None") {
    // Add iso as a suffix (see above for consistency between tools :) )
    // Note: now loose isolation
    std::string muon_isolation = m_config->muonIsolationLoose();
    muon_isolation += "Iso";
    m_muonEfficiencyCorrectionsToolLooseIso =
      setupMuonSFTool("CP::MuonEfficiencyScaleFactorsToolLooseIso",
                      muon_isolation);
  }

  /************************************************************
    * Muon TTVA SF:
    *    Track-to-vertex association. This depends on whether or
    *    not we apply the tracking groups recommended impact
    *    parameter cuts to associate muon to vertex.
    ************************************************************/
  m_muonEfficiencyCorrectionsToolTTVA
    = setupMuonSFTool("CP::MuonEfficiencyScaleFactorsToolTTVA",
                      "TTVA");

  // WARNING - The PromptLeptonIsolation scale factors are only derived with respect to the loose PID
  //         - Hence we need to fail if this has occured
  if( (m_config->muonQuality() != "Loose" && m_config->muonIsolation() == "PromptLepton")
      || (m_config->muonQualityLoose() !="Loose" && m_config->muonIsolationLoose() == "PromptLepton") ){
    ATH_MSG_ERROR("Cannot use PromptLeptonIsolation on muons without using Loose quality - Scale factors are not available");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

CP::IMuonSelectionTool*
MuonCPTools::setupMuonSelectionTool(const std::string& name, const std::string& quality, double max_eta) {
  std::map<std::string, int> muon_quality_map = {
    {"Tight" , 0}, {"Medium", 1}, {"Loose", 2}, {"VeryLoose", 3}};
  int qual_int;
  try {
    qual_int = muon_quality_map.at(quality);
  } catch (const std::out_of_range& oor_exc) {
    ATH_MSG_ERROR("\n Invalid muon quality ("
                  + quality + ") for " + name
                  + ". Valid options are: "
                  " \n\t- Tight"
                  " \n\t- Medium"
                  " \n\t- Loose"
                  " \n\t- VeryLoose");
    throw;  // Re-throw
  }

  CP::IMuonSelectionTool* tool = nullptr;
  if (asg::ToolStore::contains<CP::IMuonSelectionTool>(name)) {
    tool = asg::ToolStore::get<CP::IMuonSelectionTool>(name);
  } else {
    tool = new CP::MuonSelectionTool(name);
    top::check(asg::setProperty(tool, "MuQuality", qual_int),
                "Failed to set MuQuality for " + name);
    top::check(asg::setProperty(tool, "MaxEta", max_eta),
                "Failed to set MaxEta for " + name);
    top::check(tool->initialize(), "Failed to initialize " + name);
  }
  return tool;
}


CP::IMuonTriggerScaleFactors*
MuonCPTools::setupMuonTrigSFTool(const std::string& name, const std::string& quality, const std::string& year) {
  CP::IMuonTriggerScaleFactors* tool = nullptr;
  if (asg::ToolStore::contains<CP::IMuonTriggerScaleFactors>(name)) {
    tool = asg::ToolStore::get<CP::IMuonTriggerScaleFactors>(name);
  } else {
    tool = new CP::MuonTriggerScaleFactors(name);
    top::check(asg::setProperty(tool, "MuonQuality", quality),
                "Failed to set MuonQuality for " + name);    
    // Setting year and MC is only needed in R20.7 
    // R21 is smarter and uses eventInfo for this info
    if(m_config->getReleaseSeries() == 24){
      top::check(asg::setProperty(tool, "Year", year),
		 "Failed to set Year for " + name);
      std::string MC_version = "mc15c";
      top::check(asg::setProperty(tool, "MC", MC_version),
		 "Failed to set MC for " + name);
    }
    else{
      // This is now needed for R21 as we get many error messages otherwise
      top::check(asg::setProperty(tool,"AllowZeroSF", true),
		 "Failed to set AllowZeroSF for "+name);
    }
    top::check(tool->initialize(), "Failed to init. " + name);
  }
  return tool;
}


CP::IMuonEfficiencyScaleFactors*
MuonCPTools::setupMuonSFTool(const std::string& name, const std::string& WP) {
  CP::IMuonEfficiencyScaleFactors* tool = nullptr;
  if (asg::ToolStore::contains<CP::IMuonEfficiencyScaleFactors>(name)) {
    tool = asg::ToolStore::get<CP::MuonEfficiencyScaleFactors>(name);
  } else {
    tool = new CP::MuonEfficiencyScaleFactors(name);
    top::check(asg::setProperty(tool, "WorkingPoint", WP),
                "Failed to set WP for " + name + " tool");
    top::check(tool->initialize(),
                "Failed to set initialize " + name);
  }
  return tool;
}

}  // namespace top

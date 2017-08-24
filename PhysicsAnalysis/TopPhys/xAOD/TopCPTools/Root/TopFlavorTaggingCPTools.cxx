/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopFlavorTaggingCPTools.h"

#include <map>
#include <string>
#include <algorithm>
#include <iterator>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/Tokenize.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

namespace top {

FlavorTaggingCPTools::FlavorTaggingCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );
}

StatusCode FlavorTaggingCPTools::initialize() {
  ATH_MSG_INFO("top::FlavorTaggingCPTools initialize...");

  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::FlavorTaggingCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (!m_config->useJets()) {
    ATH_MSG_INFO("top::FlavorTaggingCPTools: no need to initialise anything since not using jets");
    return StatusCode::SUCCESS;
  }

  // 2.4 code
  if( m_release_series == 24 ){
    m_tagger = "MV2c10";
    m_tagger_algorithms = {"MV2c10"};
    m_cdi_file = "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-2017-06-07_v2.root";
    m_efficiency_maps = "410000;410004;410500;410187;410021";

    // working points for calo jets
    m_calo_WPs_calib = {"FixedCutBEff_60",
			"FixedCutBEff_70",
			"FixedCutBEff_77",
			"FixedCutBEff_85",
			"Continuous" };
    m_calo_WPs = m_calo_WPs_calib;
    m_calo_WPs.push_back("FlatBEff_30");// some other WPs are defined in addition to the calibrated ones
    m_calo_WPs.push_back("FlatBEff_40");
    m_calo_WPs.push_back("FlatBEff_50");
    m_calo_WPs.push_back("FlatBEff_60");
    m_calo_WPs.push_back("FlatBEff_70");
    m_calo_WPs.push_back("FlatBEff_77");
    m_calo_WPs.push_back("FlatBEff_85");
    m_calo_WPs.push_back("FixedCutBEff_30");
    m_calo_WPs.push_back("FixedCutBEff_50");
    m_calo_WPs.push_back("FixedCutBEff_80");
    m_calo_WPs.push_back("FixedCutBEff_90");
    
    // working points for track jets (AntiKt2)
    m_trackAntiKt2_WPs_calib = {"FixedCutBEff_60",
				"FixedCutBEff_70",
				"FixedCutBEff_77",
				"FixedCutBEff_85",
				"Continuous" };
    m_trackAntiKt2_WPs = m_trackAntiKt2_WPs_calib;
    m_trackAntiKt2_WPs.push_back("FixedCutBEff_30");// some other WPs are defined in addition to the calibrated ones
    m_trackAntiKt2_WPs.push_back("FixedCutBEff_50");
    m_trackAntiKt2_WPs.push_back("FixedCutBEff_80");
    m_trackAntiKt2_WPs.push_back("FixedCutBEff_90");
    
    // working points for track jets (AntiKt4)
    m_trackAntiKt4_WPs_calib = {"FixedCutBEff_70",
				"FixedCutBEff_77" };
    m_trackAntiKt4_WPs = m_trackAntiKt4_WPs_calib;
    m_trackAntiKt4_WPs.push_back("FixedCutBEff_60");// some other WPs are defined in addition to the calibrated ones
    m_trackAntiKt4_WPs.push_back("FixedCutBEff_85");
    m_trackAntiKt4_WPs.push_back("FixedCutBEff_30");
    m_trackAntiKt4_WPs.push_back("FixedCutBEff_50");
    m_trackAntiKt4_WPs.push_back("FixedCutBEff_80");
    m_trackAntiKt4_WPs.push_back("FixedCutBEff_90");
  }

  // 2.6/21.2
  if( m_release_series == 25 ){
    m_tagger          = ""; // Extract in the loop
    m_cdi_file        = "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2017-07-02_v1.root"; // New file
    m_efficiency_maps = "410501"; //"410000;410004;410500;410187;410021"; // Not sure what is available yet

    // Working points - available for all algorithms
    m_calo_WPs_calib = {"FixedCutBEff_60",
			"FixedCutBEff_70",
			"FixedCutBEff_77",
			"FixedCutBEff_85",
			"HybBEff_60",
			"HybBEff_70",
			"HybBEff_77",
			"HybBEff_85"};
    // All WPs are calibrated ones
    m_calo_WPs       = m_calo_WPs_calib;
    // Only for calo jets for now
    m_trackAntiKt2_WPs_calib = {};
    m_trackAntiKt4_WPs_calib = {};
    // List of algorithms in R21
    m_tagger_algorithms = {"MV2c10",
			   "MV2c10mu", 
			   "MV2c10rnn", 
			   "DL1", 
			   "DL1mu", 
			   "DL1rnn"};
  }

  const std::string calib_file_path = PathResolverFindCalibFile(m_cdi_file);
  const std::string excludedSysts = m_config->bTagSystsExcludedFromEV()=="none"?"":m_config->bTagSystsExcludedFromEV();
  
  //------------------------------------------------------------
  // Loop through all the different working points we have and create a
  // BTaggingSelectionTool and corresponding BTaggingEfficiencyTool if the working point is calibrated.
  //------------------------------------------------------------

  // special stuff to use AntiKt4EMTopoJets scale-factors and tagger WPs when using AntiKt4EMPFlowJets or AntiKt4LCTopoJets, for which no SF is yet available
  std::string caloJets_collection = m_config->sgKeyJets();
  if (caloJets_collection == "AntiKt4EMPFlowJets" || caloJets_collection == "AntiKt4LCTopoJets" || caloJets_collection == "AntiKt4EMTopoNoElJets") {
    ATH_MSG_WARNING("top::FlavorTaggingCPTools::initialize" );
    ATH_MSG_WARNING( "     No b-tagging calibration available for jet collection " + caloJets_collection);
    ATH_MSG_WARNING( "     We'll use the calibration for AntiKt4EMTopoJets instead");
    ATH_MSG_WARNING("      Be careful!!" );
    caloJets_collection = "AntiKt4EMTopoJets";
  }
  // check if the WP requested by the user are available, and if yes, initialize the tools
  // loop through all btagging WPs requested
  for (auto TaggerBtagWP : m_config->bTagWP()) {
    // Overwrite m_tagger anyway (default has to be mv2c10 for R20.7
    m_tagger               = TaggerBtagWP.first;        
    std::string btagWP     = TaggerBtagWP.second;
    std::string bTagWPName = m_tagger + "_" + btagWP;
    if( std::find(m_calo_WPs.begin(), m_calo_WPs.end(), btagWP) == m_calo_WPs.end() || std::find(m_tagger_algorithms.begin(), m_tagger_algorithms.end(), m_tagger) == m_tagger_algorithms.end() ) {
      ATH_MSG_WARNING("top::FlavorTaggingCPTools::initialize" );
      ATH_MSG_WARNING("     b-tagging WP: " + btagWP + " not supported for jet collection " + m_config->sgKeyJets() + " with algorithm " + m_tagger);
      ATH_MSG_WARNING("     it will therefore be ignored" );
    }
    else {
      //------------------------------------------------------------
      // Setup BTaggingSelectionTool
      //------------------------------------------------------------
      // Updated name to use m_tagger
      std::string btagsel_tool_name = "BTaggingSelectionTool_"+bTagWPName+"_"+m_config->sgKeyJets();
      BTaggingSelectionTool* btagsel = new BTaggingSelectionTool(btagsel_tool_name);
      top::check(btagsel->setProperty("TaggerName", m_tagger),
                "Failed to set b-tagging selecton tool TaggerName");
      top::check(btagsel->setProperty("JetAuthor", caloJets_collection),
                "Failed to set b-tagging selection JetAuthor");
      top::check(btagsel->setProperty("FlvTagCutDefinitionsFileName",
                                      m_cdi_file),
                "Failed to set b-tagging selection tool CDI file");
      top::check(btagsel->setProperty("OperatingPoint", btagWP),
                "Failed to set b-tagging selection tool OperatingPoint");
      top::check(btagsel->setProperty("MinPt",
                                      static_cast<double>(m_config->jetPtcut())),
                "Failed to set b-tagging selection tool MinPt");
      top::check(btagsel->setProperty("MaxEta",
                                      static_cast<double>(m_config->jetEtacut())),
                "Failed to set b-tagging selection tool MaxEta");
      top::check(btagsel->initialize(),
                "Failed to initialize b-tagging selection tool");
      m_btagging_selection_tools.push_back(btagsel);
        
      if(std::find( m_calo_WPs_calib.begin(),
                    m_calo_WPs_calib.end(), btagWP) == m_calo_WPs_calib.end()) {
        ATH_MSG_WARNING("top::FlavorTaggingCPTools::initialize" );
        ATH_MSG_WARNING("     b-tagging WP: " + btagWP + " is not calibrated for jet collection " + m_config->sgKeyJets());
	ATH_MSG_WARNING("     it will therefore be ignored for the scale-factors, although the tagging decisions will be saved");
      }
      else {
        //------------------------------------------------------------
        // Setup BTaggingEfficiencyTool
        //------------------------------------------------------------
        std::string btageff_tool_name = "BTaggingEfficiencyTool_"+bTagWPName+"_"+m_config->sgKeyJets();
        BTaggingEfficiencyTool* btageff = new BTaggingEfficiencyTool(btageff_tool_name);
        top::check(btageff->setProperty("TaggerName", m_tagger),
                  "Failed to set b-tagging TaggerName");
        top::check(btageff->setProperty("OperatingPoint", btagWP),
                  "Failed to set b-tagging OperatingPoint");
        top::check(btageff->setProperty("JetAuthor", caloJets_collection),
                  "Failed to set b-tagging JetAuthor");
        top::check(btageff->setProperty("EfficiencyFileName", calib_file_path),
                  "Failed to set path to b-tagging CDI file");
        top::check(btageff->setProperty("ScaleFactorFileName", calib_file_path),
                  "Failed to set path to b-tagging CDI file");
        top::check(btageff->setProperty("ScaleFactorBCalibration", m_config->bTaggingCalibration_B()),
                  "Failed to set b-tagging calibration (B): "+m_config->bTaggingCalibration_B());
        top::check(btageff->setProperty("ScaleFactorCCalibration", m_config->bTaggingCalibration_C()),
                  "Failed to set b-tagging calibration (C): "+m_config->bTaggingCalibration_C());
        // using same calibration for T as for C
        top::check(btageff->setProperty("ScaleFactorTCalibration", m_config->bTaggingCalibration_C()),
                  "Failed to set b-tagging calibration (T): "+m_config->bTaggingCalibration_C());
        top::check(btageff->setProperty("ScaleFactorLightCalibration", m_config->bTaggingCalibration_Light()),
                  "Failed to set b-tagging calibration (Light): "+m_config->bTaggingCalibration_Light());
        for (auto jet_flav : m_jet_flavors) {
          top::check(btageff->setProperty("Efficiency"+jet_flav+"Calibrations", m_efficiency_maps),
                    "Failed to set "+jet_flav+"-calibrations efficiency maps");
        }
	top::check(btageff->setProperty("ExcludeFromEigenVectorTreatment", excludedSysts),
		   "Failed to set b-tagging systematics to exclude from EV treatment");
	top::check(btageff->initialize(), "Failed to initialize "+btagWP);
	// Check the excludedSysts - Cannot check before the tool is initialised
	top::check(this->checkExcludedSysts(btageff, excludedSysts),                                                                                                                                                                                                    
                   "Incorrect excluded systematics have been provided."); 
        m_btagging_efficiency_tools.push_back(btageff);
        m_config->setBTagWP_calibrated(bTagWPName);
      }
      m_config->setBTagWP_available(bTagWPName);
    }

    if (m_config->useTrackJets()) {
      std::vector<std::string> track_WPs = {};
      std::vector<std::string> track_WPs_calib = {};
      if (m_config->sgKeyTrackJets() == "AntiKt2PV0TrackJets") {
        track_WPs = m_trackAntiKt2_WPs;
        track_WPs_calib = m_trackAntiKt2_WPs_calib;
      }
      else if (m_config->sgKeyTrackJets() == "AntiKt4PV0TrackJets") {
        track_WPs = m_trackAntiKt4_WPs;
        track_WPs_calib = m_trackAntiKt4_WPs_calib;
      }
      
      if(std::find(track_WPs.begin(), track_WPs.end(), btagWP) == track_WPs.end()) {
        ATH_MSG_WARNING("top::FlavorTaggingCPTools::initialize" );
        ATH_MSG_WARNING("     b-tagging WP: " + btagWP + " not supported for jet collection " + m_config->sgKeyTrackJets());
	ATH_MSG_WARNING("     it will therefore be ignored" );
      }
      else {
        //------------------------------------------------------------
        // Setup BTaggingSelectionTool
        //------------------------------------------------------------
        std::string btagsel_tool_name = "BTaggingSelectionTool_"+bTagWPName+"_"+m_config->sgKeyTrackJets();
        BTaggingSelectionTool* btagsel = new BTaggingSelectionTool(btagsel_tool_name);
        top::check(btagsel->setProperty("TaggerName", m_tagger),
                  "Failed to set b-tagging selecton tool TaggerName");
        top::check(btagsel->setProperty("JetAuthor", m_config->sgKeyTrackJets()),
                  "Failed to set b-tagging selection JetAuthor");
        top::check(btagsel->setProperty("FlvTagCutDefinitionsFileName",
                                        m_cdi_file),
                  "Failed to set b-tagging selection tool CDI file");
        top::check(btagsel->setProperty("OperatingPoint", btagWP),
                  "Failed to set b-tagging selection tool OperatingPoint");
        top::check(btagsel->setProperty("MinPt",
                                        static_cast<double>(m_config->jetPtcut())),
                  "Failed to set b-tagging selection tool MinPt");
        top::check(btagsel->setProperty("MaxEta",
                                        static_cast<double>(m_config->jetEtacut())),
                  "Failed to set b-tagging selection tool MaxEta");
        top::check(btagsel->initialize(),
                  "Failed to initialize b-tagging selection tool");
        m_btagging_selection_tools.push_back(btagsel);
        
        if(std::find( track_WPs_calib.begin(),
                      track_WPs_calib.end(), btagWP) == track_WPs_calib.end()) {
          ATH_MSG_WARNING("top::FlavorTaggingCPTools::initialize" );
          ATH_MSG_WARNING("     b-tagging WP: " + btagWP + " is not calibrated for jet collection " + m_config->sgKeyTrackJets() );
	  ATH_MSG_WARNING("     it will therefore be ignored for the scale-factors, although the tagging decisions will be saved");
        }
        else {
          //------------------------------------------------------------
          // Setup BTaggingEfficiencyTool
          //------------------------------------------------------------
          std::string btageff_tool_name = "BTaggingEfficiencyTool_"+bTagWPName+"_"+m_config->sgKeyTrackJets();
          BTaggingEfficiencyTool* btageff = new BTaggingEfficiencyTool(btageff_tool_name);
          top::check(btageff->setProperty("TaggerName", m_tagger),
                    "Failed to set b-tagging TaggerName");
          top::check(btageff->setProperty("OperatingPoint", btagWP),
                    "Failed to set b-tagging OperatingPoint");
          top::check(btageff->setProperty("JetAuthor", m_config->sgKeyTrackJets()),
                    "Failed to set b-tagging JetAuthor");
          top::check(btageff->setProperty("EfficiencyFileName", calib_file_path),
                    "Failed to set path to b-tagging CDI file");
          top::check(btageff->setProperty("ScaleFactorFileName", calib_file_path),
                    "Failed to set path to b-tagging CDI file");
          top::check(btageff->setProperty("ScaleFactorBCalibration", m_config->bTaggingCalibration_B()),
                    "Failed to set b-tagging calibration (B): "+m_config->bTaggingCalibration_B());
          top::check(btageff->setProperty("ScaleFactorCCalibration", m_config->bTaggingCalibration_C()),
                    "Failed to set b-tagging calibration (C): "+m_config->bTaggingCalibration_C());
          // using same calibration for T as for C
          top::check(btageff->setProperty("ScaleFactorTCalibration", m_config->bTaggingCalibration_C()),
                    "Failed to set b-tagging calibration (T): "+m_config->bTaggingCalibration_C());
          top::check(btageff->setProperty("ScaleFactorLightCalibration", m_config->bTaggingCalibration_Light()),
                    "Failed to set b-tagging calibration (Light): "+m_config->bTaggingCalibration_Light());
          for (auto jet_flav : m_jet_flavors) {
            top::check(btageff->setProperty("Efficiency"+jet_flav+"Calibrations", m_efficiency_maps),
                      "Failed to set "+jet_flav+"-calibrations efficiency maps");
          }
	  top::check(btageff->setProperty("ExcludeFromEigenVectorTreatment", excludedSysts),
		     "Failed to set b-tagging systematics to exclude from EV treatment");
	  top::check(btageff->initialize(), "Failed to initialize "+btagWP);
	  // Check the excludedSysts - Cannot check before the tool is initialised
	  top::check(this->checkExcludedSysts(btageff, excludedSysts),
		     "Incorrect excluded systematics have been provided.");
          m_btagging_efficiency_tools.push_back(btageff);
          m_config->setBTagWP_calibrated_trkJet(bTagWPName);
        }
        m_config->setBTagWP_available_trkJet(bTagWPName);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode FlavorTaggingCPTools::checkExcludedSysts(BTaggingEfficiencyTool* btageff, std::string excludedSysts){
  // We pass the pointer to the btagging efficiency tool which is being created and also the excludedSysts string which will be used
  // If the string is empty, then nothing to check
  if (excludedSysts == "") return StatusCode::SUCCESS;
  // Split by a semi-colon delimiter and then check the individual syst strings against the list from the CDI
  std::vector<std::string> listOfExcludedSysts;
  top::tokenize(excludedSysts, listOfExcludedSysts, ";");
  ATH_MSG_INFO(" ------------------------------------------------ ");
  ATH_MSG_INFO(" ------------- EXPERIMENTAL FEATURE ------------- ");
  ATH_MSG_INFO(" ------ Please provide feedback to TopReco ------ ");
  ATH_MSG_INFO(" ------------- EXPERIMENTAL FEATURE ------------- ");
  ATH_MSG_INFO(" ------------------------------------------------ ");
  ATH_MSG_INFO(" AnalysisTop - Checking excludedSysts for flavour tagging EV");
  ATH_MSG_INFO(" This has been split on the semi-colon delimiter to find...");
  for (auto s : listOfExcludedSysts) ATH_MSG_INFO("... "+s);
  // Get the map(string, vector<string>) from the CDI tool
  // Don't care about the flavours (this will be handled in the CDI)
  std::vector<std::string> listOfScaleFactorSystematics;
  for (auto flavour : btageff->listScaleFactorSystematics(false) ){
    for (auto sys : flavour.second){
      listOfScaleFactorSystematics.push_back(sys);
    }
  }
  // Make this a unique set and then we need to check that all systematics provided by the user are expected by the CDI
  std::set<std::string> setOfExcludedSysts, setOfScaleFactorSystematics;

  for (auto sys : listOfExcludedSysts){
    if(setOfExcludedSysts.find(sys) == setOfExcludedSysts.end()) setOfExcludedSysts.insert(sys);
  }

  for (auto sys : listOfScaleFactorSystematics){
    if(setOfScaleFactorSystematics.find(sys) == setOfScaleFactorSystematics.end()) setOfScaleFactorSystematics.insert(sys);
  }

  // 
  std::vector<std::string> unionOfSystematics;
  std::set_intersection(setOfExcludedSysts.begin(), setOfExcludedSysts.end(),
			setOfScaleFactorSystematics.begin(), setOfScaleFactorSystematics.end(),
			std::back_inserter(unionOfSystematics));
  // Check we have the same systematics listed in unionOfSystematics
  if (unionOfSystematics.size() != listOfExcludedSysts.size()){
    ATH_MSG_WARNING("Have not found all systematics listed to be excluded from b-tagging eigenvector method");
    ATH_MSG_INFO("Permitted values are...");
    for(auto sys : setOfScaleFactorSystematics) {
      ATH_MSG_INFO(" ... " + sys );
    }
    return StatusCode::FAILURE;
  }
  else{
    ATH_MSG_INFO(" Summary of EV impact ");
    for(auto sysRemove : listOfExcludedSysts) {
      std::string flavourAffected = "";
      for (auto flavour : btageff->listScaleFactorSystematics(false) ){
        for (auto sysCDI : flavour.second){
          if (sysRemove == sysCDI) flavourAffected += flavour.first;
        }
      }
      ATH_MSG_INFO(" ... " + sysRemove + " -> Removed from calibration(s) : [" + flavourAffected + "]");
    }
    ATH_MSG_INFO(" These will be dynamically matched to systematic tree names (if available)");
    ATH_MSG_INFO(" All systematics are accepted by CDI file ");
  }
  // We have passed all the tests so now we store the systematics removed from the EV method and use a mapping to ASG/AT naming and return
  createExcludedSystMapping(unionOfSystematics);
  ATH_MSG_INFO(" ------------------------------------------------ ");
  return StatusCode::SUCCESS;
}

void FlavorTaggingCPTools::createExcludedSystMapping(std::vector<std::string> systematics){

}

}  // namespace top

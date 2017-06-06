/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopFlavorTaggingCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
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

  m_tagger = "MV2c10";
  m_cdi_file = "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-2017-04-24_v1.root";
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
    std::cout << "     No b-tagging calibration available for jet collection " << caloJets_collection << std::endl;
    std::cout << "     We'll use the calibration for AntiKt4EMTopoJets instead" << std::endl;
    std::cout << "     Be carefull!!" << std::endl;
    caloJets_collection = "AntiKt4EMTopoJets";
  }
  // check if the WP requested by the user are available, and if yes, initialize the tools
  // loop through all btagging WPs requested
  for (auto btagWP : m_config->bTagWP()) {
    if(std::find(m_calo_WPs.begin(), m_calo_WPs.end(), btagWP) == m_calo_WPs.end()) {
      ATH_MSG_WARNING("top::FlavorTaggingCPTools::initialize" );
      std::cerr << "     b-tagging WP: " << btagWP << " not supported for jet collection " << m_config->sgKeyJets() << std::endl;
      std::cerr << "     it will therefore be ignored" << std::endl;
    }
    else {
      //------------------------------------------------------------
      // Setup BTaggingSelectionTool
      //------------------------------------------------------------
      std::string btagsel_tool_name = "BTaggingSelectionTool_"+btagWP+"_"+m_config->sgKeyJets();
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
        std::cerr << "     b-tagging WP: " << btagWP << " is not calibrated for jet collection " << m_config->sgKeyJets() << std::endl;
        std::cerr << "     it will therefore be ignored for the scale-factors, although the tagging decisions will be saved" << std::endl;
      }
      else {
        //------------------------------------------------------------
        // Setup BTaggingEfficiencyTool
        //------------------------------------------------------------
        std::string btageff_tool_name = "BTaggingEfficiencyTool_"+btagWP+"_"+m_config->sgKeyJets();
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
        m_btagging_efficiency_tools.push_back(btageff);
        m_config->setBTagWP_calibrated(btagWP);
      }
      m_config->setBTagWP_available(btagWP);
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
        std::cerr << "     b-tagging WP: " << btagWP << " not supported for jet collection " << m_config->sgKeyTrackJets() << std::endl;
        std::cerr << "     it will therefore be ignored" << std::endl;
      }
      else {
        //------------------------------------------------------------
        // Setup BTaggingSelectionTool
        //------------------------------------------------------------
        std::string btagsel_tool_name = "BTaggingSelectionTool_"+btagWP+"_"+m_config->sgKeyTrackJets();
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
          std::cerr << "     b-tagging WP: " << btagWP << " is not calibrated for jet collection " << m_config->sgKeyTrackJets() << std::endl;
          std::cerr << "     it will therefore be ignored for the scale-factors, although the tagging decisions will be saved" << std::endl;
        }
        else {
          //------------------------------------------------------------
          // Setup BTaggingEfficiencyTool
          //------------------------------------------------------------
          std::string btageff_tool_name = "BTaggingEfficiencyTool_"+btagWP+"_"+m_config->sgKeyTrackJets();
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
          m_btagging_efficiency_tools.push_back(btageff);
          m_config->setBTagWP_calibrated_trkJet(btagWP);
        }
        m_config->setBTagWP_available_trkJet(btagWP);
      }
    }
  }
  return StatusCode::SUCCESS;
}

}  // namespace top

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagScaleFactorCalculator.cxx 802983 2017-04-16 18:31:29Z tpelzer $
#include "TopCorrections/BTagScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODJet/JetContainer.h"
#include <cmath>
#include <algorithm>
#include <functional>

namespace top{

  BTagScaleFactorCalculator::BTagScaleFactorCalculator( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),
    m_nominal( CP::SystematicSet() )
  {
    declareProperty( "config" , m_config );

  }

  StatusCode BTagScaleFactorCalculator::initialize()
  {
    ATH_MSG_INFO(" top::BTagScaleFactorCalculator initialize" );

    std::string release_series = "";
    const char* rel_temp = std::getenv("ROOTCORE_RELEASE_SERIES");
    if (rel_temp) {
      release_series = std::string(rel_temp);
    } else {
      ATH_MSG_WARNING("Can not access ROOTCORE_RELEASE_SERIES");
    }

    if (release_series == "23") {
      ATH_MSG_INFO("Setting release series to 2.3");
      m_release_series = 23;
    } else if (release_series == "24") {
      m_release_series = 24;
      ATH_MSG_INFO("Setting release series to 2.4");
    } else {
      ATH_MSG_WARNING("Unknown release series, assuming 2.4");
    }
    
    // for calo jets
    std::vector<std::string> availableWPs = m_config->bTagWP_available();
    for (auto& WP : availableWPs) {
      m_btagSelTools[WP] = "BTaggingSelectionTool_"+WP+"_"+m_config->sgKeyJets();
      top::check( m_btagSelTools[WP].retrieve(), "Failed to retrieve b-tagging Selection tool" );
      if (std::find(m_config->bTagWP_calibrated().begin(),
                    m_config->bTagWP_calibrated().end(), WP) != m_config->bTagWP_calibrated().end()) {// need scale-factors only for calibrated WPs
        m_btagEffTools[WP] = "BTaggingEfficiencyTool_"+WP+"_"+m_config->sgKeyJets();
        top::check( m_btagEffTools[WP].retrieve(), "Failed to retrieve b-tagging Efficiency tool" );
        m_systs[WP] = m_btagEffTools[WP]->affectingSystematics();
        std::set<std::string> base_names = m_systs[WP].getBaseNames();
        m_config->setBTaggingSFSysts( WP, base_names );
      }
    }
    // for track jets
    availableWPs = m_config->bTagWP_available_trkJet();
    for (auto& WP : availableWPs) {
      m_trkjet_btagSelTools[WP] = "BTaggingSelectionTool_"+WP+"_"+m_config->sgKeyTrackJets();
      top::check( m_trkjet_btagSelTools[WP].retrieve(), "Failed to retrieve b-tagging Selection tool" );
      if (std::find(m_config->bTagWP_calibrated_trkJet().begin(),
                    m_config->bTagWP_calibrated_trkJet().end(), WP) != m_config->bTagWP_calibrated_trkJet().end()) {// need scale-factors only for calibrated WPs
        m_trkjet_btagEffTools[WP] = "BTaggingEfficiencyTool_"+WP+"_"+m_config->sgKeyTrackJets();
        top::check( m_trkjet_btagEffTools[WP].retrieve(), "Failed to retrieve b-tagging Efficiency tool" );
        m_trkjet_systs[WP] = m_trkjet_btagEffTools[WP]->affectingSystematics();
        std::set<std::string> base_names = m_trkjet_systs[WP].getBaseNames();
        m_config->setBTaggingSFSysts( WP, base_names, true );
      }
    }
    
    return StatusCode::SUCCESS;
  }


  StatusCode BTagScaleFactorCalculator::execute()
  {

    top::check( apply( m_config->systSgKeyMapJets(false) ),
		"Failed to apply btagging SFs" );
    if( m_config->useTrackJets() )
      top::check( apply( m_config->systSgKeyMapTrackJets(), true ),
		  "Failed to apply track jet btagging SFs" );

    return StatusCode::SUCCESS;

  }

  StatusCode BTagScaleFactorCalculator::apply( const std::shared_ptr<std::unordered_map<std::size_t,std::string>>& jet_syst_collections, bool use_trackjets ){
    ///-- Loop over all jet collections --///
    ///-- Lets assume that we're not doing ElectronInJet subtraction --///
    for (auto currentSystematic : *jet_syst_collections) {
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets, currentSystematic.second), "failed to retrieve jets");

      ///-- Tell the SF tools to use the nominal systematic --///

      /// -- Loop over all jets in each collection --///
      for (auto jetPtr : *jets) {

	bool passSelection(false);
	if (jetPtr->isAvailable<char>("passPreORSelection")) {
	  if (jetPtr->auxdataConst<char>("passPreORSelection") == 1) {
	    passSelection = true;
	  }
	}
	if (jetPtr->isAvailable<char>("passPreORSelectionLoose")) {
	  if (jetPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
	    passSelection = true;
	  }
	}

	if (passSelection) {

	  // now loop over all available WPs

	  for( auto& tagWP : (use_trackjets ? m_config->bTagWP_available_trkJet() : m_config->bTagWP_available()) ){
            // skip uncalibrated though available WPs
            if (use_trackjets && 
                std::find(m_config->bTagWP_calibrated_trkJet().begin(),
                          m_config->bTagWP_calibrated_trkJet().end(), tagWP)
                          == m_config->bTagWP_calibrated_trkJet().end()) continue;
            else if (!use_trackjets && 
                std::find(m_config->bTagWP_calibrated().begin(),
                          m_config->bTagWP_calibrated().end(), tagWP)
                          == m_config->bTagWP_calibrated().end()) continue;
            ToolHandle<IBTaggingEfficiencyTool>& btageff = use_trackjets ? m_trkjet_btagEffTools[tagWP] : m_btagEffTools[tagWP];
            ToolHandle<IBTaggingSelectionTool>& btagsel = use_trackjets ? m_trkjet_btagSelTools[tagWP] : m_btagSelTools[tagWP];
            CP::SystematicSet& sysSet = use_trackjets ? m_trkjet_systs[tagWP] : m_systs[tagWP];

            // need now the DSID to find out which shower was used in the sample
            unsigned int MapIndex = m_config -> getMapIndex();

            btageff -> setMapIndex("Light", MapIndex);
            btageff -> setMapIndex("C",     MapIndex);
            btageff -> setMapIndex("B",     MapIndex);
            btageff -> setMapIndex("T",     MapIndex);

	    top::check( btageff->applySystematicVariation(m_nominal),
			"Failed to set new b-tagging SF to nominal" );

	    float btag_SF(1.0);
            bool  isTagged = false;//unused in case of Continuous
            if (std::fabs(jetPtr->eta()) < 2.5 ) {
              if (tagWP != "Continuous") {
                isTagged = btagsel->accept(*jetPtr);
                if(isTagged)
                  top::check( btageff->getScaleFactor(*jetPtr, btag_SF),
                              "Failed to get nominal b-tagging SF" );
                else
                  top::check( btageff->getInefficiencyScaleFactor(*jetPtr, btag_SF),
                              "Failed to get nominal b-tagging SF" );
              }
              else {
                top::check( btageff->getScaleFactor(*jetPtr, btag_SF),
                                "Failed to get nominal Continuous b-tagging SF" );
              }
            }
            jetPtr->auxdecor<float>("btag_SF_"+tagWP+"_nom") = btag_SF;

	    ///-- For nominal calibration, vary the SF systematics --///
	    if (currentSystematic.first == m_config->nominalHashValue()) {

	      for( const auto& variation : sysSet ) {
                btag_SF = 1.;
                CP::SystematicSet syst_set;
                syst_set.insert( variation );
                top::check( btageff->applySystematicVariation(syst_set),
                            "Failed to set new b-tagging systematic variation "+syst_set.name() );
                if (std::fabs(jetPtr->eta()) < 2.5 ) {
                  if (tagWP != "Continuous") {
                    if (isTagged)
                      top::check( btageff->getScaleFactor(*jetPtr, btag_SF),
                                  "Failed to get b-tagging SF for variation "+syst_set.name() );
                    else
                      top::check( btageff->getInefficiencyScaleFactor(*jetPtr, btag_SF),
                                  "Failed to get b-tagging SF for variation "+syst_set.name() );
                  }
                  else {
                    top::check( btageff->getScaleFactor(*jetPtr, btag_SF),
                                "Failed to get Continuous b-tagging SF for variation "+syst_set.name() );
                  }
                }
                jetPtr->auxdecor<float>( "btag_SF_"+tagWP+"_"+variation.name() ) = btag_SF;
              } // loop through b-tagging systematic variations
	    } // Calibration systematic is nominal, so calculate SF systematics

	  }
	}
      }
    }


    return StatusCode::SUCCESS;
  }

}

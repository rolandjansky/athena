/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to <OBJECT>
// in the SUSYObjDef_xAOD class

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetSelector.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetModifier.h"
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"

#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

#include "METUtilities/METHelpers.h"

#include "AthContainers/ConstDataVector.h"
#include "FourMomUtils/xAODP4Helpers.h"
#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {

  const static SG::AuxElement::Decorator<char>     dec_bad("bad");
  const static SG::AuxElement::ConstAccessor<char> acc_bad("bad");

  const static SG::AuxElement::Decorator<char>      dec_passJvt("passJvt");
  const static SG::AuxElement::ConstAccessor<char>  acc_passJvt("passJvt");
  const static SG::AuxElement::ConstAccessor<char>  acc_passFJvt("passFJvt");

  const static SG::AuxElement::Decorator<float> dec_jvt("Jvt");
  const static SG::AuxElement::ConstAccessor<float> acc_jvt("Jvt");
  const static SG::AuxElement::Decorator<float> dec_fjvt("fJvt");
  const static SG::AuxElement::ConstAccessor<float> acc_fjvt("fJvt");

  const static SG::AuxElement::Decorator<char> dec_bjet("bjet");
  const static SG::AuxElement::ConstAccessor<char> acc_bjet("bjet");

  const static SG::AuxElement::Decorator<char> dec_bjet_jetunc("bjet_jetunc"); //added for JetUncertainties usage
  const static SG::AuxElement::Decorator<char> dec_bjet_loose("bjet_loose");

  const static SG::AuxElement::Decorator<double> dec_btag_weight("btag_weight");
  const static SG::AuxElement::Decorator<double> dec_btag_dl1pb("btag_dl1pb"); // added for dl1 components
  const static SG::AuxElement::Decorator<double> dec_btag_dl1pc("btag_dl1pc");
  const static SG::AuxElement::Decorator<double> dec_btag_dl1pu("btag_dl1pu");

  const static SG::AuxElement::Decorator<float> dec_VRradius("VRradius");
  const static SG::AuxElement::ConstAccessor<float> acc_VRradius("VRradius");

  const static SG::AuxElement::Decorator<char> dec_passDRcut("passDRcut");
  const static SG::AuxElement::ConstAccessor<char> acc_passDRcut("passDRcut");

  StatusCode SUSYObjDef_xAOD::GetJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& jetkey, const xAOD::JetContainer* containerToBeCopied)
  {
    if (!m_tool_init) {
      ATH_MSG_ERROR("SUSYTools was not initialized!!");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Default jetkey:           " << m_defaultJets);
    ATH_MSG_DEBUG("Function argument jetkey: " << jetkey);
    ATH_MSG_DEBUG("Config Btag.TimeStamp:    " << m_BtagTimeStamp);
    ATH_MSG_DEBUG("Config Btag.KeyOverride:  " << m_BtagKeyOverride);

    // load default regular & btag jet keys
    std::string jetkey_tmp = m_defaultJets;                                           // use default for regular jetkey_tmp
    std::string jetkey_tmp_btag = jetkey_tmp;                                         // as well as btag jetkey_tmp_btag
    if (!m_BtagTimeStamp.empty()) jetkey_tmp_btag += "_BTagging"+m_BtagTimeStamp;     // add default Btag.TimeStamp for jetkey_tmp_btag if given

    // override default if user is passing a jetkey
    if (!jetkey.empty()) {
      jetkey_tmp = jetkey;
      jetkey_tmp_btag = "";
      if (jetkey_tmp.find("_BTagging")!=std::string::npos) {
        ATH_MSG_WARNING("Time-stamped container key " << jetkey_tmp << " is given. These containers only contain b-tagging information and cannot be used for calibration. " <<
        "Will remove the time stamp and use the key to attach the b-tagging information to the actual container");
        if (m_BtagTimeStamp.empty()) jetkey_tmp_btag = jetkey_tmp;                              // copy time-stamped jetkey to jetkey_tmp_btag if no Btag.TimeStamp given
        jetkey_tmp = jetkey_tmp.substr(0, jetkey_tmp.find("_BTagging"));                        // remove time stamp from regular jetkey_tmp
        if (jetkey_tmp_btag.empty()) jetkey_tmp_btag = jetkey_tmp+"_BTagging"+m_BtagTimeStamp;  // use default Btag.TimeStamp for jetkey_tmp_btag if given
      }
    }
    // override if user has set a Btag.KeyOverride
    if (!m_BtagKeyOverride.empty()) {
       jetkey_tmp_btag = m_BtagKeyOverride;
       ATH_MSG_DEBUG("B-tagging jetkey override active: " << m_BtagKeyOverride);
    }

    // final settings
    ATH_MSG_DEBUG("Key for retrieving jet collection:             jetkey      = " << jetkey_tmp);
    ATH_MSG_DEBUG("Key for retrieving jet collection (bjet info): jetkey_btag = " << jetkey_tmp_btag);

    const xAOD::JetContainer* jets(0);
    if (copy==NULL) { // empty container provided
      if (containerToBeCopied != nullptr) {
        jets = containerToBeCopied;
      }
      else {
        ATH_MSG_DEBUG("Retrieve jet collection: " << jetkey_tmp);
        ATH_CHECK( evtStore()->retrieve(jets, jetkey_tmp) );
      }
      std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*jets);
      copy = shallowcopy.first;
      copyaux = shallowcopy.second;
      bool setLinks = xAOD::setOriginalObjectLink(*jets, *copy);
      if (!setLinks) {
        ATH_MSG_WARNING("Failed to set original object links on " << jetkey_tmp);
      }
    } else { // use the user-supplied collection instead
      ATH_MSG_DEBUG("Not retrieving jet collecton, using existing one provided by user");
      jets = copy;
    }
    // Copy btagging links
    if(m_useBtagging && !jetkey_tmp_btag.empty()){
      ATH_CHECK(BendBTaggingLinks(copy, jetkey_tmp_btag));
    }
    // Update the jets
    for (const auto& jet : *copy) {
      // Note that for PHYSLITE jets we don't need the nominal calibration
      ATH_CHECK( this->FillJet(*jet, jetkey!="AnalysisJets") );
    }
    // Tool requires a loop over all jets
    if (m_doFwdJVT || m_treatPUJets){
      m_jetFwdJvtTool->modify(*copy); //compute FwdJVT for all jets
    }

    for (const auto& jet : *copy) {
      // Update the JVT decorations if needed
      if( m_doFwdJVT && fabs(acc_DetEta(*jet)) > m_fJvtEtaMin ){
        dec_passJvt(*jet) = acc_passFJvt(*jet) && acc_passJvt(*jet);
        dec_fjvt(*jet) = acc_fjvt(*jet);

        //new state for OR   .  0=non-baseline objects, 1=for baseline jets not passing JVT, 2=for any other baseline object
        if ( acc_baseline(*jet) ){
          if( acc_passJvt(*jet) )     dec_selected(*jet) = 2;
          else                        dec_selected(*jet) = 1;
        }
        else{
          dec_selected(*jet) = 0;
        }
      }
      this->IsBadJet(*jet);
      this->IsSignalJet(*jet, m_jetPt, m_jetEta);
      if (!isData())this->IsTruthBJet(*jet);
    }
    if (recordSG) {
      std::string auxname = copyaux->name();
      if (auxname.compare("UNKNOWN")==0) copyaux->setName(std::string("STCalib" + jetkey_tmp + m_currentSyst.name() + "AuxCopy").c_str());
      ATH_CHECK( evtStore()->record(copy, "STCalib" + jetkey_tmp + m_currentSyst.name()) );
      ATH_CHECK( evtStore()->record(copyaux, "STCalib" + jetkey_tmp + m_currentSyst.name() + "Aux.") );
    }
    return StatusCode::SUCCESS;
  }

  StatusCode SUSYObjDef_xAOD::GetTrackJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& jetkey, const xAOD::JetContainer* containerToBeCopied)
  {
    if (!m_tool_init) {
      ATH_MSG_ERROR("SUSYTools was not initialized!!");
      return StatusCode::FAILURE;
    }

    // ! no specific btagging collection key is required for trackjets
    // because the btagging info doesn't need to be linked from a different container
    // 
    ATH_MSG_DEBUG("Default jetkey (trkjet):           " << m_defaultTrackJets);
    ATH_MSG_DEBUG("Function argument jetkey (trkjet): " << jetkey);
    ATH_MSG_DEBUG("Config Btag.TimeStamp (trkjet):    " << m_BtagTimeStamp_trkJet);
    ATH_MSG_DEBUG("Config Btag.KeyOverride (trkjet):  " << m_BtagKeyOverride);

    // load default regular & btag jet keys
    std::string jetkey_tmp = m_defaultTrackJets;                                                  // use default for jetkey_tmp
    if (!m_BtagTimeStamp_trkJet.empty()) jetkey_tmp += "_BTagging"+m_BtagTimeStamp_trkJet;        // add default Btag.TimeStamp for jetkey_tmp if given

    // override default if user is passing a jetkey
    if (!jetkey.empty()) {
      jetkey_tmp = jetkey;
      if (jetkey_tmp.find("_BTagging")==std::string::npos && !m_BtagTimeStamp_trkJet.empty()) {
        ATH_MSG_WARNING("Non time-stamped container key " << jetkey_tmp << " is given for trk jets, but central timestamp " << m_BtagTimeStamp_trkJet << " is defined." << 
              "Will add the time stamp and use the key to collect container and b-tagging information.");
        jetkey_tmp += "_BTagging"+m_BtagTimeStamp_trkJet;                               // timestamped jetkey_tmp
      }
    }
    // override if user has set a Btag.KeyOverride
    if (!m_BtagKeyOverride.empty()) {
       jetkey_tmp = m_BtagKeyOverride;
       ATH_MSG_DEBUG("B-tagging jetkey override active: " << m_BtagKeyOverride);
    }

    // final settings
    ATH_MSG_DEBUG("Key for retrieving trkjet collection (as well as bjet info):      jetkey      = " << jetkey_tmp);

    const xAOD::JetContainer* jets(0);
    if (copy==NULL) { // empty container provided
      if (containerToBeCopied != nullptr) {
        jets = containerToBeCopied;
      }
      else {
        ATH_MSG_DEBUG("Retrieve jet collection: " << jetkey_tmp);
        ATH_CHECK( evtStore()->retrieve(jets, jetkey_tmp) );
      }
      std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*jets);
      copy = shallowcopy.first;
      copyaux = shallowcopy.second;
      bool setLinks = xAOD::setOriginalObjectLink(*jets, *copy);
      if (!setLinks) {
        ATH_MSG_WARNING("Failed to set original object links on " << jetkey_tmp);
      }
    } else { // use the user-supplied collection instead
      ATH_MSG_DEBUG("Not retrieving jet collection, using existing one provided by user");
      jets = copy;
    }

    // Update the jets
    for (const auto& jet : *copy) {
      ATH_CHECK( this->FillTrackJet(*jet) );
    }

    if (copy->size() > 1 && m_defaultTrackJets == "AntiKtVR30Rmax4Rmin02TrackJets") {
        // Use iterators to avoid pairing the jets twice
        for (xAOD::JetContainer::const_iterator j1 = copy->begin()+1; j1!= copy->end();++j1) {
          const xAOD::Jet* jet1 = (*j1);
          if (!acc_signal(*jet1)) continue;
          for (xAOD::JetContainer::const_iterator j2 = copy->begin(); j2 != j1; ++j2) {
            const xAOD::Jet* jet2 = (*j2);
            if (!acc_baseline(*jet2)) continue;
            //Reference to the use method in P4Helper: deltaR2( const xAOD::IParticle& p4, const xAOD::IParticle& , bool useRapidity=true )
            float dr_jets =  xAOD::P4Helpers::deltaR(jet1,jet2, false);
            const xAOD::Jet* to_check = acc_VRradius(*jet1) < acc_VRradius(*jet2) ? jet1 : jet2;
            if( dr_jets < acc_VRradius(*to_check)) dec_passDRcut(*to_check) = false;
            //break the loop at this point???
          }
        }
    }
    if (recordSG) {
      ATH_CHECK( evtStore()->record(copy, "STCalib" + jetkey_tmp + m_currentSyst.name()) );
      ATH_CHECK( evtStore()->record(copyaux, "STCalib" + jetkey_tmp + m_currentSyst.name() + "Aux.") );
    }
    return StatusCode::SUCCESS;
  }

  StatusCode SUSYObjDef_xAOD::GetFatJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& jetkey, const bool doLargeRdecorations, const xAOD::JetContainer* containerToBeCopied)
  {
    if (!m_tool_init) {
      ATH_MSG_ERROR("SUSYTools was not initialized!!");
      return StatusCode::FAILURE;
    }

    if (m_fatJets.empty()) {
      ATH_MSG_ERROR("JetFatCalibTool was not initialized for largeR jet!!");
      return StatusCode::FAILURE;
    }

    std::string jetkey_tmp = jetkey;
    if (jetkey.empty()) {
      jetkey_tmp = m_fatJets;
    }

    const xAOD::JetContainer* jets(0);
    if (copy==NULL) { // empty container provided
      if (containerToBeCopied != nullptr) {
        jets = containerToBeCopied;
      }
      else {
        ATH_MSG_DEBUG("Retrieve jet collection: " << jetkey_tmp);
        ATH_CHECK( evtStore()->retrieve(jets, jetkey_tmp) );
      }
      std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*jets);
      copy = shallowcopy.first;
      copyaux = shallowcopy.second;
      bool setLinks = xAOD::setOriginalObjectLink(*jets, *copy);
      if (!setLinks) {
        ATH_MSG_WARNING("Failed to set original object links on " << jetkey_tmp);
      }
    } else { // use the user-supplied collection instead
      ATH_MSG_DEBUG("Not retrieving jet collection, using existing one provided by user");
      jets=copy;
    }

    for (const auto& jet : *copy) {
      ATH_CHECK( this->FillJet(*jet, true, true, true) );
      //...
      const static SG::AuxElement::Decorator<int> dec_wtagged("wtagged");
      const static SG::AuxElement::Decorator<int> dec_ztagged("ztagged");
      const static SG::AuxElement::Decorator<int> dec_toptagged("toptagged");
      dec_wtagged(*jet) = -1;
      dec_ztagged(*jet) = -1;
      dec_toptagged(*jet) = -1;
      if ( doLargeRdecorations) {
        if (!m_WtagConfig.empty()) dec_wtagged(*jet) = m_WTaggerTool->tag(*jet);
        if (!m_ZtagConfig.empty()) dec_ztagged(*jet) = m_ZTaggerTool->tag(*jet);
        if (!m_ToptagConfig.empty()) dec_toptagged(*jet) = m_TopTaggerTool->tag(*jet);
      }
      //  For OR, selected if it passed cuts
      if ( acc_baseline(*jet) ){
        dec_selected(*jet) = 1;
      }
      else{
        dec_selected(*jet) = 0;
      }
      // Truth Labeling (MC only)
      m_jetTruthLabelingTool->modifyJet(*jet);
    }
    if (recordSG) {
      ATH_CHECK( evtStore()->record(copy, "STCalib" + jetkey_tmp + m_currentSyst.name()) );
      ATH_CHECK( evtStore()->record(copyaux, "STCalib" + jetkey_tmp + m_currentSyst.name() + "Aux.") );
    }
    return StatusCode::SUCCESS;
  }


  StatusCode SUSYObjDef_xAOD::GetJetsSyst(const xAOD::JetContainer& calibjets, xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& jetkey)
  {
    if (!m_tool_init) {
      ATH_MSG_ERROR("SUSYTools was not initialized!!");
      return StatusCode::FAILURE;
    }

    std::string jetkey_tmp = jetkey;
    if (jetkey.empty()) {
      jetkey_tmp = m_defaultJets;
    }

    std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(calibjets);
    copy = shallowcopy.first;
    copyaux = shallowcopy.second;

    bool setLinks = xAOD::setOriginalObjectLink(calibjets, *copy);
    if (!setLinks) {
      ATH_MSG_WARNING("Failed to set original object links on " << jetkey_tmp);
    }

    // ghost associate the muons to the jets (needed by MET muon-jet OR later)
    ATH_MSG_VERBOSE("Run muon-to-jet ghost association");
    const xAOD::MuonContainer* muons(0);
    // Do a little guessing
    if (jetkey!="AnalysisJets"){
      ATH_CHECK( evtStore()->retrieve(muons, "Muons") );
    } else {
      ATH_CHECK( evtStore()->retrieve(muons, "AnalysisMuons") );
    }
    met::addGhostMuonsToJets(*muons, *copy);

    // Update the jets
    for (const auto& jet : *copy) {
      ATH_CHECK( this->FillJet(*jet, false) );
    }
    // Tool requires a loop over all jets
    if (m_doFwdJVT || m_treatPUJets){
      m_jetFwdJvtTool->modify(*copy); //compute FwdJVT for all jets
    }

    for (const auto& jet : *copy) {
      // Update the JVT decorations if needed
      if( m_doFwdJVT && fabs(acc_DetEta(*jet)) > m_fJvtEtaMin ){
        dec_passJvt(*jet) = acc_passFJvt(*jet) && acc_passJvt(*jet);

        //new state for OR   .  0=non-baseline objects, 1=for baseline jets not passing JVT, 2=for any other baseline object
        if ( acc_baseline(*jet) ){
          if( acc_passJvt(*jet) )     dec_selected(*jet) = 2;
          else                        dec_selected(*jet) = 1;
        }
        else{
          dec_selected(*jet) = 0;
        }
      }
      this->IsBadJet(*jet);
      this->IsSignalJet(*jet, m_jetPt, m_jetEta);
      if (!isData())this->IsTruthBJet(*jet);
    }
    if (recordSG) {
      ATH_CHECK( evtStore()->record(copy, "STCalib" + jetkey_tmp + m_currentSyst.name()) );
      ATH_CHECK( evtStore()->record(copyaux, "STCalib" + jetkey_tmp + m_currentSyst.name() + "Aux.") );
    }
    return StatusCode::SUCCESS;
  }

  StatusCode SUSYObjDef_xAOD::FillJet(xAOD::Jet& input, bool doCalib, bool isFat, bool isTCC) {

    ATH_MSG_VERBOSE( "Starting FillJet on jet with pt=" << input.pt() );
    ATH_MSG_VERBOSE(  "jet (pt,eta,phi) before calibration " << input.pt() << " " << input.eta() << " " << input.phi() );

    if (doCalib) {
      if(!isFat){
        ATH_CHECK( m_jetCalibTool->applyCalibration(input) );
      }
      else{
        ATH_CHECK( m_jetFatCalibTool->applyCalibration(input) );
	dec_baseline(input) = ( input.pt() > m_jetPt ) || ( input.pt() > 20e3 ); // Allows for setting m_jetPt < 20e3
        dec_bad(input) = false;
        dec_signal(input) = false;
        dec_bjet_loose(input) = false;
        dec_effscalefact(input) = 1.;
        dec_passOR(input) = true;
        dec_bjet_jetunc(input) = false;
        dec_btag_weight(input) = -999.;

        // If a user hasn't specified an uncertainty config, then this tool will be empty
        // for large R jets
        if (!m_fatjetUncertaintiesTool.empty()){
          CP::CorrectionCode result = m_fatjetUncertaintiesTool->applyCorrection(input);
          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( "Failed to apply largeR jet scale uncertainties.");
            return StatusCode::FAILURE;
            //break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( "No valid pt/eta/m range for largeR jet scale uncertainties. ");
            break;
          default:
            break;
          }
        } else {
          ATH_MSG_DEBUG( "No valid fat jet uncertainty, but FillJet called with a fat jet. Skipping uncertainties." );
        }

        // for TCC jets
        if (!m_TCCjetUncertaintiesTool.empty() && isTCC){
          CP::CorrectionCode result = m_TCCjetUncertaintiesTool->applyCorrection(input);
          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( "Failed to apply TCC jet scale uncertainties.");
            return StatusCode::FAILURE;
            //break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( "No valid pt/eta/m range for TCC jet scale uncertainties. ");
            break;
          default:
            break;
          }
        } else {
          ATH_MSG_DEBUG( "No valid TCC jet uncertainty, but FillJet called with a TCC jet. Skipping uncertainties." );
        }

        return StatusCode::SUCCESS;
      }
      ATH_MSG_VERBOSE(  "jet (pt,eta,phi) after calibration " << input.pt() << " " << input.eta() << " " << input.phi() );

      //central jvt
      float jvt = m_jetJvtUpdateTool->updateJvt(input);
      ATH_MSG_VERBOSE("JVT recalculation: "
                      << acc_jvt(input) << " (before)"
                      << jvt << " (after)");

      dec_jvt(input) = jvt;
    }

    dec_passOR(input) = true;
    dec_bjet_jetunc(input) = false;

    if (m_useBtagging) {
      if (m_BtagWP != "Continuous") this->IsBJet(input);
      else this->IsBJetContinuous(input);
    }

    if ( (input.pt() > m_jetPt) || (input.pt() > 15e3) ) {
      if(!isFat && m_currentSyst.name().find("__2") == std::string::npos){
	// Use the normal jet uncertainties tool for this systematic and do not use the PDSmeared initialised tool
	CP::CorrectionCode result = m_jetUncertaintiesTool->applyCorrection(input);
	switch (result) {
        case CP::CorrectionCode::Error:
          ATH_MSG_ERROR( "Failed to apply JES correction" );
          break;
        case CP::CorrectionCode::OutOfValidityRange:
          ATH_MSG_WARNING( "JES correction OutOfValidity range."); // Jet (pt,eta,phi) = (" << input.pt() << ", " << input.eta() << ", " << input.phi() << ")");
          break;
        default:
	  break;
        }
      }
    }

    if ( (input.pt() > m_jetPt) || (input.pt() > 15e3) ) {
      if(!isFat && m_currentSyst.name().find("__2") != std::string::npos){
	// Use the PDSmeared uncertainties tool on the systematic with PDsmear in the name
	CP::CorrectionCode result = m_jetUncertaintiesPDSmearTool->applyCorrection(input);
        switch (result) {
        case CP::CorrectionCode::Error:
          ATH_MSG_ERROR( "Failed to apply JES correction" );
          break;
        case CP::CorrectionCode::OutOfValidityRange:
          ATH_MSG_WARNING( "JES correction OutOfValidity range."); // Jet (pt,eta,phi) = (" << input.pt() << ", " << input.eta() << ", " << input.phi() << ")");
          break;
        default:
          break;
        }
      }
    }


    ATH_MSG_VERBOSE(  "jet (pt,eta,phi) after JES correction " << input.pt() << " " << input.eta() << " " << input.phi() );

    dec_passJvt(input) = !m_applyJVTCut || m_jetJvtEfficiencyTool->passesJvtCut(input);
    dec_baseline(input) = ( input.pt() > m_jetPt ) || ( input.pt() > 20e3 ); // Allows for setting m_jetPt < 20e3
    dec_bad(input) = false;
    dec_signal_less_JVT(input) = false;
    dec_signal(input) = false;
    dec_bjet_loose(input) = false;
    dec_effscalefact(input) = 1.;

    //new state for OR   .  0=non-baseline objects, 1=for baseline jets not passing JVT, 2=for any other baseline object
    if (acc_baseline(input) ){
      if( acc_passJvt(input) ) dec_selected(input) = 2;
      else                     dec_selected(input) = 1;
    }
    else{
      dec_selected(input) = 0;
    }

    if (m_useBtagging && !m_orBtagWP.empty()) {
      dec_bjet_loose(input) = this->IsBJetLoose(input);
    }

    if (m_debug) {
      ATH_MSG_INFO( "JET pt: " << input.pt() );
      ATH_MSG_INFO( "JET eta: " << input.eta() );
      ATH_MSG_INFO( "JET phi: " << input.phi() );
      ATH_MSG_INFO( "JET E: " << input.e() );
      ATH_MSG_INFO( "JET Ceta: " << input.jetP4(xAOD::JetConstitScaleMomentum).eta() );
      ATH_MSG_INFO( "JET Cphi: " << input.jetP4(xAOD::JetConstitScaleMomentum).phi() );
      ATH_MSG_INFO( "JET CE: " << input.jetP4(xAOD::JetConstitScaleMomentum).e() );
      ATH_MSG_INFO( "JET Cm: " << input.jetP4(xAOD::JetConstitScaleMomentum).M() ); // fix-me M

    }

    return StatusCode::SUCCESS;
  }

  StatusCode SUSYObjDef_xAOD::FillTrackJet(xAOD::Jet& input) {

    ATH_MSG_VERBOSE( "Starting FillTrackJet on jet with pt=" << input.pt() );

    dec_btag_weight(input) = -999.;
    dec_effscalefact(input) = 1.;

    if (m_defaultTrackJets == "AntiKtVR30Rmax4Rmin02TrackJets") {
      // VR recommendation
      // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalib2017#Recommendations_for_variable_rad
      dec_baseline(input) = input.pt() >= 5e3 && input.numConstituents() >= 2;
      if (m_trkJetPt < 10e3)
        ATH_MSG_WARNING ("The pt threshold of VR jets you set is: " << m_trkJetPt/1000. << " GeV. But VR jets with pt < 10GeV are uncalibrated.");
      dec_signal(input) = acc_baseline(input) && input.pt() >= m_trkJetPt && fabs(input.eta()) <= m_trkJetEta;
      dec_VRradius(input) = std::max(0.02,std::min(0.4,30000./input.pt()));
      dec_passDRcut(input) = acc_signal(input);
    } else {
      dec_baseline(input) = input.pt() >= m_trkJetPt && fabs(input.eta()) <= m_trkJetEta;
      dec_signal(input) = acc_baseline(input);
    }

    if (m_useBtagging_trkJet) {
      if (m_BtagWP_trkJet != "Continuous") this->IsTrackBJet(input);
      else this->IsTrackBJetContinuous(input);
    }

    if (m_debug) {
      ATH_MSG_INFO( "TRK JET pt: " << input.pt() );
      ATH_MSG_INFO( "TRK JET eta: " << input.eta() );
      ATH_MSG_INFO( "TRK JET phi: " << input.phi() );
      ATH_MSG_INFO( "TRK JET E: " << input.e() );
    }

    return StatusCode::SUCCESS;
  }


  bool SUSYObjDef_xAOD::IsBJetLoose(const xAOD::Jet& input) const {
    bool isbjet_loose = false;
    if (m_orBJetPtUpperThres < 0 || m_orBJetPtUpperThres > input.pt())
      isbjet_loose = m_btagSelTool_OR->accept(input); //note : b-tag applies only to jet with eta < 2.5
    return isbjet_loose;
  }

  bool SUSYObjDef_xAOD::JetPassJVT(xAOD::Jet& input, bool update_jvt) {
    if(update_jvt){
      dec_jvt(input) = m_jetJvtUpdateTool->updateJvt(input);
    }

    char pass_jvt = !m_applyJVTCut || m_jetJvtEfficiencyTool->passesJvtCut(input);
    dec_passJvt(input) = pass_jvt;
    return pass_jvt;
  }

  bool SUSYObjDef_xAOD::IsSignalJet(const xAOD::Jet& input, float ptcut, float etacut) const {
    if ( !acc_baseline(input)  || !acc_passOR(input) ) return false;

    if ( input.pt() <= ptcut || fabs(input.eta()) >= etacut) return false;

    bool isgoodjet = !acc_bad(input) && acc_passJvt(input);

    dec_signal(input) = isgoodjet;

    // For JVT calculation
    dec_signal_less_JVT(input) = !acc_bad(input);


    if (m_debug) {
      float emfrac, hecf, LArQuality, HECQuality, Timing,  fracSamplingMax, NegativeE, AverageLArQF;
      std::vector<float> sumpttrk_vec;

      input.getAttribute(xAOD::JetAttribute::EMFrac, emfrac);
      input.getAttribute(xAOD::JetAttribute::HECFrac, hecf);
      input.getAttribute(xAOD::JetAttribute::LArQuality, LArQuality);
      input.getAttribute(xAOD::JetAttribute::HECQuality, HECQuality);
      input.getAttribute(xAOD::JetAttribute::Timing, Timing);
      input.getAttribute(xAOD::JetAttribute::SumPtTrkPt500, sumpttrk_vec);
      input.getAttribute(xAOD::JetAttribute::FracSamplingMax, fracSamplingMax);
      input.getAttribute(xAOD::JetAttribute::NegativeE, NegativeE);
      input.getAttribute(xAOD::JetAttribute::AverageLArQF, AverageLArQF);

      float sumpttrk;
      if (sumpttrk_vec.size() > 0 && this->GetPrimVtx()) {
        sumpttrk = sumpttrk_vec[this->GetPrimVtx()->index()];
      } else {
        sumpttrk = 0;
      }

      ATH_MSG_INFO( "JET pt: " << input.pt() );
      ATH_MSG_INFO( "JET eta: " << input.eta() );
      ATH_MSG_INFO( "JET emfrac: " << emfrac );
      ATH_MSG_INFO( "JET hecfrac: " << hecf );
      ATH_MSG_INFO( "JET LArQuality: " << LArQuality );
      ATH_MSG_INFO( "JET HECQuality: " << HECQuality );
      ATH_MSG_INFO( "JET Timing: " << Timing );
      ATH_MSG_INFO( "JET sumpttrk: " << sumpttrk );
      ATH_MSG_INFO( "JET fracSamplingMax: " << fracSamplingMax );
      ATH_MSG_INFO( "JET AverageLArQF: " << AverageLArQF );
    }

    ATH_MSG_VERBOSE( "JET isbad?: " << (int) acc_bad(input) );

    return isgoodjet;
  }


  bool SUSYObjDef_xAOD::IsBadJet(const xAOD::Jet& input) const {

    if ( !acc_passOR(input) ) return false;

    float ptcut = 20e3;
    if ( m_jetPt < ptcut ) ptcut = m_jetPt;

    bool  isPileup = !acc_passJvt(input);

    if ( input.pt() <= ptcut || isPileup ) return false;

    if (m_jetInputType == xAOD::JetInput::EMTopo) { //--- Jet cleaning only well defined for EMTopo jets!
      if (m_acc_jetClean.isAvailable(input)) {
	dec_bad(input) = !m_acc_jetClean(input);
      } else {
	ATH_MSG_VERBOSE("DFCommon jet cleaning variable not available ... Using jet cleaning tool");
	dec_bad(input) = m_jetCleaningTool.empty() ? false : !m_jetCleaningTool->keep(input);
      }
    }
    else {
      dec_bad(input) = false;
      ATH_MSG_VERBOSE("Jet cleaning is available only for EMTopo jet collection (InputType == 1), your jet collection: " << m_jetInputType );
    }

    ATH_MSG_VERBOSE( "JET isbad?: " << (int) acc_bad(input) );

    return acc_bad(input);
  }


  bool SUSYObjDef_xAOD::IsBJet(const xAOD::Jet& input) const {

    bool isbjet = m_btagSelTool->accept(input);
    dec_bjet(input) = isbjet;

    ATH_CHECK( SetBtagWeightDecorations(input, m_btagSelTool, m_BtagTagger) );

    return isbjet;
  }

  bool SUSYObjDef_xAOD::IsTrackBJet(const xAOD::Jet& input) const {

    bool isbjet = m_btagSelTool_trkJet->accept(input);
    dec_bjet(input) = isbjet;

    ATH_CHECK( SetBtagWeightDecorations(input, m_btagSelTool_trkJet, m_BtagTagger_trkJet) );

    return isbjet;
  }

  int SUSYObjDef_xAOD::IsBJetContinuous(const xAOD::Jet& input) const {
    //////////////////////
    // Cheatsheet:
    // returns 5 if between 60% and 0%
    // returns 4 if between 70% and 60%
    // returns 3 if between 77% and 70%
    // returns 2 if between 85% and 77%
    // returns 1 if between 100% and 85%
    // returns 0 if smaller than -1e4-> should never happen
    // return -1 if bigger than 1e4 or not in b-tagging acceptance
    //////////////////////

    int isbjet = m_btagSelTool->getQuantile(input);
    dec_bjet(input) = isbjet;

    ATH_CHECK( SetBtagWeightDecorations(input, m_btagSelTool, m_BtagTagger) );

    return isbjet;
  }

  int SUSYObjDef_xAOD::IsTrackBJetContinuous(const xAOD::Jet& input) const {

    int isbjet = m_btagSelTool_trkJet->getQuantile(input);
    dec_bjet(input) = isbjet;

    ATH_CHECK( SetBtagWeightDecorations(input, m_btagSelTool_trkJet, m_BtagTagger_trkJet) );

    return isbjet;
  }

  float SUSYObjDef_xAOD::BtagSF(const xAOD::JetContainer* jets) const {

    float totalSF = 1.;
    for ( const xAOD::Jet* jet : *jets ) {

      float sf = 1.;

      if ( fabs(jet->eta()) > 2.5 ) {
        ATH_MSG_VERBOSE( "Trying to retrieve b-tagging SF for jet with |eta|>2.5 (jet eta=" << jet->eta() << "), jet will be skipped");
      } else if ( jet->pt() < 20e3 ){
        ATH_MSG_VERBOSE( "Trying to retrieve b-tagging SF for jet with invalid pt (jet pt=" << jet->pt() << "), jet will be skipped");
      } else {

        CP::CorrectionCode result;
        int truthlabel(-1);
        if (!jet->getAttribute("HadronConeExclTruthLabelID", truthlabel)) {
          ATH_MSG_ERROR("Failed to get jet truth label!");
        }
        ATH_MSG_VERBOSE("This jet is " << (acc_bjet(*jet) ? "" : "not ") << "b-tagged.");
        ATH_MSG_VERBOSE("This jet's truth label is " << truthlabel);

        if ( acc_bjet(*jet) ) {
          result = m_btagEffTool->getScaleFactor(*jet, sf);

          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( "Failed to retrieve SF for b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( "No valid SF for b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          default:
            ATH_MSG_VERBOSE( "Retrieve SF for b-tagged jets in SUSYTools_xAOD::BtagSF with value " << sf );
          }
        } else {

          result = m_btagEffTool->getInefficiencyScaleFactor(*jet, sf);

          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( "Failed to retrieve SF for non-b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( "No valid inefficiency SF for non-b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          default:
            ATH_MSG_VERBOSE( "Retrieve SF for non-b-tagged jets in SUSYTools_xAOD::BtagSF with value " << sf );
          }
        }
      }

      dec_effscalefact(*jet) = sf;

      if( acc_signal(*jet) && acc_passOR(*jet) ) totalSF *= sf; //consider goodjets only

    }

    return totalSF;

  }


  float SUSYObjDef_xAOD::BtagSFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig)
  {

    float totalSF = 1.;

    //Set the new systematic variation
    CP::SystematicCode ret = m_btagEffTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure BTaggingEfficiencyTool for systematic var. " << systConfig.name() );
    }

    totalSF = BtagSF( jets );

    ret = m_btagEffTool->applySystematicVariation(m_currentSyst);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure BTaggingEfficiencyTool for systematic var. " << systConfig.name() );
    }

    return totalSF;

  }

  float SUSYObjDef_xAOD::BtagSF_trkJet(const xAOD::JetContainer* trkjets) const {

    float totalSF = 1.;
    for ( const xAOD::Jet* trkjet : *trkjets ) {

      float sf = 1.;

      if ( fabs(trkjet->eta()) > 2.5 ) {
        ATH_MSG_VERBOSE( "Trying to retrieve b-tagging SF for trkjet with |eta|>2.5 (trkjet eta=" << trkjet->eta() << "), trkjet will be skipped");
      } else if ( trkjet->pt() < 10e3 ){
        ATH_MSG_VERBOSE( "Trying to retrieve b-tagging SF for trkjet with invalid pt (trkjet pt=" << trkjet->pt() << "), jet will be skipped");
      } else {

        CP::CorrectionCode result;
        int truthlabel(-1);
        if (!trkjet->getAttribute("HadronConeExclTruthLabelID", truthlabel)) {
          ATH_MSG_ERROR("Failed to get jet truth label!");
        }
        ATH_MSG_VERBOSE("This jet is " << (acc_bjet(*trkjet) ? "" : "not ") << "b-tagged.");
        ATH_MSG_VERBOSE("This jet's truth label is " << truthlabel);

        if ( acc_bjet(*trkjet) ) {
          result = m_btagEffTool_trkJet->getScaleFactor(*trkjet, sf);

          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( "Failed to retrieve SF for b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( "No valid SF for b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          default:
            ATH_MSG_VERBOSE( "Retrieve SF for b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet with value " << sf );
          }
        } else {

          result = m_btagEffTool_trkJet->getInefficiencyScaleFactor(*trkjet, sf);

          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( "Failed to retrieve SF for non-b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( "No valid inefficiency SF for non-b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          default:
            ATH_MSG_VERBOSE( "Retrieve SF for non-b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet with value " << sf );
          }
        }
      }

      dec_effscalefact(*trkjet) = sf;

      if( acc_signal(*trkjet) ) totalSF *= sf;

    }

    return totalSF;

  }


  float SUSYObjDef_xAOD::BtagSFsys_trkJet(const xAOD::JetContainer* trkjets, const CP::SystematicSet& systConfig)
  {

    float totalSF = 1.;

    //Set the new systematic variation
    CP::SystematicCode ret = m_btagEffTool_trkJet->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure BTaggingEfficiencyTool (track jets) for systematic var. " << systConfig.name() );
    }

    totalSF = BtagSF_trkJet( trkjets );

    ret = m_btagEffTool_trkJet->applySystematicVariation(m_currentSyst);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure BTaggingEfficiencyTool (track jets) for systematic var. " << systConfig.name() );
    }

    return totalSF;

  }

  double SUSYObjDef_xAOD::JVT_SF(const xAOD::JetContainer* jets) {

    float totalSF = 1.;
    if (!m_applyJVTCut) return totalSF;

    ConstDataVector<xAOD::JetContainer> jvtjets(SG::VIEW_ELEMENTS);
    for (const xAOD::Jet* jet : *jets) {
      // Only jets that were good for every cut except JVT
      if (acc_signal_less_JVT(*jet) && acc_passOR(*jet)) {
        jvtjets.push_back(jet);
      }
    }

    CP::CorrectionCode ret = m_jetJvtEfficiencyTool->applyAllEfficiencyScaleFactor( jvtjets.asDataVector() , totalSF );

    switch (ret) {
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve SF for jet in SUSYTools_xAOD::JVT_SF" );
      // this is probably not right, should report an error here
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "No valid SF for jet in SUSYTools_xAOD::JVT_SF" );
      // this may or may not be right, may want to report that this
      // jet doesn't have a valid scale factor
      break;
    default:
      ATH_MSG_VERBOSE( "Retrieve SF for jet container in SUSYTools_xAOD::JVT_SF with value " << totalSF );
    }

    return totalSF;

  }


  double SUSYObjDef_xAOD::JVT_SFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) {

    float totalSF = 1.;
    if (!m_applyJVTCut) return totalSF;

    //Set the new systematic variation
    CP::SystematicCode ret = m_jetJvtEfficiencyTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure JVTEfficiencyTool for systematic var. " << systConfig.name() );
    }

    // Delegate
    totalSF = SUSYObjDef_xAOD::JVT_SF( jets );

    if (m_applyJVTCut) {
      ret = m_jetJvtEfficiencyTool->applySystematicVariation(m_currentSyst);
      if ( ret != CP::SystematicCode::Ok) {
        ATH_MSG_ERROR("Cannot configure JVTEfficiencyTool for systematic var. " << systConfig.name() );
      }
    }

    return totalSF;

  }

  double SUSYObjDef_xAOD::FJVT_SF(const xAOD::JetContainer* jets) {

    float totalSF = 1.;

    ConstDataVector<xAOD::JetContainer> fjvtjets(SG::VIEW_ELEMENTS);
    for (const xAOD::Jet* jet : *jets) {
      // Only jets that were good for every cut except JVT
      if (acc_signal_less_JVT(*jet) && acc_passOR(*jet) && fabs(acc_DetEta(*jet))>m_fJvtEtaMin) {
        fjvtjets.push_back(jet);
      }
    }

    CP::CorrectionCode ret = m_jetFwdJvtEfficiencyTool->applyAllEfficiencyScaleFactor( fjvtjets.asDataVector() , totalSF );

    switch (ret) {
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve SF for jet in SUSYTools_xAOD::FJVT_SF" );
      // this is probably not right, should report an error here
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "No valid SF for jet in SUSYTools_xAOD::FJVT_SF" );
      // this may or may not be right, may want to report that this
      // jet doesn't have a valid scale factor
      break;
    default:
      ATH_MSG_VERBOSE( "Retrieve SF for jet container in SUSYTools_xAOD::FJVT_SF with value " << totalSF );
    }

    return totalSF;

  }

  double SUSYObjDef_xAOD::FJVT_SFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) {

    float totalSF = 1.;

    //Set the new systematic variation
    CP::SystematicCode ret = m_jetFwdJvtEfficiencyTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure FJVTEfficiencyTool for systematic var. " << systConfig.name() );
    }

    // Delegate
    totalSF = SUSYObjDef_xAOD::FJVT_SF( jets );

    if (m_applyJVTCut) {
      ret = m_jetFwdJvtEfficiencyTool->applySystematicVariation(m_currentSyst);
      if ( ret != CP::SystematicCode::Ok) {
        ATH_MSG_ERROR("Cannot configure FJVTEfficiencyTool for systematic var. " << systConfig.name() );
      }
    }

    return totalSF;

  }

  double SUSYObjDef_xAOD::GetTotalJetSF(const xAOD::JetContainer* jets, const bool btagSF, const bool jvtSF, const bool fjvtSF) {

    double totalSF = 1.;
    if (btagSF) totalSF *= BtagSF(jets);

    if (jvtSF && m_applyJVTCut) totalSF *= JVT_SF(jets);

    if (fjvtSF) totalSF *= FJVT_SF(jets);

    return totalSF;
  }


  double SUSYObjDef_xAOD::GetTotalJetSFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig, const bool btagSF, const bool jvtSF, const bool fjvtSF) {

    double totalSF = 1.;
    if (btagSF) totalSF *= BtagSFsys(jets, systConfig);

    if (jvtSF && m_applyJVTCut) totalSF *= JVT_SFsys(jets, systConfig);

    if (fjvtSF) totalSF *= FJVT_SFsys(jets, systConfig);

    return totalSF;
  }

  StatusCode SUSYObjDef_xAOD::BendBTaggingLinks(xAOD::JetContainer* to_container , const std::string& bTagKey) const {
    const xAOD::JetContainer* b_tag_jets = nullptr;
    ATH_CHECK(evtStore()->retrieve(b_tag_jets,bTagKey));
    if (b_tag_jets->size() != to_container->size()) {
      ATH_MSG_FATAL("Size of the original jet container and of the btagg container do not match");
      return StatusCode::FAILURE;
    }
    xAOD::JetContainer::const_iterator btag_begin = b_tag_jets->begin();
    xAOD::JetContainer::const_iterator btag_end   = b_tag_jets->end();

    xAOD::JetContainer::iterator to_begin = to_container->begin();
    xAOD::JetContainer::iterator to_end   = to_container->end();
    for (  ; to_begin != to_end && btag_begin != btag_end ; ++to_begin, ++btag_begin) {
      (*to_begin)->setBTaggingLink((*btag_begin)->btaggingLink());
    }
    return StatusCode::SUCCESS;
  }

  StatusCode SUSYObjDef_xAOD::SetBtagWeightDecorations(const xAOD::Jet& input, const asg::AnaToolHandle<IBTaggingSelectionTool>& btagSelTool, std::string btagTagger) const {
    double weight = 0.;
    if ( btagSelTool->getTaggerWeight(input, weight, false/*useVetoWP=false*/) != CP::CorrectionCode::Ok ) {
      ATH_MSG_ERROR( btagSelTool->name() << ": could not retrieve b-tag weight (" << btagTagger << ")." );
    }
    dec_btag_weight(input) = weight;
    ATH_MSG_DEBUG( btagSelTool->name() << " b-tag weight: " << weight );

    if ( btagSelTool->name().find("DL1")!=std::string::npos ) {
       double dl1_pb(-10), dl1_pc(-10), dl1_pu(-10);
       input.btagging()->pb(btagTagger, dl1_pb);
       input.btagging()->pc(btagTagger, dl1_pc);
       input.btagging()->pu(btagTagger, dl1_pu);
       dec_btag_dl1pb(input) = dl1_pb;
       dec_btag_dl1pc(input) = dl1_pc;
       dec_btag_dl1pu(input) = dl1_pu;
       ATH_MSG_DEBUG( btagSelTool->name() << " b-tag dl1-type pb: " << dl1_pb );
       ATH_MSG_DEBUG( btagSelTool->name() << " b-tag dl1-type pc: " << dl1_pc );
       ATH_MSG_DEBUG( btagSelTool->name() << " b-tag dl1-type pu: " << dl1_pu );
    }
    return StatusCode::SUCCESS;
  }

}

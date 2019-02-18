/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  
  const static SG::AuxElement::Decorator<char> dec_bjet("bjet");
  const static SG::AuxElement::ConstAccessor<char> acc_bjet("bjet");

  const static SG::AuxElement::Decorator<char> dec_bjet_jetunc("bjet_jetunc"); //added for JetUncertainties usage
  const static SG::AuxElement::Decorator<char> dec_bjet_loose("bjet_loose");

  const static SG::AuxElement::Decorator<double> dec_btag_weight("btag_weight");

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

    std::string jetkey_tmp = jetkey;
    if (jetkey.empty()) {
      jetkey_tmp = m_defaultJets;
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
      ATH_MSG_DEBUG("Not retrieving jet collecton, using existing one provided by user");
      jets=copy;
    }

    // Update the jets
    for (const auto& jet : *copy) {
      ATH_CHECK( this->FillJet(*jet, true) );
    }
    // Tool requires a loop over all jets
    m_jetFwdJvtTool->modify(*copy); //compute FwdJVT for all jets

    for (const auto& jet : *copy) {
      // Update the JVT decorations if needed
      if( m_doFwdJVT && fabs(acc_DetEta(*jet)) > m_fwdjetEtaMin ){
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

  StatusCode SUSYObjDef_xAOD::GetTrackJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& jetkey, const xAOD::JetContainer* containerToBeCopied) 
  {
    if (!m_tool_init) {
      ATH_MSG_ERROR("SUSYTools was not initialized!!");
      return StatusCode::FAILURE;
    }

    std::string jetkey_tmp = jetkey;
    if (jetkey.empty()) {
      jetkey_tmp = m_defaultTrackJets;
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
      ATH_MSG_DEBUG("Not retrieving jet collecton, using existing one provided by user");
      jets=copy;
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

    if (!m_fatJets.empty()) {
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
      ATH_MSG_DEBUG("Not retrieving jet collecton, using existing one provided by user");
      jets=copy;
    }

    for (const auto& jet : *copy) {
      ATH_CHECK( this->FillJet(*jet, true, true) );
      //...
      const static SG::AuxElement::Decorator<int> dec_wtagged("wtagged");
      const static SG::AuxElement::Decorator<int> dec_ztagged("ztagged");
      if ( doLargeRdecorations && !m_WtagConfig.empty() && !m_ZtagConfig.empty()){
        dec_wtagged(*jet) = m_WTaggerTool->keep(*jet);
        dec_ztagged(*jet) = m_ZTaggerTool->keep(*jet);
      }
      else{
        dec_wtagged(*jet) = -1;
        dec_ztagged(*jet) = -1;
      }
      //  For OR, selected if it passed cuts
      if ( acc_baseline(*jet) ){
        dec_selected(*jet) = 1;
      }
      else{
        dec_selected(*jet) = 0;
      }
    }
    if (recordSG) {
      ATH_CHECK( evtStore()->record(copy, "STCalib" + jetkey_tmp) );
      ATH_CHECK( evtStore()->record(copyaux, "STCalib" + jetkey_tmp + "Aux.") );
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
    ATH_CHECK( evtStore()->retrieve(muons, "Muons") );
    met::addGhostMuonsToJets(*muons, *copy);

    // Update the jets
    for (const auto& jet : *copy) {
      ATH_CHECK( this->FillJet(*jet, false) );
    }
    // Tool requires a loop over all jets
    m_jetFwdJvtTool->modify(*copy); //compute FwdJVT for all jets
    for (const auto& jet : *copy) {
      // Update the JVT decorations if needed
      if( m_doFwdJVT && fabs(acc_DetEta(*jet)) > m_fwdjetEtaMin ){
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

  StatusCode SUSYObjDef_xAOD::FillJet(xAOD::Jet& input, bool doCalib, bool isFat) {

    ATH_MSG_VERBOSE( "Starting FillJet on jet with pt=" << input.pt() );
    ATH_MSG_VERBOSE(  " jet (pt,eta,phi) before calibration " << input.pt() << " " << input.eta() << " " << input.phi() );

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
        if (!m_fatjetUncertaintiesTool.empty()){
          CP::CorrectionCode result = m_fatjetUncertaintiesTool->applyCorrection(input);
          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( " Failed to apply largeR jet scale uncertainties.");
            return StatusCode::FAILURE;
            //break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( " No valid pt/eta/m range for largeR jet scale uncertainties. ");
            break;
          default:
            break;
          }
        } else {
          ATH_MSG_DEBUG( "No valid fat jet uncertainty, but FillJet called with a fat jet. Skipping uncertainties." );
        }

        return StatusCode::SUCCESS;
      }
      ATH_MSG_VERBOSE(  " jet (pt,eta,phi) after calibration " << input.pt() << " " << input.eta() << " " << input.phi() );

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
      if(!isFat){
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
  
    ATH_MSG_VERBOSE(  " jet (pt,eta,phi) after JES correction " << input.pt() << " " << input.eta() << " " << input.phi() );

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

    if (m_useBtagging) {
      if (m_BtagWP != "Continuous") this->IsTrackBJet(input);
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

    double weight = 0.;
    if ( m_btagSelTool->getTaggerWeight(input, weight, false/*useVetoWP=false*/) != CP::CorrectionCode::Ok ) {
      ATH_MSG_ERROR ("btagSelTool:: could not retrieve b-tag weight.");
    }
    dec_btag_weight(input) = weight;
    ATH_MSG_VERBOSE( "b-tag weight?: " << weight );

    return isbjet;
  }

  bool SUSYObjDef_xAOD::IsTrackBJet(const xAOD::Jet& input) const {

    bool isbjet = m_btagSelTool_trkJet->accept(input);
    dec_bjet(input) = isbjet;

    double weight = 0.;
    if ( m_btagSelTool_trkJet->getTaggerWeight(input, weight, false/*useVetoWP=false*/) != CP::CorrectionCode::Ok ) {
      ATH_MSG_ERROR ("btagSelTool_trkJet:: could not retrieve b-tag weight.");
    }
    dec_btag_weight(input) = weight;
    ATH_MSG_VERBOSE( "b-tag weight (track jets)?: " << weight );

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

    return isbjet;
  }

  int SUSYObjDef_xAOD::IsTrackBJetContinuous(const xAOD::Jet& input) const {

    int isbjet = m_btagSelTool_trkJet->getQuantile(input);
    dec_bjet(input) = isbjet;

    return isbjet;
  }

  float SUSYObjDef_xAOD::BtagSF(const xAOD::JetContainer* jets) const {

    float totalSF = 1.;
    for ( const auto& jet : *jets ) {

      float sf = 1.;

      if ( fabs(jet->eta()) > 2.5 ) {
        ATH_MSG_VERBOSE( " Trying to retrieve b-tagging SF for jet with |eta|>2.5 (jet eta=" << jet->eta() << "), jet will be skipped");
      } else if ( jet->pt() < 20e3 ){ 
        ATH_MSG_VERBOSE( " Trying to retrieve b-tagging SF for jet with invalid pt (jet pt=" << jet->pt() << "), jet will be skipped");
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
            ATH_MSG_ERROR( " Failed to retrieve SF for b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( " No valid SF for b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          default:
            ATH_MSG_VERBOSE( " Retrieve SF for b-tagged jets in SUSYTools_xAOD::BtagSF with value " << sf );
          }
        } else {

          result = m_btagEffTool->getInefficiencyScaleFactor(*jet, sf);

          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( " Failed to retrieve SF for non-b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( " No valid inefficiency SF for non-b-tagged jets in SUSYTools_xAOD::BtagSF" );
            break;
          default:
            ATH_MSG_VERBOSE( " Retrieve SF for non-b-tagged jets in SUSYTools_xAOD::BtagSF with value " << sf );
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
    for ( const auto& trkjet : *trkjets ) {

      float sf = 1.;

      if ( fabs(trkjet->eta()) > 2.5 ) {
        ATH_MSG_VERBOSE( " Trying to retrieve b-tagging SF for trkjet with |eta|>2.5 (trkjet eta=" << trkjet->eta() << "), trkjet will be skipped");
      } else if ( trkjet->pt() < 20e3 ){ 
        ATH_MSG_VERBOSE( " Trying to retrieve b-tagging SF for trkjet with invalid pt (trkjet pt=" << trkjet->pt() << "), jet will be skipped");
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
            ATH_MSG_ERROR( " Failed to retrieve SF for b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( " No valid SF for b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          default:
            ATH_MSG_VERBOSE( " Retrieve SF for b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet with value " << sf );
          }
        } else {

          result = m_btagEffTool_trkJet->getInefficiencyScaleFactor(*trkjet, sf);

          switch (result) {
          case CP::CorrectionCode::Error:
            ATH_MSG_ERROR( " Failed to retrieve SF for non-b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          case CP::CorrectionCode::OutOfValidityRange:
            ATH_MSG_VERBOSE( " No valid inefficiency SF for non-b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet" );
            break;
          default:
            ATH_MSG_VERBOSE( " Retrieve SF for non-b-tagged trk jets in SUSYTools_xAOD::BtagSF_trkJet with value " << sf );
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
    for (const auto& jet : *jets) {
      // Only jets that were good for every cut except JVT
      if (acc_signal_less_JVT(*jet) && acc_passOR(*jet)) {
        jvtjets.push_back(jet);
      }
    }

    CP::CorrectionCode ret = m_jetJvtEfficiencyTool->applyAllEfficiencyScaleFactor( jvtjets.asDataVector() , totalSF );

    switch (ret) {
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve SF for jet in SUSYTools_xAOD::JVT_SF" );
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "No valid SF for jet in SUSYTools_xAOD::JVT_SF" );
    default:
      ATH_MSG_VERBOSE( " Retrieve SF for jet container in SUSYTools_xAOD::JVT_SF with value " << totalSF );
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
    for (const auto& jet : *jets) {
      // Only jets that were good for every cut except JVT
      if (acc_signal_less_JVT(*jet) && acc_passOR(*jet) && fabs(acc_DetEta(*jet))>m_fwdjetEtaMin) {
        fjvtjets.push_back(jet);
      }
    }

    CP::CorrectionCode ret = m_jetFJvtEfficiencyTool->applyAllEfficiencyScaleFactor( fjvtjets.asDataVector() , totalSF );

    switch (ret) {
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve SF for jet in SUSYTools_xAOD::FJVT_SF" );
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "No valid SF for jet in SUSYTools_xAOD::FJVT_SF" );
    default:
      ATH_MSG_VERBOSE( " Retrieve SF for jet container in SUSYTools_xAOD::FJVT_SF with value " << totalSF );
    }

    return totalSF;

  }

  double SUSYObjDef_xAOD::FJVT_SFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) {

    float totalSF = 1.;

    //Set the new systematic variation
    CP::SystematicCode ret = m_jetFJvtEfficiencyTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure FJVTEfficiencyTool for systematic var. " << systConfig.name() );
    }

    // Delegate
    totalSF = SUSYObjDef_xAOD::FJVT_SF( jets );

    if (m_applyJVTCut) {
      ret = m_jetFJvtEfficiencyTool->applySystematicVariation(m_currentSyst);
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


}

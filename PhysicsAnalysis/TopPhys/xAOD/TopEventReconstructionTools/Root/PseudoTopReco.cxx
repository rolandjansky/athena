/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// definitions taken from the MAMbo code here: https://twiki.cern.ch/twiki/bin/view/Main/MAMbo

// Local include(s):
#include "TopEventReconstructionTools/PseudoTopReco.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>

namespace top{
  
  PseudoTopReco::PseudoTopReco( const std::string& name) :
    asg::AsgTool( name ),
    m_config(nullptr),
    m_bTagCutValue(9999.9), 
    m_leptonType("SetMe")
  {
    declareProperty( "config" ,    m_config ,                    "Set the configuration" );
    declareProperty( "LeptonType", m_leptonType = "kUndefined" , "Define the lepton type" );
  }
  
  /// Function initialising the tool
  StatusCode PseudoTopReco::initialize()
  {    
    // Have you set the config??
    if (m_config == nullptr) {
      ATH_MSG_ERROR("Please set the top::TopConfig");
      return StatusCode::FAILURE;
    }

    m_config->setPseudoTop();
      
    // Figure out the b tagging working point
    if (m_config->bTagWP_available().size() != 1) {
      ATH_MSG_INFO(m_config->bTagWP_available().size()<<" b-tagging WP - cannot pick b-jets. Please select only 1 WP if you want to use the PseudoTop reconstruction");
    }
    
    
    ATH_MSG_INFO("++++++++++++++++++++++++++++++");
    ATH_MSG_INFO("  Using Lepton \t" << m_leptonType);
    ATH_MSG_INFO("++++++++++++++++++++++++++++++");    
    
    /// Return gracefully:
    return StatusCode::SUCCESS;     
  }
  
  /// Function executing the tool
  StatusCode PseudoTopReco::execute(const top::Event& event)
  {

    // Create the partonHistory xAOD object                                                                                                                             
    xAOD::PseudoTopResultAuxContainer* pseudoTopAuxCont = new xAOD::PseudoTopResultAuxContainer{};
    xAOD::PseudoTopResultContainer* pseudoTop = new xAOD::PseudoTopResultContainer{};
    pseudoTop->setStore( pseudoTopAuxCont );

    xAOD::PseudoTopResult* PseudoTopResult = new xAOD::PseudoTopResult{};
    pseudoTop       -> push_back( PseudoTopResult );

    PseudoTopResult -> IniVar(true);

    // fill either reco particles or truth particles into four vectors
    // store them in GeV
    // check if particle/jet is b-tagged
    // check if electron or muon channel
    // store output so that it can be written later to the event saver
        
    m_bJets.clear();
    m_lightJets.clear();
    
    // get the MET x and y components for the reconstruction
    m_nu_px  = event.m_met->mpx() / 1.e3;
    m_nu_py  = event.m_met->mpy() / 1.e3;
    m_met_et =  sqrt(m_nu_px*m_nu_px + m_nu_py*m_nu_py);

    SetJetInfo(event);
    SetChargedLeptonInfo(event);

    if(m_bJets.size() == 2 && m_lightJets.size() >= 2){

      ReconstructLeptonicW();
      RunReconstruction();

      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_lep_pt" )  = m_top_lep.Pt();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_lep_eta" ) = m_top_lep.Eta();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_lep_phi" ) = m_top_lep.Phi();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_lep_m" )   = m_top_lep.M();

      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_had_pt" )  = m_top_had.Pt();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_had_eta" ) = m_top_had.Eta();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_had_phi" ) = m_top_had.Phi();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_top_had_m" )   = m_top_had.M();

      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_ttbar_pt" )  = m_ttbar.Pt();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_ttbar_eta" ) = m_ttbar.Eta();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_ttbar_phi" ) = m_ttbar.Phi();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Reco_ttbar_m" )   = m_ttbar.M();

    }
    else{

      //      std::cout<< "NOT ENOUGH BJETS!!!" << std::endl;

    }

    // Save to StoreGate / TStore                                                                                                                                       
    std::string outputSGKey    = m_config->sgKeyPseudoTop(event.m_hashValue);
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save    = evtStore()->tds()->record( pseudoTop , outputSGKey );
    xAOD::TReturnCode saveAux = evtStore()->tds()->record( pseudoTopAuxCont , outputSGKeyAux );
    if( !save || !saveAux ){
      return StatusCode::FAILURE;
    }    
    
    /// Return gracefully:
    return StatusCode::SUCCESS;     
  }

  StatusCode PseudoTopReco::execute(const top::ParticleLevelEvent& plEvent)
  {

    // Create the pseudoTopHistory xAOD object                                                                                                                             
    xAOD::PseudoTopResultAuxContainer* pseudoTopAuxCont = new xAOD::PseudoTopResultAuxContainer{};
    xAOD::PseudoTopResultContainer* pseudoTop = new xAOD::PseudoTopResultContainer{};
    pseudoTop->setStore( pseudoTopAuxCont );

    xAOD::PseudoTopResult* PseudoTopResult = new xAOD::PseudoTopResult{};
    pseudoTop->push_back( PseudoTopResult );

    PseudoTopResult -> IniVar(false);


    m_bJets.clear();
    m_lightJets.clear();

    // get the MET x and y components for the reconstruction                                                                                                           
    m_nu_px  = plEvent.m_met->mpx() / 1.e3;
    m_nu_py  = plEvent.m_met->mpy() / 1.e3;
    m_met_et = sqrt(m_nu_px*m_nu_px + m_nu_py*m_nu_py);
 
    SetJetInfo(plEvent);
    SetChargedLeptonInfo(plEvent);

    if(m_bJets.size() == 2 && m_lightJets.size() >= 2){

      ReconstructLeptonicW();
      RunReconstruction();

      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_lep_pt" )  = m_top_lep.Pt();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_lep_eta" ) = m_top_lep.Eta();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_lep_phi" ) = m_top_lep.Phi();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_lep_m" )   = m_top_lep.M();

      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_had_pt" )  = m_top_had.Pt();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_had_eta" ) = m_top_had.Eta();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_had_phi" ) = m_top_had.Phi();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_top_had_m" )   = m_top_had.M();

      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_ttbar_pt" )  = m_ttbar.Pt();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_ttbar_eta" ) = m_ttbar.Eta();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_ttbar_phi" ) = m_ttbar.Phi();
      PseudoTopResult->auxdecor< float >( "PseudoTop_Particle_ttbar_m" )   = m_ttbar.M();

      

    }
    else{

      //    std::cout << "NOT ENOUGH BJETS!!!" << std::endl;

    }


    // Save to StoreGate / TStore                                                                                                                                       
    std::string outputSGKey    = m_config->sgKeyPseudoTop(0);
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save    = evtStore()->tds()->record( pseudoTop , outputSGKey );
    xAOD::TReturnCode saveAux = evtStore()->tds()->record( pseudoTopAuxCont , outputSGKeyAux );
    if( !save || !saveAux ){
      return StatusCode::FAILURE;
    }

    /// Return gracefully:                                                                                                                                             
    return StatusCode::SUCCESS;
  }
  

  bool PseudoTopReco::SetChargedLeptonInfo(const top::Event& event){

    if (m_leptonType == "kElectron") {
      m_lepton.SetPtEtaPhiE(event.m_electrons.at(0)->pt()/1.e3 , event.m_electrons.at(0)->eta(), event.m_electrons.at(0)->phi(), event.m_electrons.at(0) ->e() / 1.e3 );
    }
    else if (m_leptonType == "kMuon") {
      m_lepton.SetPtEtaPhiE( event.m_muons.at(0)->pt()/1.e3 ,  event.m_muons.at(0)->eta() , event.m_muons.at(0)->phi(),  event.m_muons.at(0)->e() / 1.e3);
    }
    else {
      ATH_MSG_ERROR(" Please supply a valid LeptonType : kElectron or kMuon");
      return false;
    }

    return true;

  }

  bool PseudoTopReco::SetChargedLeptonInfo(const top::ParticleLevelEvent& plEvent){

    if (m_leptonType == "kElectron") {
      m_lepton.SetPtEtaPhiE( plEvent.m_electrons->at(0)->pt()/1.e3 , plEvent.m_electrons->at(0)->eta(), plEvent.m_electrons->at(0)->phi(),  plEvent.m_electrons->at(0)->e() / 1.e3 );
    }
    else if (m_leptonType == "kMuon") {
      m_lepton.SetPtEtaPhiE( plEvent.m_muons->at(0)->pt()/1.e3 , plEvent.m_muons->at(0)->eta(), plEvent.m_muons->at(0)->phi(),  plEvent.m_muons->at(0)->e() / 1.e3 );
    }
    else {
      ATH_MSG_ERROR(" Please supply a valid LeptonType : kElectron or kMuon");
      return false;
    }
    
    return true;
    
  }
  

  bool PseudoTopReco::SetJetInfo(const top::Event& event){
    
    int bCounter = 0;

    for (const auto* const jetPtr : event.m_jets) {

      TLorentzVector helpVec(0, 0, 0, 0);
      helpVec.SetPtEtaPhiE( jetPtr->pt() / 1.e3 , jetPtr->eta() , jetPtr->phi() , jetPtr->e() / 1.e3);
      
      // if more than two b-jets are available, consider third leading b-tagged jet as light jet!
      std::string out = m_config->bTagWP_available()[0];

      const bool hasbTagFlag = jetPtr->isAvailable<char>("isbtagged_"+out);

      if (hasbTagFlag) {
	if (jetPtr->auxdataConst<char>("isbtagged_"+out) && bCounter < 2) {
	  m_bJets.push_back(helpVec);

	  bCounter++;
	}
	else{
	  m_lightJets.push_back(helpVec);
	}
      }
    } // end loop over jets
    

    return true;
    
  }
  
  
  bool PseudoTopReco::SetJetInfo(const top::ParticleLevelEvent& plEvent){
    
    // if more than two b-jets are available, consider third leading b-tagged jet as light jet!

    int bCounter = 0;

    for (const auto & jetPtr : * plEvent.m_jets) {

      TLorentzVector helpVec(0, 0, 0, 0);
      helpVec.SetPtEtaPhiE( jetPtr->pt() / 1.e3 , jetPtr->eta() , jetPtr->phi() , jetPtr->e() / 1.e3);
      
      int nGhosts = jetPtr->auxdata<int>( "GhostBHadronsFinalCount" );
      
      if(nGhosts >= 1 && bCounter < 2){
	m_bJets.push_back(helpVec);
	bCounter++;
      }
      else
	m_lightJets.push_back(helpVec);
    }
      
    return true;
    
  }


  bool PseudoTopReco::RunReconstruction(){
  
    double bestwmass = 999.e9;
    TLorentzVector Whelp(0, 0, 0, 0);

    // get first the b-jet closest to the lepton in dR
    // define this as TLorentzVector m_b_lep;
    // take now remaining b-jet as m_b_had 
    double dRb1 = m_bJets[0].DeltaR(m_lepton);
    double dRb2 = m_bJets[1].DeltaR(m_lepton);

    if(dRb1 < dRb2){
      m_b_lep = m_bJets[0];
      m_b_had = m_bJets[1];
    }
    else{
      m_b_lep = m_bJets[1];
      m_b_had = m_bJets[0];
    }

    // then loop over all light jets                                                                                                                                    
    // take combination which is closest to mW (PDG) 
    for(unsigned int iJet = 0; iJet < m_lightJets.size(); ++iJet){
      for(unsigned int kJet = 0; kJet < m_lightJets.size(); ++kJet){
	
	if(iJet == kJet)
	  continue;

	Whelp = m_lightJets[iJet] + m_lightJets[kJet];
		
	if(fabs(mWPDG - Whelp.M()) < fabs(mWPDG - bestwmass)){
	
	  m_W_had = Whelp;
	  bestwmass = Whelp.M();	  
	  
	}

      }
    } 
    
    // define now the top and ttbar four-vectors
    m_top_lep = m_W_lep + m_b_lep;
    m_top_had = m_W_had + m_b_had;
    m_ttbar   = m_top_lep + m_top_had;
    
    return true;
    
  }
  
  bool PseudoTopReco::ReconstructLeptonicW()
  {

    double v_pz  = -KinemEdge;
    double delta = -KinemEdge;
    
    // lepton already made by reco or particle levels:                                                                                                                  
    double l_px = m_lepton.Px();  double l_py = m_lepton.Py();  double l_pz = m_lepton.Pz();
    double l_m  = m_lepton.M();   double l_E  = m_lepton.E();

    double mdiff = 0.5 * ( mWPDG*mWPDG - l_m*l_m );
    double pT_vl = m_nu_px*l_px + m_nu_py*l_py; 

    double a = l_E*l_E - l_pz*l_pz; 
    double b = -2. * l_pz * ( mdiff + pT_vl ); 
    double c = m_met_et*m_met_et*l_E*l_E - mdiff*mdiff - pT_vl*pT_vl - 2.*mdiff*pT_vl; 

    delta = b*b - 4.*a*c; 

    if( delta <= 0. ) {
      // take only the real part
      v_pz = -0.5*b/a;
    } else {
      double v_pz_1 = 0.5 * ( -b - sqrt(delta) ) / a;
      double v_pz_2 = 0.5 * ( -b + sqrt(delta) ) / a;
   
      v_pz = ( fabs(v_pz_1) > fabs(v_pz_2) ) ? v_pz_2 : v_pz_1;
      
    }
    
    double v_E  = sqrt( m_met_et*m_met_et + v_pz*v_pz ); 

    m_neutrino.SetPxPyPzE( m_nu_px, m_nu_py, v_pz, v_E );
    
    m_W_lep = m_neutrino + m_lepton;

    return true;

  }


  /// Function finalizing the tool
  StatusCode PseudoTopReco::finalize()
  {
    /// Return gracefully:
    return StatusCode::SUCCESS;    
  }


  

}

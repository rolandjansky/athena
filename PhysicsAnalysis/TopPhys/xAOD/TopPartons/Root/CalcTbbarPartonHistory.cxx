/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopPartons/CalcTbbarPartonHistory.h"
#include "TopConfiguration/TopConfig.h"

namespace top{
  
  CalcTbbarPartonHistory::CalcTbbarPartonHistory(const std::string& name) : CalcTopPartonHistory( name ){}
  
  void CalcTbbarPartonHistory::tbbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* tbbarPartonHistory){
     
     tbbarPartonHistory->IniVarTbbar();

     TLorentzVector t_before, t_after;
     TLorentzVector Wp;
     TLorentzVector b_from_top;
     TLorentzVector WpDecay1;	 
     TLorentzVector WpDecay2;
     int WpDecay1_pdgId;
     int WpDecay2_pdgId;
     bool          event_top = CalcTopPartonHistory::topWb(truthParticles,  6, t_before, t_after, Wp, b_from_top, WpDecay1, WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
     if(!event_top)event_top = CalcTopPartonHistory::topWb(truthParticles, -6, t_before, t_after, Wp, b_from_top, WpDecay1, WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId);
     
     TLorentzVector b_beforeFSR;
     TLorentzVector b_afterFSR;
     bool event_b = CalcTopPartonHistory::b(truthParticles, b_beforeFSR, b_afterFSR);
     
     if (event_top && event_b){
     	     TLorentzVector temp = t_before+b_beforeFSR;
     	     tbbarPartonHistory->auxdecor< float >( "MC_tbbar_beforeFSR_m" ) = temp.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_tbbar_beforeFSR_pt" ) = temp.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_tbbar_beforeFSR_phi" ) = temp.Phi();
     	     fillEtaBranch(tbbarPartonHistory,"MC_tbbar_beforeFSR_eta", temp);
     	     
     	     temp = WpDecay1+WpDecay2+b_from_top + b_afterFSR;
     	     tbbarPartonHistory->auxdecor< float >( "MC_tbbar_afterFSR_m" ) = temp.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_tbbar_afterFSR_pt" ) = temp.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_tbbar_afterFSR_phi" ) = temp .Phi();
	     fillEtaBranch(tbbarPartonHistory, "MC_tbbar_afterFSR_eta", temp);
     }//if       
     if (event_top){
     	     tbbarPartonHistory->auxdecor< float >( "MC_t_beforeFSR_m" ) = t_before.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_t_beforeFSR_pt" ) = t_before.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_t_beforeFSR_phi" ) = t_before.Phi();
     	     fillEtaBranch(tbbarPartonHistory,"MC_t_beforeFSR_eta", t_before);

     	     tbbarPartonHistory->auxdecor< float >( "MC_W_from_t_m" ) = Wp.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_W_from_t_pt" ) = Wp.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_W_from_t_phi" ) = Wp.Phi();
     	     fillEtaBranch(tbbarPartonHistory, "MC_W_from_t_eta", Wp);
    
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_from_t_m" ) = b_from_top.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_from_t_pt" ) = b_from_top.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_from_t_phi" ) = b_from_top.Phi();
     	     fillEtaBranch(tbbarPartonHistory,"MC_b_from_t_eta", b_from_top);

     	     tbbarPartonHistory->auxdecor< float >( "MC_Wdecay1_from_t_m" ) = WpDecay1.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_Wdecay1_from_t_pt" ) = WpDecay1.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_Wdecay1_from_t_phi" ) = WpDecay1.Phi();
     	     tbbarPartonHistory->auxdecor< int >( "MC_Wdecay1_from_t_pdgId" ) = WpDecay1_pdgId;
     	     fillEtaBranch(tbbarPartonHistory,"MC_Wdecay1_from_t_eta", WpDecay1);

     	     tbbarPartonHistory->auxdecor< float >( "MC_Wdecay2_from_t_m" ) = WpDecay2.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_Wdecay2_from_t_pt" ) = WpDecay2.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_Wdecay2_from_t_phi" ) = WpDecay2.Phi();
     	     tbbarPartonHistory->auxdecor< int >( "MC_Wdecay2_from_t_pdgId" ) = WpDecay2_pdgId;
     	     fillEtaBranch(tbbarPartonHistory,"MC_Wdecay2_from_t_eta", WpDecay2);
     }//if       
     if (event_b){

     	     tbbarPartonHistory->auxdecor< float >( "MC_b_beforeFSR_m" ) = b_beforeFSR.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_beforeFSR_pt" ) = b_beforeFSR.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_beforeFSR_phi" ) = b_beforeFSR.Phi();
     	     fillEtaBranch(tbbarPartonHistory,"MC_b_beforeFSR_eta", b_beforeFSR);

     	     tbbarPartonHistory->auxdecor< float >( "MC_b_afterFSR_m" ) = b_afterFSR.M();
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_afterFSR_pt" ) = b_afterFSR.Pt();
     	     tbbarPartonHistory->auxdecor< float >( "MC_b_afterFSR_phi" ) = b_afterFSR.Phi();
     	     fillEtaBranch(tbbarPartonHistory,"MC_b_afterFSR_eta", b_afterFSR);
	     
     }//if 
         
  }

  StatusCode CalcTbbarPartonHistory::execute()
  {
     // Get the Truth Particles
     const xAOD::TruthParticleContainer* truthParticles(nullptr);
     ATH_CHECK( evtStore()->retrieve( truthParticles , m_config->sgKeyMCParticle() ) );

     // Create the partonHistory xAOD object
     xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer{};    
     xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer{};
     partonCont->setStore( partonAuxCont );
  
     xAOD::PartonHistory* tbbarPartonHistory = new xAOD::PartonHistory{};
     partonCont->push_back( tbbarPartonHistory );
     
     // Recover the parton history for tbbar events     
     tbbarHistorySaver(truthParticles, tbbarPartonHistory);     
          
     // Save to StoreGate / TStore
     std::string outputSGKey = m_config->sgKeyTopPartonHistory();
     std::string outputSGKeyAux = outputSGKey + "Aux.";
      
     xAOD::TReturnCode save = evtStore()->tds()->record( partonCont , outputSGKey );
     xAOD::TReturnCode saveAux = evtStore()->tds()->record( partonAuxCont , outputSGKeyAux );
     if( !save || !saveAux ){
       return StatusCode::FAILURE;
     }      
    
     return StatusCode::SUCCESS;
  }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopPartons/CalcTtbarGammaPartonHistory.h"
#include "TopConfiguration/TopConfig.h"

namespace top{
  
  CalcTtbarGammaPartonHistory::CalcTtbarGammaPartonHistory(const std::string& name) : CalcTopPartonHistory( name ){}

  void CalcTtbarGammaPartonHistory::ttbarGammaHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* ttbarGammaPartonHistory){

     ttbarGammaPartonHistory->IniVarTtGamma();
     TLorentzVector ph_t; bool has_ph_t; int branchtype_t=-1; int init_type=-1;
     TLorentzVector t_before, t_after;
     TLorentzVector Wp;
     TLorentzVector b;
     TLorentzVector WpDecay1;	 
     TLorentzVector WpDecay2;
     int WpDecay1_pdgId;
     int WpDecay2_pdgId;
     bool event_top = CalcTopPartonHistory::topPhWb(truthParticles, 6, t_before, t_after, ph_t, Wp, b, WpDecay1, WpDecay1_pdgId, WpDecay2, WpDecay2_pdgId, has_ph_t, branchtype_t, init_type);

     TLorentzVector ph_tbar; bool has_ph_tbar; int branchtype_tbar=0;
     TLorentzVector tbar_before, tbar_after;
     TLorentzVector Wm;
     TLorentzVector bbar;
     TLorentzVector WmDecay1;	 
     TLorentzVector WmDecay2;
     int WmDecay1_pdgId;
     int WmDecay2_pdgId;
     bool event_topbar = CalcTopPartonHistory::topPhWb(truthParticles, -6, tbar_before, tbar_after, ph_tbar, Wm, bbar, WmDecay1, WmDecay1_pdgId, WmDecay2, WmDecay2_pdgId, has_ph_tbar, branchtype_tbar, init_type);


     TLorentzVector ph; int ph_source = -1; 
     int category = -1;
     /// event category:
	/// 0: missing top or missing W(only if the corresponding top radiates)
	/// 1: Single Lepton inital quark radiation
	/// 2: Single Lepton virtual top radiation
	/// 3: Single Lepton leptonic top radiation
	/// 4: Single Lepton hadronic top radiation
	/// 5: Single Lepton leptonic W radiation
	/// 6: Single Lepton hadronic W radiation
	/// 7: Dilepton quark radiation
	/// 8: Dilepton virtual top radiation
	/// 9: Dilepton top radiation
	// 10: Dilepton W radiation



     if( (event_top && !event_topbar) || (!event_top && event_topbar) ) {
	category = 0;
	ttbarGammaPartonHistory->auxdecor< int >( "MC_Event_Category" ) = category;
     }// one of the top is virtual

     else if(event_top && event_topbar && (has_ph_t || has_ph_tbar)){

     	if(has_ph_t && has_ph_tbar){// isr should give the same photon to both tops
	if(ph_t.Pt()>ph_tbar.Pt()){ph = ph_t;}// FOR EXTRA SAFETY
	else{ph = ph_tbar;}
        ph_source=0; // isr
     	}
     	else if(has_ph_t){ph = ph_t; ph_source = 1;}// top
     	else if(has_ph_tbar){ph = ph_tbar; ph_source = 2;}// antitop
	else if(!has_ph_t && !has_ph_tbar){ph_source = -1;}

	// determination of event category
	// ISR
	if(ph_source==0){
		if(init_type==2) {
			category = 1;
			if(branchtype_t==15 && branchtype_tbar==15){category = 7;} // ISR_qq_DL
		}	//ISR_qq
		else if(init_type==1) {
			category = 2;
			if(branchtype_t==15 && branchtype_tbar==15){category = 8;} // ISR_gg_DL
		}	//ISR_gg/top
	}
	else if(ph_source==1 || ph_source==2){
		if((branchtype_t==12 && branchtype_tbar==50) || (branchtype_t==50 && branchtype_tbar==12)){category = 3;}//	top Radiation leptonic
		else if((branchtype_t==52 && branchtype_tbar==10) || (branchtype_t==10 && branchtype_tbar==52)){category = 4;}//	top Radiation hadronic
		else if((branchtype_t==13 && branchtype_tbar==50) || (branchtype_t==50 && branchtype_tbar==13)){category = 5;}//	W Radiation leptonic
		else if((branchtype_t==53 && branchtype_tbar==10) || (branchtype_t==10 && branchtype_tbar==53)){category = 6;}//	W Radiation hadronic
		else if((branchtype_t==12 && branchtype_tbar==10) || (branchtype_t==10 && branchtype_tbar==12)){category = 9;}//	Top Radiation dileptonic
		else if((branchtype_t==13 && branchtype_tbar==10) || (branchtype_t==10 && branchtype_tbar==13)){category = 10;}//	W Radiation dileptonic
		else if(branchtype_t==1 || branchtype_tbar==1){category=0;}
	}
	else if(ph_source==-1){category=-1;}

//------------------------------------------------------------------------------------------
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ph_m" ) = ph.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ph_pt" ) = ph.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ph_phi" ) = ph.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_ph_eta", ph);

        ttbarGammaPartonHistory->auxdecor< int >( "MC_branchtype_t" ) = branchtype_t;
        ttbarGammaPartonHistory->auxdecor< int >( "MC_branchtype_tbar" ) = branchtype_tbar;
        ttbarGammaPartonHistory->auxdecor< int >( "MC_initial_parton" ) = init_type;
        ttbarGammaPartonHistory->auxdecor< int >( "MC_ph_from_t_tbar" ) = ph_source;
	ttbarGammaPartonHistory->auxdecor< int >( "MC_Event_Category" ) = category;
//------------------------------------------------------------------------------------------

     	TLorentzVector temp = t_before+tbar_before;
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ttbar_beforeFSR_m" ) = temp.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ttbar_beforeFSR_pt" ) = temp.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ttbar_beforeFSR_phi" ) = temp.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_ttbar_beforeFSR_eta", temp);

     	temp = WmDecay1+WmDecay2+b + WpDecay1+WpDecay2+bbar + ph;
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ttbar_afterFSR_m" ) = temp.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ttbar_afterFSR_pt" ) = temp.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_ttbar_afterFSR_phi" ) = temp .Phi();
	fillEtaBranch(ttbarGammaPartonHistory, "MC_ttbar_afterFSR_eta", temp);
     }


//------------------------------------------------------------------------------------------
     if(event_top){
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_t_beforeFSR_m" ) = t_before.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_t_beforeFSR_pt" ) = t_before.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_t_beforeFSR_phi" ) = t_before.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_t_beforeFSR_eta", t_before);
	     
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_t_afterFSR_m" ) = t_after.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_t_afterFSR_pt" ) = t_after.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_t_afterFSR_phi" ) = t_after.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_t_afterFSR_eta", t_after);
	   
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_W_from_t_m" ) = Wp.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_W_from_t_pt" ) = Wp.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_W_from_t_phi" ) = Wp.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory, "MC_W_from_t_eta", Wp);
    
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_b_from_t_m" ) = b.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_b_from_t_pt" ) = b.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_b_from_t_phi" ) = b.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_b_from_t_eta", b);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay1_from_t_m" ) = WpDecay1.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay1_from_t_pt" ) = WpDecay1.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay1_from_t_phi" ) = WpDecay1.Phi();
     	ttbarGammaPartonHistory->auxdecor< int >( "MC_Wdecay1_from_t_pdgId" ) = WpDecay1_pdgId;
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_Wdecay1_from_t_eta", WpDecay1);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay2_from_t_m" ) = WpDecay2.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay2_from_t_pt" ) = WpDecay2.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay2_from_t_phi" ) = WpDecay2.Phi();
     	ttbarGammaPartonHistory->auxdecor< int >( "MC_Wdecay2_from_t_pdgId" ) = WpDecay2_pdgId;
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_Wdecay2_from_t_eta", WpDecay2);
     }


//-------------------------------------------------------------------------------------------
     if(event_topbar){
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_tbar_beforeFSR_m" ) = tbar_before.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_tbar_beforeFSR_pt" ) = tbar_before.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_tbar_beforeFSR_phi" ) = tbar_before.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_tbar_beforeFSR_eta", tbar_before);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_tbar_afterFSR_m" ) = tbar_after.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_tbar_afterFSR_pt" ) = tbar_after.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_tbar_afterFSR_phi" ) = tbar_after.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_tbar_afterFSR_eta", tbar_after);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_W_from_tbar_m" ) = Wm.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_W_from_tbar_pt" ) = Wm.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_W_from_tbar_phi" ) = Wm.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_W_from_tbar_eta", Wm);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_b_from_tbar_m" ) = bbar.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_b_from_tbar_pt" ) = bbar.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_b_from_tbar_phi" ) = bbar.Phi();
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_b_from_tbar_eta", bbar);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay1_from_tbar_m" ) = WmDecay1.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay1_from_tbar_pt" ) = WmDecay1.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay1_from_tbar_phi" ) = WmDecay1.Phi();
     	ttbarGammaPartonHistory->auxdecor< int >( "MC_Wdecay1_from_tbar_pdgId" ) = WmDecay1_pdgId;
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_Wdecay1_from_tbar_eta", WmDecay1);

     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay2_from_tbar_m" ) = WmDecay2.M();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay2_from_tbar_pt" ) = WmDecay2.Pt();
     	ttbarGammaPartonHistory->auxdecor< float >( "MC_Wdecay2_from_tbar_phi" ) = WmDecay2.Phi(); 
     	ttbarGammaPartonHistory->auxdecor< int >( "MC_Wdecay2_from_tbar_pdgId" ) = WmDecay2_pdgId; 
     	fillEtaBranch(ttbarGammaPartonHistory,"MC_Wdecay2_from_tbar_eta", WmDecay2);			     
     }
        
  }

  StatusCode CalcTtbarGammaPartonHistory::execute()
  {
     // Get the Truth Particles
     const xAOD::TruthParticleContainer* truthParticles(nullptr);
     ATH_CHECK( evtStore()->retrieve( truthParticles , m_config->sgKeyMCParticle() ) );

     // Create the partonHistory xAOD object
     xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer{};    
     xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer{};
     partonCont->setStore( partonAuxCont );
  
     xAOD::PartonHistory* ttbarGammaPartonHistory = new xAOD::PartonHistory{};
     partonCont->push_back( ttbarGammaPartonHistory );
     
     // Recover the parton history for ttbar events     
     ttbarGammaHistorySaver(truthParticles, ttbarGammaPartonHistory);     
          
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

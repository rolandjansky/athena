/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopPartons/CalcWlvPartonHistory.h"
#include "TopConfiguration/TopConfig.h"

namespace top{
  
  CalcWlvPartonHistory::CalcWlvPartonHistory(const std::string& name) : CalcTopPartonHistory( name ){}
  
  void CalcWlvPartonHistory::WlvHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* wlvPartonHistory){
     
     wlvPartonHistory->IniVarWlv();

     TLorentzVector W;
     TLorentzVector WDecay1;	 
     TLorentzVector WDecay2;
     int WDecay1_pdgId;
     int WDecay2_pdgId;
     bool goodevent = CalcTopPartonHistory::Wlv(truthParticles, W, WDecay1, WDecay1_pdgId, WDecay2, WDecay2_pdgId);
     
      
     if (goodevent){
     	     wlvPartonHistory->auxdecor< float >( "MC_W_m" ) = W.M();
     	     wlvPartonHistory->auxdecor< float >( "MC_W_pt" ) = W.Pt();
     	     wlvPartonHistory->auxdecor< float >( "MC_W_phi" ) = W.Phi();
     	     fillEtaBranch(wlvPartonHistory, "MC_W_eta", W);
    

     	     wlvPartonHistory->auxdecor< float >( "MC_l_m" ) = WDecay1.M();
     	     wlvPartonHistory->auxdecor< float >( "MC_l_pt" ) = WDecay1.Pt();
     	     wlvPartonHistory->auxdecor< float >( "MC_l_phi" ) = WDecay1.Phi();
     	     wlvPartonHistory->auxdecor< int >( "MC_l_pdgId" ) = WDecay1_pdgId;
     	     fillEtaBranch(wlvPartonHistory,"MC_l_eta", WDecay1);

     	     wlvPartonHistory->auxdecor< float >( "MC_v_m" ) = WDecay2.M();
     	     wlvPartonHistory->auxdecor< float >( "MC_v_pt" ) = WDecay2.Pt();
     	     wlvPartonHistory->auxdecor< float >( "MC_v_phi" ) = WDecay2.Phi();
     	     wlvPartonHistory->auxdecor< int >( "MC_v_pdgId" ) = WDecay2_pdgId;
     	     fillEtaBranch(wlvPartonHistory,"MC_v_eta", WDecay2);
     }//if       
 
         
  }

  StatusCode CalcWlvPartonHistory::execute()
  {
     // Get the Truth Particles
     const xAOD::TruthParticleContainer* truthParticles(nullptr);
     ATH_CHECK( evtStore()->retrieve( truthParticles , m_config->sgKeyMCParticle() ) );

     // Create the partonHistory xAOD object
     xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer{};    
     xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer{};
     partonCont->setStore( partonAuxCont );
  
     xAOD::PartonHistory* wlvPartonHistory = new xAOD::PartonHistory{};
     partonCont->push_back( wlvPartonHistory );
     
     // Recover the parton history for wlv events     
     WlvHistorySaver(truthParticles, wlvPartonHistory);     
          
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

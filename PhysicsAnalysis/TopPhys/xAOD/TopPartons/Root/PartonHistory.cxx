/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PartonHistory.cxx 800464 2017-03-13 18:06:24Z tpelzer $
#include "TopPartons/PartonHistory.h"
#include "xAODCore/AddDVProxy.h"

namespace xAOD{
  /// Aux Container   
  PartonHistoryAuxContainer::PartonHistoryAuxContainer() :
    AuxContainerBase() 
  {
  }
  
  /// Interface class
  PartonHistory::PartonHistory() :
    SG::AuxElement()
  {        
  }

  //Initialize variables for ttbar events
  void PartonHistory::IniVarTtbar(){
	
   //ttbar variables  
   this->auxdecor< float >( "MC_ttbar_beforeFSR_m" ) = -1;
   this->auxdecor< float >( "MC_ttbar_beforeFSR_pt") = -1; 
   this->auxdecor< float >( "MC_ttbar_beforeFSR_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_ttbar_beforeFSR_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_ttbar_afterFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_ttbar_afterFSR_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_ttbar_afterFSR_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_ttbar_afterFSR_phi" ) = -1000 ; 

   //t variables  
   this->auxdecor< float >( "MC_t_beforeFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_t_beforeFSR_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_t_beforeFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_t_beforeFSR_phi" ) = -1000 ; 
   
   this->auxdecor< float >( "MC_t_afterFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_t_afterFSR_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_t_afterFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_t_afterFSR_phi" ) = -1000 ; 
   
   this->auxdecor< float >( "MC_t_afterFSR_SC_m" ) = -1 ; 
   this->auxdecor< float >( "MC_t_afterFSR_SC_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_t_afterFSR_SC_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_t_afterFSR_SC_phi" ) = -1000 ; 
   
   //tbar variables  
   this->auxdecor< float >( "MC_tbar_beforeFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_tbar_beforeFSR_pt" ) = -1 ;
   this->auxdecor< float >( "MC_tbar_beforeFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_tbar_beforeFSR_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_tbar_afterFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_tbar_afterFSR_pt" ) = -1 ;
   this->auxdecor< float >( "MC_tbar_afterFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_tbar_afterFSR_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_tbar_afterFSR_SC_m" ) = -1 ; 
   this->auxdecor< float >( "MC_tbar_afterFSR_SC_pt" ) = -1 ;
   this->auxdecor< float >( "MC_tbar_afterFSR_SC_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_tbar_afterFSR_SC_phi" ) = -1000 ; 
   
   this->auxdecor< float >( "MC_W_from_t_m" ) = -1 ; 
   this->auxdecor< float >( "MC_W_from_t_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_W_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_W_from_t_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_W_from_tbar_m" ) = -1 ; 
   this->auxdecor< float >( "MC_W_from_tbar_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_W_from_tbar_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_W_from_tbar_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_b_from_t_m" ) = -1 ; 
   this->auxdecor< float >( "MC_b_from_t_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_b_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_b_from_t_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_b_from_tbar_m" ) = -1 ;
   this->auxdecor< float >( "MC_b_from_tbar_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_b_from_tbar_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_b_from_tbar_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_Wdecay1_from_t_m" ) = -1 ;
   this->auxdecor< float >( "MC_Wdecay1_from_t_pt" ) = -1 ;
   this->auxdecor< float >( "MC_Wdecay1_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_Wdecay1_from_t_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_Wdecay1_from_t_pdgId") = 0 ; 

   this->auxdecor< float >( "MC_Wdecay1_from_tbar_m" ) = -1 ;
   this->auxdecor< float >( "MC_Wdecay1_from_tbar_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay1_from_tbar_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_Wdecay1_from_tbar_phi" ) = -1000 ;
   this->auxdecor< int >( "MC_Wdecay1_from_tbar_pdgId") = 0 ; 

   this->auxdecor< float >( "MC_Wdecay2_from_t_m" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_t_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_t_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_Wdecay2_from_t_pdgId") = 0 ; 

   this->auxdecor< float >( "MC_Wdecay2_from_tbar_m" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_tbar_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_tbar_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_Wdecay2_from_tbar_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_Wdecay2_from_tbar_pdgId") = 0 ;

  }

  //Initialize variables for tbbar events
  void PartonHistory::IniVarTbbar(){
	
   //ttbar variables  
   this->auxdecor< float >( "MC_tbbar_beforeFSR_m" ) = -1;
   this->auxdecor< float >( "MC_tbbar_beforeFSR_pt") = -1; 
   this->auxdecor< float >( "MC_tbbar_beforeFSR_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_tbbar_beforeFSR_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_tbbar_afterFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_tbbar_afterFSR_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_tbbar_afterFSR_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_tbbar_afterFSR_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_t_beforeFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_t_beforeFSR_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_t_beforeFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_t_beforeFSR_phi" ) = -1000 ; 
   
   //tbbar variables  
   this->auxdecor< float >( "MC_b_beforeFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_b_beforeFSR_pt" ) = -1 ;
   this->auxdecor< float >( "MC_b_beforeFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_b_beforeFSR_phi" ) = -1000 ; 
   
   this->auxdecor< float >( "MC_b_afterFSR_m" ) = -1 ; 
   this->auxdecor< float >( "MC_b_afterFSR_pt" ) = -1 ;
   this->auxdecor< float >( "MC_b_afterFSR_eta" ) = -1000 ;
   this->auxdecor< float >( "MC_b_afterFSR_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_W_from_t_m" ) = -1 ; 
   this->auxdecor< float >( "MC_W_from_t_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_W_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_W_from_t_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_b_from_t_m" ) = -1 ; 
   this->auxdecor< float >( "MC_b_from_t_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_b_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_b_from_t_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_Wdecay1_from_t_m" ) = -1 ;
   this->auxdecor< float >( "MC_Wdecay1_from_t_pt" ) = -1 ;
   this->auxdecor< float >( "MC_Wdecay1_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_Wdecay1_from_t_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_Wdecay1_from_t_pdgId") = 0 ; 

   this->auxdecor< float >( "MC_Wdecay2_from_t_m" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_t_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_t_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_Wdecay2_from_t_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_Wdecay2_from_t_pdgId") = 0 ; 

  }
  
    //Initialize variables for Wlv events
  void PartonHistory::IniVarWlv(){
	
   // W
   this->auxdecor< float >( "MC_W_m" ) = -1 ; 
   this->auxdecor< float >( "MC_W_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_W_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_W_phi" ) = -1000 ; 

   this->auxdecor< float >( "MC_l_m" ) = -1 ;
   this->auxdecor< float >( "MC_l_pt" ) = -1 ;
   this->auxdecor< float >( "MC_l_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_l_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_l_pdgId") = 0 ; 

   this->auxdecor< float >( "MC_v_m" ) = -1 ; 
   this->auxdecor< float >( "MC_v_pt" ) = -1 ; 
   this->auxdecor< float >( "MC_v_eta" ) = -1000 ; 
   this->auxdecor< float >( "MC_v_phi" ) = -1000 ; 
   this->auxdecor< int >( "MC_v_pdgId") = 0 ; 

  }


}
ADD_NS_DV_PROXY( xAOD , PartonHistoryContainer );

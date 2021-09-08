/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// $Id: PartonHistory.cxx 800464 2017-03-13 18:06:24Z tpelzer $
#include "TopPartons/PartonHistory.h"
#include "xAODCore/AddDVProxy.h"

namespace xAOD {
  /// Aux Container
  PartonHistoryAuxContainer::PartonHistoryAuxContainer() :
    AuxContainerBase() {
  }

  /// Interface class
  PartonHistory::PartonHistory() :
    SG::AuxElement() {
  }

  //Initialize variables for ttbar events
  void PartonHistory::IniVarTtbar() {
    //ttbar variables
    this->auxdecor< float >("MC_ttbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_ttbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_ttbar_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_ttbar_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_ttbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_ttbar_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_m") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_pt") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_eta") = -1000;
    this->auxdecor< float >("MC_ttbar_afterFSR_beforeDecay_phi") = -1000;

    //t variables
    this->auxdecor< float >("MC_t_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_m") = -1;
    this->auxdecor< float >("MC_t_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_t_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_SC_m") = -1;
    this->auxdecor< float >("MC_t_afterFSR_SC_pt") = -1;
    this->auxdecor< float >("MC_t_afterFSR_SC_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_phi") = -1000;

    //tbar variables
    this->auxdecor< float >("MC_tbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_tbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_tbar_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_tbar_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_tbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_tbar_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_tbar_afterFSR_SC_m") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_SC_pt") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_SC_eta") = -1000;
    this->auxdecor< float >("MC_tbar_afterFSR_SC_phi") = -1000;

    this->auxdecor< float >("MC_W_from_t_m") = -1;
    this->auxdecor< float >("MC_W_from_t_pt") = -1;
    this->auxdecor< float >("MC_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_W_from_t_phi") = -1000;

    this->auxdecor< float >("MC_W_from_tbar_m") = -1;
    this->auxdecor< float >("MC_W_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_W_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_W_from_tbar_phi") = -1000;

    this->auxdecor< float >("MC_b_from_t_m") = -1;
    this->auxdecor< float >("MC_b_from_t_pt") = -1;
    this->auxdecor< float >("MC_b_from_t_eta") = -1000;
    this->auxdecor< float >("MC_b_from_t_phi") = -1000;

    this->auxdecor< float >("MC_b_from_tbar_m") = -1;
    this->auxdecor< float >("MC_b_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_b_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_b_from_tbar_phi") = -1000;

    this->auxdecor< float >("MC_Wdecay1_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay1_from_tbar_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_tbar_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay2_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay2_from_tbar_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_tbar_pdgId") = 0;
  }

  //Initialize variables for ttbar events
  void PartonHistory::IniVarTtbarLight() {
    //ttbar variables
    this->auxdecor< float >("MC_ttbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_ttbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_ttbar_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_ttbar_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_ttbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_ttbar_afterFSR_phi") = -1000;

    //t variables
    this->auxdecor< float >("MC_t_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_m") = -1;
    this->auxdecor< float >("MC_t_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_t_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_SC_m") = -1;
    this->auxdecor< float >("MC_t_afterFSR_SC_pt") = -1;
    this->auxdecor< float >("MC_t_afterFSR_SC_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_phi") = -1000;

    //tbar variables
    this->auxdecor< float >("MC_tbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_tbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_tbar_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_tbar_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_tbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_tbar_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_tbar_afterFSR_SC_m") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_SC_pt") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_SC_eta") = -1000;
    this->auxdecor< float >("MC_tbar_afterFSR_SC_phi") = -1000;

    this->auxdecor< float >("MC_W_from_t_m") = -1;
    this->auxdecor< float >("MC_W_from_t_pt") = -1;
    this->auxdecor< float >("MC_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_W_from_t_phi") = -1000;

    this->auxdecor< float >("MC_W_from_tbar_m") = -1;
    this->auxdecor< float >("MC_W_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_W_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_W_from_tbar_phi") = -1000;

    this->auxdecor< float >("MC_q_from_t_m") = -1;
    this->auxdecor< float >("MC_q_from_t_pt") = -1;
    this->auxdecor< float >("MC_q_from_t_eta") = -1000;
    this->auxdecor< float >("MC_q_from_t_phi") = -1000;
    this->auxdecor< int >("MC_q_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_q_from_tbar_m") = -1;
    this->auxdecor< float >("MC_q_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_q_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_q_from_tbar_phi") = -1000;
    this->auxdecor< int >("MC_q_from_tbar_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay1_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay1_from_tbar_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_tbar_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay2_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay2_from_tbar_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_tbar_pdgId") = 0;
  }

  //Initialize variables for tbbar events
  void PartonHistory::IniVarTbbar() {
    //ttbar variables
    this->auxdecor< float >("MC_tbbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_tbbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_tbbar_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_tbbar_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_tbbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_tbbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_tbbar_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_tbbar_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_phi") = -1000;

    //tbbar variables
    this->auxdecor< float >("MC_b_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_b_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_b_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_b_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_b_afterFSR_m") = -1;
    this->auxdecor< float >("MC_b_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_b_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_b_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_W_from_t_m") = -1;
    this->auxdecor< float >("MC_W_from_t_pt") = -1;
    this->auxdecor< float >("MC_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_W_from_t_phi") = -1000;

    this->auxdecor< float >("MC_b_from_t_m") = -1;
    this->auxdecor< float >("MC_b_from_t_pt") = -1;
    this->auxdecor< float >("MC_b_from_t_eta") = -1000;
    this->auxdecor< float >("MC_b_from_t_phi") = -1000;

    this->auxdecor< float >("MC_Wdecay1_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay2_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = 0;
  }

  //Initialize variables for Wlv events
  void PartonHistory::IniVarWlv() {
    // W
    this->auxdecor< float >("MC_W_m") = -1;
    this->auxdecor< float >("MC_W_pt") = -1;
    this->auxdecor< float >("MC_W_eta") = -1000;
    this->auxdecor< float >("MC_W_phi") = -1000;

    this->auxdecor< float >("MC_l_m") = -1;
    this->auxdecor< float >("MC_l_pt") = -1;
    this->auxdecor< float >("MC_l_eta") = -1000;
    this->auxdecor< float >("MC_l_phi") = -1000;
    this->auxdecor< int >("MC_l_pdgId") = 0;

    this->auxdecor< float >("MC_v_m") = -1;
    this->auxdecor< float >("MC_v_pt") = -1;
    this->auxdecor< float >("MC_v_eta") = -1000;
    this->auxdecor< float >("MC_v_phi") = -1000;
    this->auxdecor< int >("MC_v_pdgId") = 0;
  }

  // Initialize variable for Wt ST events
  void PartonHistory::IniVarWtb() {
    // top or antitop
    this->auxdecor< float >("MC_top_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_top_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_top_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_top_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_top_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_top_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_top_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_top_afterFSR_m") = -1;

    // b from top
    this->auxdecor< float >("MC_b_from_top_pt") = -1;
    this->auxdecor< float >("MC_b_from_top_eta") = -1000;
    this->auxdecor< float >("MC_b_from_top_phi") = -1000;
    this->auxdecor< float >("MC_b_from_top_m") = -1;

    // W from top
    this->auxdecor< float >("MC_W_from_top_pt") = -1;
    this->auxdecor< float >("MC_W_from_top_eta") = -1000;
    this->auxdecor< float >("MC_W_from_top_phi") = -1000;
    this->auxdecor< float >("MC_W_from_top_m") = -1;

    // W from top decay product 1
    this->auxdecor< float >("MC_Wdecay1_from_top_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_top_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_top_phi") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_top_m") = -1;
    this->auxdecor< int >("MC_Wdecay1_from_top_pdgId") = 0;

    // W from top decay product 2
    this->auxdecor< float >("MC_Wdecay2_from_top_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_top_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_top_phi") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_top_m") = -1;
    this->auxdecor< int >("MC_Wdecay2_from_top_pdgId") = 0;

    // W NOT from top
    this->auxdecor< float >("MC_W_not_from_top_pt") = -1;
    this->auxdecor< float >("MC_W_not_from_top_eta") = -1000;
    this->auxdecor< float >("MC_W_not_from_top_phi") = -1000;
    this->auxdecor< float >("MC_W_not_from_top_m") = -1;

    // W NOT from top decay product 1
    this->auxdecor< float >("MC_Wdecay1_not_from_top_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_not_from_top_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_not_from_top_phi") = -1000;
    this->auxdecor< float >("MC_Wdecay1_not_from_top_m") = -1;
    this->auxdecor< int >("MC_Wdecay1_not_from_top_pdgId") = 0;

    // W NOT from top decay product 2
    this->auxdecor< float >("MC_Wdecay2_not_from_top_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_not_from_top_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_not_from_top_phi") = -1000;
    this->auxdecor< float >("MC_Wdecay2_not_from_top_m") = -1;
    this->auxdecor< int >("MC_Wdecay2_not_from_top_pdgId") = 0;

    // b not from top, for Wtb events
    this->auxdecor< float >("MC_b_not_from_top_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_b_not_from_top_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_b_not_from_top_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_b_not_from_top_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_b_not_from_top_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_b_not_from_top_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_b_not_from_top_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_b_not_from_top_afterFSR_m") = -1;
    this->auxdecor< int >("MC_b_not_from_top_pdgId") = 0;
  }
  
  
  	
  // Initialize variable for single top t-channel events
  void PartonHistory::IniVarTChannelSingleTop() {
    // top or antitop
    this->auxdecor< float >("MC_top_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_top_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_top_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_top_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_top_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_top_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_top_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_top_afterFSR_m") = -1;
    // b from top
    this->auxdecor< float >("MC_b_from_top_pt") = -1;
    this->auxdecor< float >("MC_b_from_top_eta") = -1000;
    this->auxdecor< float >("MC_b_from_top_phi") = -1000;
    this->auxdecor< float >("MC_b_from_top_m") = -1;
    // W from top
    this->auxdecor< float >("MC_W_from_top_pt") = -1;
    this->auxdecor< float >("MC_W_from_top_eta") = -1000;
    this->auxdecor< float >("MC_W_from_top_phi") = -1000;
    this->auxdecor< float >("MC_W_from_top_m") = -1;
    // W from top decay product 1
    this->auxdecor< float >("MC_Wdecay1_from_top_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_top_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_top_phi") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_top_m") = -1;
    this->auxdecor< int >("MC_Wdecay1_from_top_pdgId") = 0;
    // W from top decay product 2
    this->auxdecor< float >("MC_Wdecay2_from_top_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_top_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_top_phi") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_top_m") = -1;
    this->auxdecor< int >("MC_Wdecay2_from_top_pdgId") = 0;
    // spectatorquark
    this->auxdecor< int >("MC_spectatorquark_pdgId") = 0;
    this->auxdecor< int >("MC_spectatorquark_status") = 0;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_m") = -1;

    this->auxdecor< int >("MC_spectatorquark_method2_pdgId") = 0;
    this->auxdecor< int >("MC_spectatorquark_method2_status") = 0;
    this->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_spectatorquark_method2_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_spectatorquark_method2_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_spectatorquark_method2_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_spectatorquark_method2_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_spectatorquark_method2_afterFSR_m") = -1;
  }

  // Initialize variables for an additional final-state Z.
  void PartonHistory::IniVarZ() {
    this->auxdecor< float >("MC_Z_m") = -1;
    this->auxdecor< float >("MC_Z_pt") = -1;
    this->auxdecor< float >("MC_Z_eta") = -1000;
    this->auxdecor< float >("MC_Z_phi") = -1000;

    this->auxdecor< float >("MC_Zdecay1_m") = -1;
    this->auxdecor< float >("MC_Zdecay1_pt") = -1;
    this->auxdecor< float >("MC_Zdecay1_eta") = -1000;
    this->auxdecor< float >("MC_Zdecay1_phi") = -1000;
    this->auxdecor< int >("MC_Zdecay1_pdgId") = 0;

    this->auxdecor< float >("MC_Zdecay2_m") = -1;
    this->auxdecor< float >("MC_Zdecay2_pt") = -1;
    this->auxdecor< float >("MC_Zdecay2_eta") = -1000;
    this->auxdecor< float >("MC_Zdecay2_phi") = -1000;
    this->auxdecor< int >("MC_Zdecay2_pdgId") = 0;

    this->auxdecor< int >("MC_Z_AncestryCorrupted") = -1;
    this->auxdecor< int >("MC_Z_IsOnShell") = -1;
  }

  //Initialize variables for ttbargamma events
  void PartonHistory::IniVarTtGamma() {
    //ttbar variables
    this->auxdecor< float >("MC_ttbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_ttbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_ttbar_beforeFSR_eta") = -10;
    this->auxdecor< float >("MC_ttbar_beforeFSR_phi") = -10;

    this->auxdecor< float >("MC_ttbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_ttbar_afterFSR_eta") = -10;
    this->auxdecor< float >("MC_ttbar_afterFSR_phi") = -10;

    //t variables
    this->auxdecor< float >("MC_t_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_t_beforeFSR_eta") = -10;
    this->auxdecor< float >("MC_t_beforeFSR_phi") = -10;

    this->auxdecor< float >("MC_t_afterFSR_m") = -1;
    this->auxdecor< float >("MC_t_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_t_afterFSR_eta") = -10;
    this->auxdecor< float >("MC_t_afterFSR_phi") = -10;

    //tbar variables
    this->auxdecor< float >("MC_tbar_beforeFSR_m") = -1;
    this->auxdecor< float >("MC_tbar_beforeFSR_pt") = -1;
    this->auxdecor< float >("MC_tbar_beforeFSR_eta") = -10;
    this->auxdecor< float >("MC_tbar_beforeFSR_phi") = -10;

    this->auxdecor< float >("MC_tbar_afterFSR_m") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_pt") = -1;
    this->auxdecor< float >("MC_tbar_afterFSR_eta") = -10;
    this->auxdecor< float >("MC_tbar_afterFSR_phi") = -10;

    //W variables
    this->auxdecor< float >("MC_W_from_t_m") = -1;
    this->auxdecor< float >("MC_W_from_t_pt") = -1;
    this->auxdecor< float >("MC_W_from_t_eta") = -10;
    this->auxdecor< float >("MC_W_from_t_phi") = -10;

    this->auxdecor< float >("MC_W_from_tbar_m") = -1;
    this->auxdecor< float >("MC_W_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_W_from_tbar_eta") = -10;
    this->auxdecor< float >("MC_W_from_tbar_phi") = -10;

    //b variables
    this->auxdecor< float >("MC_b_from_t_m") = -1;
    this->auxdecor< float >("MC_b_from_t_pt") = -1;
    this->auxdecor< float >("MC_b_from_t_eta") = -10;
    this->auxdecor< float >("MC_b_from_t_phi") = -10;

    this->auxdecor< float >("MC_b_from_tbar_m") = -1;
    this->auxdecor< float >("MC_b_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_b_from_tbar_eta") = -10;
    this->auxdecor< float >("MC_b_from_tbar_phi") = -10;

    //Wdecay1 variables
    this->auxdecor< float >("MC_Wdecay1_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_t_eta") = -10;
    this->auxdecor< float >("MC_Wdecay1_from_t_phi") = -10;
    this->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay1_from_tbar_m") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_eta") = -10;
    this->auxdecor< float >("MC_Wdecay1_from_tbar_phi") = -10;
    this->auxdecor< int >("MC_Wdecay1_from_tbar_pdgId") = 0;

    //Wdecay2 variables
    this->auxdecor< float >("MC_Wdecay2_from_t_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_t_eta") = -10;
    this->auxdecor< float >("MC_Wdecay2_from_t_phi") = -10;
    this->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = 0;

    this->auxdecor< float >("MC_Wdecay2_from_tbar_m") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_pt") = -1;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_eta") = -10;
    this->auxdecor< float >("MC_Wdecay2_from_tbar_phi") = -10;
    this->auxdecor< int >("MC_Wdecay2_from_tbar_pdgId") = 0;

    //photon variables
    this->auxdecor< float >("MC_ph_m") = -1;
    this->auxdecor< float >("MC_ph_pt") = -1;
    this->auxdecor< float >("MC_ph_eta") = -10;
    this->auxdecor< float >("MC_ph_phi") = -10;

    this->auxdecor< int >("MC_branchtype_t") = -1;
    this->auxdecor< int >("MC_branchtype_tbar") = -1;
    this->auxdecor< int >("MC_initial_parton") = -1;
    this->auxdecor< int >("MC_ph_from_t_tbar") = -1;
    this->auxdecor< int >("MC_Event_Category") = -1;
  }

  // Initialize variables for thqtautau events
  void PartonHistory::IniVarThq() {
    //t variables
    this->auxdecor< float >("MC_t_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_SC_m") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_pt") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_phi") = -1000;

    this->auxdecor< float >("MC_W_from_t_m") = -1000;
    this->auxdecor< float >("MC_W_from_t_pt") = -1000;
    this->auxdecor< float >("MC_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_W_from_t_phi") = -1000;

    this->auxdecor< float >("MC_b_from_t_m") = -1000;
    this->auxdecor< float >("MC_b_from_t_pt") = -1000;
    this->auxdecor< float >("MC_b_from_t_eta") = -1000;
    this->auxdecor< float >("MC_b_from_t_phi") = -1000;

    this->auxdecor< float >("MC_Wdecay1_from_t_m") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_pt") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = -9999;

    this->auxdecor< float >("MC_Wdecay2_from_t_m") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_pt") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = -9999;

    this->auxdecor< float >("MC_tau_from_W_from_t_m") = -1000;
    this->auxdecor< float >("MC_tau_from_W_from_t_pt") = -1000;
    this->auxdecor< float >("MC_tau_from_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_tau_from_W_from_t_phi") = -1000;
    this->auxdecor< int >("MC_tau_from_W_from_t_isHadronic") = -9999;
    this->auxdecor< float >("MC_tauvis_from_W_from_t_m") = -1000;
    this->auxdecor< float >("MC_tauvis_from_W_from_t_pt") = -1000;
    this->auxdecor< float >("MC_tauvis_from_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_tauvis_from_W_from_t_phi") = -1000;


    // Higgs variables
    this->auxdecor< float >("MC_Higgs_m") = -1000;
    this->auxdecor< float >("MC_Higgs_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_phi") = -1000;

    this->auxdecor< float >("MC_Higgs_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_eta") = -1000;
    this->auxdecor< int >("MC_Higgs_decay1_pdgId") = -1000;
    
    this->auxdecor< int >("MC_Higgs_tau_decay1_isHadronic") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_eta") = -1000;
    
    this->auxdecor< float >("MC_Higgs_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_eta") = -1000;
    this->auxdecor< int >("MC_Higgs_decay2_pdgId") = -1000;

    this->auxdecor< int >("MC_Higgs_tau_decay2_isHadronic") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_eta") = -1000;    

    this->auxdecor< float >("MC_Higgs_decay1_from_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_from_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_from_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_from_decay1_eta") = -1000;
    this->auxdecor< int >("MC_Higgs_decay1_from_decay1_pdgId") = -1000;

    this->auxdecor< int >("MC_Higgs_tau_decay1_from_decay1_isHadronic") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay1_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay1_eta") = -1000;
    
    this->auxdecor< float >("MC_Higgs_decay2_from_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_from_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_from_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_from_decay1_eta") = -1000;
    this->auxdecor< int >("MC_Higgs_decay2_from_decay1_pdgId") = -1000;

    this->auxdecor< int >("MC_Higgs_tau_decay2_from_decay1_isHadronic") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay1_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay1_eta") = -1000;    

    this->auxdecor< float >("MC_Higgs_decay1_from_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_from_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_from_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_decay1_from_decay2_eta") = -1000;
    this->auxdecor< int >("MC_Higgs_decay1_from_decay2_pdgId") = -1000;

    this->auxdecor< int >("MC_Higgs_tau_decay1_from_decay2_isHadronic") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay1_from_decay2_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay1_from_decay2_eta") = -1000;
   

    this->auxdecor< float >("MC_Higgs_decay2_from_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_from_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_from_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_decay2_from_decay2_eta") = -1000;
    this->auxdecor< int >("MC_Higgs_decay2_from_decay2_pdgId") = -1000;

    this->auxdecor< int >("MC_Higgs_tau_decay2_from_decay2_isHadronic") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tau_decay2_from_decay2_eta") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_m") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_pt") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_phi") = -1000;
    this->auxdecor< float >("MC_Higgs_tauvis_decay2_from_decay2_eta") = -1000;

    // Spectator light quark
    this->auxdecor< int >("MC_spectatorquark_pdgId") = -1000;
    this->auxdecor< int >("MC_spectatorquark_status") = -1000;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_spectatorquark_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_spectatorquark_afterFSR_m") = -1000;
    
    // Second b-quark 
    this->auxdecor< float >("MC_secondb_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_secondb_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_secondb_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_secondb_beforeFSR_phi") = -1000;
    this->auxdecor< int >("MC_secondb_beforeFSR_pdgId") = -1000;
    this->auxdecor< float >("MC_secondb_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_secondb_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_secondb_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_secondb_afterFSR_phi") = -1000;
    this->auxdecor< int >("MC_secondb_afterFSR_pdgId") = -1000;
  }
  
  // Initialize variables for tZq events
  void PartonHistory::IniVarTzq() {
    //t variables
    this->auxdecor< float >("MC_t_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_beforeFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_phi") = -1000;

    this->auxdecor< float >("MC_t_afterFSR_SC_m") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_pt") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_eta") = -1000;
    this->auxdecor< float >("MC_t_afterFSR_SC_phi") = -1000;

    this->auxdecor< float >("MC_W_from_t_m") = -1000;
    this->auxdecor< float >("MC_W_from_t_pt") = -1000;
    this->auxdecor< float >("MC_W_from_t_eta") = -1000;
    this->auxdecor< float >("MC_W_from_t_phi") = -1000;

    this->auxdecor< float >("MC_b_from_t_m") = -1000;
    this->auxdecor< float >("MC_b_from_t_pt") = -1000;
    this->auxdecor< float >("MC_b_from_t_eta") = -1000;
    this->auxdecor< float >("MC_b_from_t_phi") = -1000;

    this->auxdecor< float >("MC_Wdecay1_from_t_m") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_pt") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay1_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay1_from_t_pdgId") = -9999;

    this->auxdecor< float >("MC_Wdecay2_from_t_m") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_pt") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_eta") = -1000;
    this->auxdecor< float >("MC_Wdecay2_from_t_phi") = -1000;
    this->auxdecor< int >("MC_Wdecay2_from_t_pdgId") = -9999;
    
    //Z variables
    this->auxdecor< float >("MC_Z_m") = -1000;
    this->auxdecor< float >("MC_Z_pt") = -1000;
    this->auxdecor< float >("MC_Z_eta") = -1000;
    this->auxdecor< float >("MC_Z_phi") = -1000;
   
    
    this->auxdecor< float >("MC_Zdecay1_m") = -1000;
    this->auxdecor< float >("MC_Zdecay1_pt") = -1000;
    this->auxdecor< float >("MC_Zdecay1_eta") = -1000;
    this->auxdecor< float >("MC_Zdecay1_phi") = -1000;
    this->auxdecor< int >("MC_Zdecay1_pdgId") = -9999;
    this->auxdecor< int >("MC_Zdecay1_status") = -9999;
    
    this->auxdecor< float >("MC_Zdecay2_m") = -1000;
    this->auxdecor< float >("MC_Zdecay2_pt") = -1000;
    this->auxdecor< float >("MC_Zdecay2_eta") = -1000;
    this->auxdecor< float >("MC_Zdecay2_phi") = -1000;
    this->auxdecor< int >("MC_Zdecay2_pdgId") = -9999;
    this->auxdecor< int >("MC_Zdecay2_status") = -9999;
        
 }

  // Initialize variables for tZq events
  void PartonHistory::IniVarTttt() {

    //top1 variables
    this->auxdecor< int >("MC_top1_pdgId") = -9999;
    this->auxdecor< float >("MC_top1_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_top1_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_top1_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_top1_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_top1_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_top1_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_top1_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_top1_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_top1_W_m") = -1000;
    this->auxdecor< float >("MC_top1_W_pt") = -1000;
    this->auxdecor< float >("MC_top1_W_eta") = -1000;
    this->auxdecor< float >("MC_top1_W_phi") = -1000;
    this->auxdecor< float >("MC_top1_b_m") = -1000;
    this->auxdecor< float >("MC_top1_b_pt") = -1000;
    this->auxdecor< float >("MC_top1_b_eta") = -1000;
    this->auxdecor< float >("MC_top1_b_phi") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay1_m") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay1_pt") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay1_eta") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay1_phi") = -1000;
    this->auxdecor< int >("MC_top1_Wdecay1_pdgId") = -9999;
    this->auxdecor< float >("MC_top1_Wdecay2_m") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay2_pt") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay2_eta") = -1000;
    this->auxdecor< float >("MC_top1_Wdecay2_phi") = -1000;
    this->auxdecor< int >("MC_top1_Wdecay2_pdgId") = -9999;

    //top2 variables
    this->auxdecor< int >("MC_top2_pdgId") = -9999;
    this->auxdecor< float >("MC_top2_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_top2_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_top2_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_top2_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_top2_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_top2_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_top2_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_top2_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_top2_W_m") = -1000;
    this->auxdecor< float >("MC_top2_W_pt") = -1000;
    this->auxdecor< float >("MC_top2_W_eta") = -1000;
    this->auxdecor< float >("MC_top2_W_phi") = -1000;
    this->auxdecor< float >("MC_top2_b_m") = -1000;
    this->auxdecor< float >("MC_top2_b_pt") = -1000;
    this->auxdecor< float >("MC_top2_b_eta") = -1000;
    this->auxdecor< float >("MC_top2_b_phi") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay1_m") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay1_pt") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay1_eta") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay1_phi") = -1000;
    this->auxdecor< int >("MC_top2_Wdecay1_pdgId") = -9999;
    this->auxdecor< float >("MC_top2_Wdecay2_m") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay2_pt") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay2_eta") = -1000;
    this->auxdecor< float >("MC_top2_Wdecay2_phi") = -1000;
    this->auxdecor< int >("MC_top2_Wdecay2_pdgId") = -9999;

    //top3 variables
    this->auxdecor< int >("MC_top3_pdgId") = -9999;
    this->auxdecor< float >("MC_top3_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_top3_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_top3_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_top3_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_top3_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_top3_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_top3_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_top3_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_top3_W_m") = -1000;
    this->auxdecor< float >("MC_top3_W_pt") = -1000;
    this->auxdecor< float >("MC_top3_W_eta") = -1000;
    this->auxdecor< float >("MC_top3_W_phi") = -1000;
    this->auxdecor< float >("MC_top3_b_m") = -1000;
    this->auxdecor< float >("MC_top3_b_pt") = -1000;
    this->auxdecor< float >("MC_top3_b_eta") = -1000;
    this->auxdecor< float >("MC_top3_b_phi") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay1_m") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay1_pt") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay1_eta") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay1_phi") = -1000;
    this->auxdecor< int >("MC_top3_Wdecay1_pdgId") = -9999;
    this->auxdecor< float >("MC_top3_Wdecay2_m") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay2_pt") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay2_eta") = -1000;
    this->auxdecor< float >("MC_top3_Wdecay2_phi") = -1000;
    this->auxdecor< int >("MC_top3_Wdecay2_pdgId") = -9999;

    //top4 variables
    this->auxdecor< int >("MC_top4_pdgId") = -9999;
    this->auxdecor< float >("MC_top4_beforeFSR_m") = -1000;
    this->auxdecor< float >("MC_top4_beforeFSR_pt") = -1000;
    this->auxdecor< float >("MC_top4_beforeFSR_eta") = -1000;
    this->auxdecor< float >("MC_top4_beforeFSR_phi") = -1000;
    this->auxdecor< float >("MC_top4_afterFSR_m") = -1000;
    this->auxdecor< float >("MC_top4_afterFSR_pt") = -1000;
    this->auxdecor< float >("MC_top4_afterFSR_eta") = -1000;
    this->auxdecor< float >("MC_top4_afterFSR_phi") = -1000;
    this->auxdecor< float >("MC_top4_W_m") = -1000;
    this->auxdecor< float >("MC_top4_W_pt") = -1000;
    this->auxdecor< float >("MC_top4_W_eta") = -1000;
    this->auxdecor< float >("MC_top4_W_phi") = -1000;
    this->auxdecor< float >("MC_top4_b_m") = -1000;
    this->auxdecor< float >("MC_top4_b_pt") = -1000;
    this->auxdecor< float >("MC_top4_b_eta") = -1000;
    this->auxdecor< float >("MC_top4_b_phi") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay1_m") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay1_pt") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay1_eta") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay1_phi") = -1000;
    this->auxdecor< int >("MC_top4_Wdecay1_pdgId") = -9999;
    this->auxdecor< float >("MC_top4_Wdecay2_m") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay2_pt") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay2_eta") = -1000;
    this->auxdecor< float >("MC_top4_Wdecay2_phi") = -1000;
    this->auxdecor< int >("MC_top4_Wdecay2_pdgId") = -9999;

    // Other parton-level variables
    this->auxdecor< float >("MC_mtttt") = -1000;
    this->auxdecor< float >("MC_HT") = -1000;
 }

}
ADD_NS_DV_PROXY(xAOD, PartonHistoryContainer);

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TriggerNavigationVariables_h__
#define __TriggerNavigationVariables_h__

#include <vector>
#include <limits>
#include <iostream>



class TriggerNavigationVariables
{
  public:
    TriggerNavigationVariables() :
      hasSetVariables(false),
      hasIsolationVariables(false),
      isFirstCall(true),
      trig_DB_SMK(0),
      trig_Nav_n(0),
      trig_Nav_chain_ChainId(0),
      trig_Nav_chain_RoIType(0),
      trig_Nav_chain_RoIIndex(0),
      /* electron */
      trig_RoI_EF_e_egammaContainer_egamma_Electrons(0),
      trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus(0),
      trig_EF_el_n(0),
      trig_EF_el_eta(0),
      trig_EF_el_phi(0),
      trig_EF_el_Et(0),
      /* muon */
      trig_RoI_EF_mu_Muon_ROI(0),
      trig_RoI_EF_mu_TrigMuonEFInfoContainer(0),
      trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus(0),
      trig_RoI_L2_mu_CombinedMuonFeature(0),
      trig_RoI_L2_mu_CombinedMuonFeatureStatus(0),
      trig_RoI_L2_mu_MuonFeature(0),
      trig_RoI_L2_mu_Muon_ROI(0),
      trig_EF_trigmuonef_track_CB_pt(0),
      trig_EF_trigmuonef_track_CB_eta(0),
      trig_EF_trigmuonef_track_CB_phi(0),
      trig_EF_trigmuonef_track_SA_pt(0),
      trig_EF_trigmuonef_track_SA_eta(0),
      trig_EF_trigmuonef_track_SA_phi(0),
      trig_EF_trigmugirl_track_CB_pt(0),
      trig_EF_trigmugirl_track_CB_eta(0),
      trig_EF_trigmugirl_track_CB_phi(0),
      trig_EF_trigmuonef_track_MuonType(0),
      trig_L2_combmuonfeature_eta(0),
      trig_L2_combmuonfeature_phi(0),
      trig_L2_muonfeature_eta(0),
      trig_L2_muonfeature_phi(0),
      trig_L1_mu_eta(0),
      trig_L1_mu_phi(0),
      trig_L1_mu_thrName(0),
      /* muon isolation */
      trig_RoI_EF_mu_TrigMuonEFIsolationContainer(0),
      trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus(0),
      /* workaround for TriggerMenuAnalysis-00-02-86 */
      trig_EF_trigmuonef_EF_mu8(0),
      trig_EF_trigmuonef_EF_mu15(0),
      trig_EF_trigmuonef_EF_mu24i_tight(0),
      trig_EF_trigmuonef_EF_mu36_tight(0),
      trig_EF_trigmuonef_EF_mu18_tight(0),
      trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS(0),
      trig_EF_trigmuonef_EF_2mu13(0),
      trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight(0),
      trig_EF_trigmuonef_EF_mu24_tight(0),
      trig_EF_trigmuonef_EF_mu20it_tight(0)
      {}

    TriggerNavigationVariables(
      unsigned int &trig_DB_SMK_,
      int &trig_Nav_n_,
      std::vector<short> *&trig_Nav_chain_ChainId_,
      std::vector<std::vector<int> > *&trig_Nav_chain_RoIType_,
      std::vector<std::vector<int> > *&trig_Nav_chain_RoIIndex_,
      /* electron */
      std::vector<std::vector<int> > *&trig_RoI_EF_e_egammaContainer_egamma_Electrons_,
      std::vector<std::vector<int> > *&trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus_,
      int &trig_EF_el_n_,
      std::vector<float> *&trig_EF_el_eta_,
      std::vector<float> *&trig_EF_el_phi_,
      std::vector<float> *&trig_EF_el_Et_,
      /* muon */
      std::vector<int> *&trig_RoI_EF_mu_Muon_ROI_,
      std::vector<std::vector<int> > *&trig_RoI_EF_mu_TrigMuonEFInfoContainer_,
      std::vector<std::vector<int> > *&trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus_,
      std::vector<int> *&trig_RoI_L2_mu_CombinedMuonFeature_,
      std::vector<int> *&trig_RoI_L2_mu_CombinedMuonFeatureStatus_,
      std::vector<int> *&trig_RoI_L2_mu_MuonFeature_,
      std::vector<int> *&trig_RoI_L2_mu_Muon_ROI_,
      std::vector<std::vector<float> > *&trig_EF_trigmuonef_track_CB_pt_,
      std::vector<std::vector<float> > *&trig_EF_trigmuonef_track_CB_eta_,
      std::vector<std::vector<float> > *&trig_EF_trigmuonef_track_CB_phi_,
      std::vector<std::vector<float> > *&trig_EF_trigmuonef_track_SA_pt_,
      std::vector<std::vector<float> > *&trig_EF_trigmuonef_track_SA_eta_,
      std::vector<std::vector<float> > *&trig_EF_trigmuonef_track_SA_phi_,
      std::vector<std::vector<float> > *&trig_EF_trigmugirl_track_CB_pt_,
      std::vector<std::vector<float> > *&trig_EF_trigmugirl_track_CB_eta_,
      std::vector<std::vector<float> > *&trig_EF_trigmugirl_track_CB_phi_,
      std::vector<float> *&trig_L2_combmuonfeature_eta_,
      std::vector<float> *&trig_L2_combmuonfeature_phi_,
      std::vector<float> *&trig_L2_muonfeature_eta_,
      std::vector<float> *&trig_L2_muonfeature_phi_,
      std::vector<float> *&trig_L1_mu_eta_,
      std::vector<float> *&trig_L1_mu_phi_,
      std::vector<std::string> *&trig_L1_mu_thrName_,
      /* muon isolation */
      std::vector<std::vector<int> > *&trig_RoI_EF_mu_TrigMuonEFIsolationContainer_,
      std::vector<std::vector<int> > *&trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus_
    ) :
      hasSetVariables(true),
      trig_DB_SMK(&trig_DB_SMK_),
      trig_Nav_n(&trig_Nav_n_),
      trig_Nav_chain_ChainId(trig_Nav_chain_ChainId_),
      trig_Nav_chain_RoIType(trig_Nav_chain_RoIType_),
      trig_Nav_chain_RoIIndex(trig_Nav_chain_RoIIndex_),
      /* electron */
      trig_RoI_EF_e_egammaContainer_egamma_Electrons(trig_RoI_EF_e_egammaContainer_egamma_Electrons_),
      trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus(trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus_),
      trig_EF_el_n(&trig_EF_el_n_),
      trig_EF_el_eta(trig_EF_el_eta_),
      trig_EF_el_phi(trig_EF_el_phi_),
      trig_EF_el_Et(trig_EF_el_Et_),
      /* muon */
      trig_RoI_EF_mu_Muon_ROI(trig_RoI_EF_mu_Muon_ROI_),
      trig_RoI_EF_mu_TrigMuonEFInfoContainer(trig_RoI_EF_mu_TrigMuonEFInfoContainer_),
      trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus(trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus_),
      trig_RoI_L2_mu_CombinedMuonFeature(trig_RoI_L2_mu_CombinedMuonFeature_),
      trig_RoI_L2_mu_CombinedMuonFeatureStatus(trig_RoI_L2_mu_CombinedMuonFeatureStatus_),
      trig_RoI_L2_mu_MuonFeature(trig_RoI_L2_mu_MuonFeature_),
      trig_RoI_L2_mu_Muon_ROI(trig_RoI_L2_mu_Muon_ROI_),
      trig_EF_trigmuonef_track_CB_pt(trig_EF_trigmuonef_track_CB_pt_),
      trig_EF_trigmuonef_track_CB_eta(trig_EF_trigmuonef_track_CB_eta_),
      trig_EF_trigmuonef_track_CB_phi(trig_EF_trigmuonef_track_CB_phi_),
      trig_EF_trigmuonef_track_SA_pt(trig_EF_trigmuonef_track_SA_pt_),
      trig_EF_trigmuonef_track_SA_eta(trig_EF_trigmuonef_track_SA_eta_),
      trig_EF_trigmuonef_track_SA_phi(trig_EF_trigmuonef_track_SA_phi_),
      trig_EF_trigmugirl_track_CB_pt(trig_EF_trigmugirl_track_CB_pt_),
      trig_EF_trigmugirl_track_CB_eta(trig_EF_trigmugirl_track_CB_eta_),
      trig_EF_trigmugirl_track_CB_phi(trig_EF_trigmugirl_track_CB_phi_),
      trig_L2_combmuonfeature_eta(trig_L2_combmuonfeature_eta_),
      trig_L2_combmuonfeature_phi(trig_L2_combmuonfeature_phi_),
      trig_L2_muonfeature_eta(trig_L2_muonfeature_eta_),
      trig_L2_muonfeature_phi(trig_L2_muonfeature_phi_),
      trig_L1_mu_eta(trig_L1_mu_eta_),
      trig_L1_mu_phi(trig_L1_mu_phi_),
      trig_L1_mu_thrName(trig_L1_mu_thrName_),
      /* muon isolation */
      trig_RoI_EF_mu_TrigMuonEFIsolationContainer(trig_RoI_EF_mu_TrigMuonEFIsolationContainer_),
      trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus(trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus_)
      {}

    ~TriggerNavigationVariables() {}
 

    void set_trig_DB_SMK(unsigned int &x) {trig_DB_SMK = &x;}
    void set_trig_Nav_n(int &x) {trig_Nav_n = &x;}
    void set_trig_Nav_chain_ChainId(std::vector<short> *&x) {trig_Nav_chain_ChainId = x;}
    void set_trig_Nav_chain_RoIType(std::vector<std::vector<int> > *&x) {trig_Nav_chain_RoIType = x;}
    void set_trig_Nav_chain_RoIIndex(std::vector<std::vector<int> > *&x) {trig_Nav_chain_RoIIndex = x;}

    /* electron */
    void set_trig_RoI_EF_e_egammaContainer_egamma_Electrons(std::vector<std::vector<int> > *&x) {trig_RoI_EF_e_egammaContainer_egamma_Electrons = x;}
    void set_trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus(std::vector<std::vector<int> > *&x) {trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus = x;}
    void set_trig_EF_el_n(int &x) {trig_EF_el_n = &x;}
    void set_trig_EF_el_eta(std::vector<float> *&x) {trig_EF_el_eta = x;}
    void set_trig_EF_el_phi(std::vector<float> *&x) {trig_EF_el_phi = x;}
    void set_trig_EF_el_Et(std::vector<float> *&x) {trig_EF_el_Et = x;}

    /* muon */
    void set_trig_RoI_EF_mu_Muon_ROI(std::vector<int> *&x) {trig_RoI_EF_mu_Muon_ROI = x;}
    void set_trig_RoI_EF_mu_TrigMuonEFInfoContainer(std::vector<std::vector<int> > *&x) {trig_RoI_EF_mu_TrigMuonEFInfoContainer = x;}
    void set_trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus(std::vector<std::vector<int> > *&x) {trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus = x;}
    void set_trig_RoI_L2_mu_CombinedMuonFeature(std::vector<int> *&x) {trig_RoI_L2_mu_CombinedMuonFeature = x;}
    void set_trig_RoI_L2_mu_CombinedMuonFeatureStatus(std::vector<int> *&x) {trig_RoI_L2_mu_CombinedMuonFeatureStatus = x;}
    void set_trig_RoI_L2_mu_MuonFeature(std::vector<int> *&x) {trig_RoI_L2_mu_MuonFeature = x;}
    void set_trig_RoI_L2_mu_Muon_ROI(std::vector<int> *&x) {trig_RoI_L2_mu_Muon_ROI = x;}
    void set_trig_EF_trigmuonef_track_CB_pt(std::vector<std::vector<float> > *&x) {trig_EF_trigmuonef_track_CB_pt = x;}
    void set_trig_EF_trigmuonef_track_CB_eta(std::vector<std::vector<float> > *&x) {trig_EF_trigmuonef_track_CB_eta = x;}
    void set_trig_EF_trigmuonef_track_CB_phi(std::vector<std::vector<float> > *&x) {trig_EF_trigmuonef_track_CB_phi = x;}
    void set_trig_EF_trigmuonef_track_SA_pt(std::vector<std::vector<float> > *&x) {trig_EF_trigmuonef_track_SA_pt = x;}
    void set_trig_EF_trigmuonef_track_SA_eta(std::vector<std::vector<float> > *&x) {trig_EF_trigmuonef_track_SA_eta = x;}
    void set_trig_EF_trigmuonef_track_SA_phi(std::vector<std::vector<float> > *&x) {trig_EF_trigmuonef_track_SA_phi = x;}
    void set_trig_EF_trigmugirl_track_CB_pt(std::vector<std::vector<float> > *&x) {trig_EF_trigmugirl_track_CB_pt = x;}
    void set_trig_EF_trigmugirl_track_CB_eta(std::vector<std::vector<float> > *&x) {trig_EF_trigmugirl_track_CB_eta = x;}
    void set_trig_EF_trigmugirl_track_CB_phi(std::vector<std::vector<float> > *&x) {trig_EF_trigmugirl_track_CB_phi = x;}
    void set_trig_EF_trigmuonef_track_MuonType(std::vector<std::vector<int> > *&x) {trig_EF_trigmuonef_track_MuonType = x;}
    void set_trig_L2_combmuonfeature_eta(std::vector<float> *&x) {trig_L2_combmuonfeature_eta = x;}
    void set_trig_L2_combmuonfeature_phi(std::vector<float> *&x) {trig_L2_combmuonfeature_phi = x;}
    void set_trig_L2_muonfeature_eta(std::vector<float> *&x) {trig_L2_muonfeature_eta = x;}
    void set_trig_L2_muonfeature_phi(std::vector<float> *&x) {trig_L2_muonfeature_phi = x;}
    void set_trig_L1_mu_eta(std::vector<float> *&x) {trig_L1_mu_eta = x;}
    void set_trig_L1_mu_phi(std::vector<float> *&x) {trig_L1_mu_phi = x;}
    void set_trig_L1_mu_thrName(std::vector<std::string> *&x) {trig_L1_mu_thrName = x;}
    /* muon isolation */
    void set_trig_RoI_EF_mu_TrigMuonEFIsolationContainer(std::vector<std::vector<int> > *&x) {trig_RoI_EF_mu_TrigMuonEFIsolationContainer = x;}
    void set_trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus(std::vector<std::vector<int> > *&x) {trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus = x;}

    /* workaround for TriggerMenuAnalysis-00-02-86 */
    void set_trig_EF_trigmuonef_EF_mu8(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu8 = x;}
    void set_trig_EF_trigmuonef_EF_mu15(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu15 = x;}
    void set_trig_EF_trigmuonef_EF_mu24i_tight(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu24i_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu36_tight(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu36_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu18_tight(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu18_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS = x;}
    void set_trig_EF_trigmuonef_EF_2mu13(std::vector<int> *&x) {trig_EF_trigmuonef_EF_2mu13 = x;}
    void set_trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu24_tight(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu24_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu20it_tight(std::vector<int> *&x) {trig_EF_trigmuonef_EF_mu20it_tight = x;}
    
    bool isValid() const {
      if (hasSetVariables) return true;

      if (trig_DB_SMK == 0) return false;
      if (trig_Nav_n == 0) return false;
      if (trig_Nav_chain_ChainId == 0) return false;
      if (trig_Nav_chain_RoIType == 0) return false;
      if (trig_Nav_chain_RoIIndex == 0) return false;
      /* electron */
      if (trig_RoI_EF_e_egammaContainer_egamma_Electrons == 0) return false;
      if (trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus == 0) return false;
      if (trig_EF_el_n == 0) return false;
      if (trig_EF_el_eta == 0) return false;
      if (trig_EF_el_phi == 0) return false;
      /* muon */
      if (trig_RoI_EF_mu_Muon_ROI == 0) return false;
      if (trig_RoI_EF_mu_TrigMuonEFInfoContainer == 0) return false;
      if (trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus == 0) return false;
      if (trig_RoI_L2_mu_CombinedMuonFeature == 0) return false;
      if (trig_RoI_L2_mu_CombinedMuonFeatureStatus == 0) return false;
      if (trig_RoI_L2_mu_MuonFeature == 0) return false;
      if (trig_RoI_L2_mu_Muon_ROI == 0) return false;
      if (trig_EF_trigmuonef_track_CB_pt == 0) return false;
      if (trig_EF_trigmuonef_track_CB_eta == 0) return false;
      if (trig_EF_trigmuonef_track_CB_phi == 0) return false;
      if (trig_EF_trigmuonef_track_SA_pt == 0) return false;
      if (trig_EF_trigmuonef_track_SA_eta == 0) return false;
      if (trig_EF_trigmuonef_track_SA_phi == 0) return false;
      if (trig_EF_trigmugirl_track_CB_pt == 0) return false;
      if (trig_EF_trigmugirl_track_CB_eta == 0) return false;
      if (trig_EF_trigmugirl_track_CB_phi == 0) return false;
      if (trig_L2_combmuonfeature_eta == 0) return false;
      if (trig_L2_combmuonfeature_phi == 0) return false;
      if (trig_L2_muonfeature_eta == 0) return false;
      if (trig_L2_muonfeature_phi == 0) return false;
      if (trig_L1_mu_eta == 0) return false;
      if (trig_L1_mu_phi == 0) return false;
      if (trig_L1_mu_thrName == 0) return false;

      /* for asymmetric di-electron trigger */
      if (trig_EF_el_Et == 0) {
        if (isFirstCall) std::cout << "INFORMATION: variables needed for asymmetric di-electron triggers not set "
                                      "(trig_EF_el_Et)\n";
      }

      /* for full-scan trigger in 2012 */
      if (trig_EF_trigmuonef_track_MuonType == 0) {
        if (isFirstCall) std::cout << "INFORMATION: variables needed for full scan di-muon triggers in 2012 not set "
                                      "(trig_EF_trigmuonef_track_MuonType)\n";
      }

      /* muon isolation */
      if ((trig_RoI_EF_mu_TrigMuonEFIsolationContainer == 0) or
          (trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus  == 0))
      {
        if (isFirstCall) std::cout << "INFORMATION: variables needed for isolated muon triggers not set "
                                      "(trig_RoI_EF_mu_TrigMuonEFIsolationContainer) and/or "
                                      "(trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus)\n";
      } else {
        hasIsolationVariables = true;
      }

      /* workaround for TriggerMenuAnalysis-00-02-86 */
      if ((trig_EF_trigmuonef_EF_mu24i_tight != 0) or
          (trig_EF_trigmuonef_EF_mu36_tight != 0) or
          (trig_EF_trigmuonef_EF_mu18_tight != 0) or
          (trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS != 0) or
          (trig_EF_trigmuonef_EF_2mu13 != 0) or
          (trig_EF_trigmuonef_EF_mu8 != 0) or
          (trig_EF_trigmuonef_EF_mu15 != 0) or
          (trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight != 0)) {
        if (isFirstCall) std::cout << "INFORMATION: variables needed for workaround for TriggerMenuAnalysis-00-02-86 available "
                                      "(trig_EF_trigmuonef_EF_mu24i_tight) and/or "
                                      "(trig_EF_trigmuonef_EF_mu36_tight) and/or "
                                      "(trig_EF_trigmuonef_EF_mu18_tight) and/or "
                                      "(trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS) and/or "
                                      "(trig_EF_trigmuonef_EF_2mu13) and/or "
                                      "(trig_EF_trigmuonef_EF_mu8) and/or "
                                      "(trig_EF_trigmuonef_EF_mu15) and/or "
                                      "(trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight)\n";
      }

      hasSetVariables = true;
      isFirstCall = false;
      return true;
    };



  private:
    bool mutable hasSetVariables;
    bool mutable hasIsolationVariables;
    bool mutable isFirstCall;

  public:
    const unsigned int *trig_DB_SMK;
    const int *trig_Nav_n;
    const std::vector<short> *trig_Nav_chain_ChainId;
    const std::vector<std::vector<int> > *trig_Nav_chain_RoIType;
    const std::vector<std::vector<int> > *trig_Nav_chain_RoIIndex;

    /* electron */
    const std::vector<std::vector<int> > *trig_RoI_EF_e_egammaContainer_egamma_Electrons;
    const std::vector<std::vector<int> > *trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus;
    const int *trig_EF_el_n;
    const std::vector<float> *trig_EF_el_eta;
    const std::vector<float> *trig_EF_el_phi;
    const std::vector<float> *trig_EF_el_Et;

    /* muon */
    const std::vector<int> *trig_RoI_EF_mu_Muon_ROI;
    const std::vector<std::vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer;
    const std::vector<std::vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus;
    const std::vector<int> *trig_RoI_L2_mu_CombinedMuonFeature;
    const std::vector<int> *trig_RoI_L2_mu_CombinedMuonFeatureStatus;
    const std::vector<int> *trig_RoI_L2_mu_MuonFeature;
    const std::vector<int> *trig_RoI_L2_mu_Muon_ROI;
    const std::vector<std::vector<float> > *trig_EF_trigmuonef_track_CB_pt;
    const std::vector<std::vector<float> > *trig_EF_trigmuonef_track_CB_eta;
    const std::vector<std::vector<float> > *trig_EF_trigmuonef_track_CB_phi;
    const std::vector<std::vector<float> > *trig_EF_trigmuonef_track_SA_pt;
    const std::vector<std::vector<float> > *trig_EF_trigmuonef_track_SA_eta;
    const std::vector<std::vector<float> > *trig_EF_trigmuonef_track_SA_phi;
    const std::vector<std::vector<float> > *trig_EF_trigmugirl_track_CB_pt;
    const std::vector<std::vector<float> > *trig_EF_trigmugirl_track_CB_eta;
    const std::vector<std::vector<float> > *trig_EF_trigmugirl_track_CB_phi;
    const std::vector<std::vector<int> > *trig_EF_trigmuonef_track_MuonType;
    const std::vector<float> *trig_L2_combmuonfeature_eta;
    const std::vector<float> *trig_L2_combmuonfeature_phi;
    const std::vector<float> *trig_L2_muonfeature_eta;
    const std::vector<float> *trig_L2_muonfeature_phi;
    const std::vector<float> *trig_L1_mu_eta;
    const std::vector<float> *trig_L1_mu_phi;
    const std::vector<std::string> *trig_L1_mu_thrName;

    /* muon isolation */
    const std::vector<std::vector<int> > *trig_RoI_EF_mu_TrigMuonEFIsolationContainer;
    const std::vector<std::vector<int> > *trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus;

    /* workaround for TriggerMenuAnalysis-00-02-86 */
    const std::vector<int> *trig_EF_trigmuonef_EF_mu8;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu15;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu24i_tight;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu36_tight;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu18_tight;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS;
    const std::vector<int> *trig_EF_trigmuonef_EF_2mu13;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu24_tight;
    const std::vector<int> *trig_EF_trigmuonef_EF_mu20it_tight;

#include "TriggerNavigationVariables.i"
};
#endif // __TriggerNavigationVariables_h__

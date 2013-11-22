#ifndef __TriggerNavigationVariables_i__
#define __TriggerNavigationVariables_i__

    //
    // variables & methods for python users
    //

    unsigned int trig_DB_SMK_python;
    int trig_Nav_n_python;
    std::vector<short> trig_Nav_chain_ChainId_python;
    std::vector<std::vector<int> > trig_Nav_chain_RoIType_python;
    std::vector<std::vector<int> > trig_Nav_chain_RoIIndex_python;

    /* electron */
    std::vector<std::vector<int> > trig_RoI_EF_e_egammaContainer_egamma_Electrons_python;
    std::vector<std::vector<int> > trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus_python;
    int trig_EF_el_n_python;
    std::vector<float> trig_EF_el_eta_python;
    std::vector<float> trig_EF_el_phi_python;
    std::vector<float> trig_EF_el_Et_python;

    /* muon */
    std::vector<int> trig_RoI_EF_mu_Muon_ROI_python;
    std::vector<std::vector<int> > trig_RoI_EF_mu_TrigMuonEFInfoContainer_python;
    std::vector<std::vector<int> > trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus_python;
    std::vector<int> trig_RoI_L2_mu_CombinedMuonFeature_python;
    std::vector<int> trig_RoI_L2_mu_CombinedMuonFeatureStatus_python;
    std::vector<int> trig_RoI_L2_mu_MuonFeature_python;
    std::vector<int> trig_RoI_L2_mu_Muon_ROI_python;
    std::vector<std::vector<float> > trig_EF_trigmuonef_track_CB_pt_python;
    std::vector<std::vector<float> > trig_EF_trigmuonef_track_CB_eta_python;
    std::vector<std::vector<float> > trig_EF_trigmuonef_track_CB_phi_python;
    std::vector<std::vector<float> > trig_EF_trigmuonef_track_SA_pt_python;
    std::vector<std::vector<float> > trig_EF_trigmuonef_track_SA_eta_python;
    std::vector<std::vector<float> > trig_EF_trigmuonef_track_SA_phi_python;
    std::vector<std::vector<float> > trig_EF_trigmugirl_track_CB_pt_python;
    std::vector<std::vector<float> > trig_EF_trigmugirl_track_CB_eta_python;
    std::vector<std::vector<float> > trig_EF_trigmugirl_track_CB_phi_python;
    std::vector<std::vector<int> > trig_EF_trigmuonef_track_MuonType_python;
    std::vector<float> trig_L2_combmuonfeature_eta_python;
    std::vector<float> trig_L2_combmuonfeature_phi_python;
    std::vector<float> trig_L2_muonfeature_eta_python;
    std::vector<float> trig_L2_muonfeature_phi_python;
    std::vector<float> trig_L1_mu_eta_python;
    std::vector<float> trig_L1_mu_phi_python;
    std::vector<std::string> trig_L1_mu_thrName_python;

    /* muon isolation */
    std::vector<std::vector<int> > trig_RoI_EF_mu_TrigMuonEFIsolationContainer_python;
    std::vector<std::vector<int> > trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus_python;

    /* workaround for TriggerMenuAnalysis-00-02-86 */
    std::vector<int> trig_EF_trigmuonef_EF_mu8_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu15_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu24i_tight_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu36_tight_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu18_tight_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS_python;
    std::vector<int> trig_EF_trigmuonef_EF_2mu13_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu24_tight_python;
    std::vector<int> trig_EF_trigmuonef_EF_mu20it_tight_python;


    void set_trig_DB_SMK_python(unsigned int x) {
      trig_DB_SMK_python = x;
      trig_DB_SMK        = &trig_DB_SMK_python;
      }
    void set_trig_Nav_n_python(int x) {
      trig_Nav_n_python = x;
      trig_Nav_n        = &trig_Nav_n_python;
      }
    void set_trig_Nav_chain_ChainId_python(std::vector<short> &x) {
      trig_Nav_chain_ChainId_python = x;
      trig_Nav_chain_ChainId        = &trig_Nav_chain_ChainId_python;
      }
    void set_trig_Nav_chain_RoIType_python(std::vector<std::vector<int> > &x) {
      trig_Nav_chain_RoIType_python = x;
      trig_Nav_chain_RoIType        = &trig_Nav_chain_RoIType_python;
      }
    void set_trig_Nav_chain_RoIIndex_python(std::vector<std::vector<int> > &x) {
      trig_Nav_chain_RoIIndex_python = x;
      trig_Nav_chain_RoIIndex        = &trig_Nav_chain_RoIIndex_python;
      }

    /* electron */
    void set_trig_RoI_EF_e_egammaContainer_egamma_Electrons_python(std::vector<std::vector<int> > &x) {
      trig_RoI_EF_e_egammaContainer_egamma_Electrons_python = x;
      trig_RoI_EF_e_egammaContainer_egamma_Electrons        = &trig_RoI_EF_e_egammaContainer_egamma_Electrons_python;
      }
    void set_trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus_python(std::vector<std::vector<int> > &x) {
      trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus_python = x;
      trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus        = &trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus_python;
      }
    void set_trig_EF_el_n_python(int x) {
      trig_EF_el_n_python = x;
      trig_EF_el_n        = &trig_EF_el_n_python;
      }
    void set_trig_EF_el_eta_python(std::vector<float> &x) {
      trig_EF_el_eta_python = x;
      trig_EF_el_eta        = &trig_EF_el_eta_python;
      }
    void set_trig_EF_el_phi_python(std::vector<float> &x) {
      trig_EF_el_phi_python = x;
      trig_EF_el_phi        = &trig_EF_el_phi_python;
      }
    void set_trig_EF_el_Et_python(std::vector<float> &x) {
      trig_EF_el_Et_python = x;
      trig_EF_el_Et        = &trig_EF_el_Et_python;
      }

    /* muon */
    void set_trig_RoI_EF_mu_Muon_ROI_python(std::vector<int> &x) {
      trig_RoI_EF_mu_Muon_ROI_python = x;
      trig_RoI_EF_mu_Muon_ROI        = &trig_RoI_EF_mu_Muon_ROI_python;
      }
    void set_trig_RoI_EF_mu_TrigMuonEFInfoContainer_python(std::vector<std::vector<int> > &x) {
      trig_RoI_EF_mu_TrigMuonEFInfoContainer_python = x;
      trig_RoI_EF_mu_TrigMuonEFInfoContainer        = &trig_RoI_EF_mu_TrigMuonEFInfoContainer_python;
      }
    void set_trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus_python(std::vector<std::vector<int> > &x) {
      trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus_python = x;
      trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus        = &trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus_python;
      }
    void set_trig_RoI_L2_mu_CombinedMuonFeature_python(std::vector<int> &x) {
      trig_RoI_L2_mu_CombinedMuonFeature_python = x;
      trig_RoI_L2_mu_CombinedMuonFeature        = &trig_RoI_L2_mu_CombinedMuonFeature_python;
      }
    void set_trig_RoI_L2_mu_CombinedMuonFeatureStatus_python(std::vector<int> &x) {
      trig_RoI_L2_mu_CombinedMuonFeatureStatus_python = x;
      trig_RoI_L2_mu_CombinedMuonFeatureStatus        = &trig_RoI_L2_mu_CombinedMuonFeatureStatus_python;
      }
    void set_trig_RoI_L2_mu_MuonFeature_python(std::vector<int> &x) {
      trig_RoI_L2_mu_MuonFeature_python = x;
      trig_RoI_L2_mu_MuonFeature        = &trig_RoI_L2_mu_MuonFeature_python;
      }
    void set_trig_RoI_L2_mu_Muon_ROI_python(std::vector<int> &x) {
      trig_RoI_L2_mu_Muon_ROI_python = x;
      trig_RoI_L2_mu_Muon_ROI        = &trig_RoI_L2_mu_Muon_ROI_python;
      }
    void set_trig_EF_trigmuonef_track_CB_pt_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmuonef_track_CB_pt_python = x;
      trig_EF_trigmuonef_track_CB_pt        = &trig_EF_trigmuonef_track_CB_pt_python;
      }
    void set_trig_EF_trigmuonef_track_CB_eta_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmuonef_track_CB_eta_python = x;
      trig_EF_trigmuonef_track_CB_eta        = &trig_EF_trigmuonef_track_CB_eta_python;
      }
    void set_trig_EF_trigmuonef_track_CB_phi_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmuonef_track_CB_phi_python = x;
      trig_EF_trigmuonef_track_CB_phi        = &trig_EF_trigmuonef_track_CB_phi_python;
      }
    void set_trig_EF_trigmuonef_track_SA_pt_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmuonef_track_SA_pt_python = x;
      trig_EF_trigmuonef_track_SA_pt        = &trig_EF_trigmuonef_track_SA_pt_python;
      }
    void set_trig_EF_trigmuonef_track_SA_eta_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmuonef_track_SA_eta_python = x;
      trig_EF_trigmuonef_track_SA_eta        = &trig_EF_trigmuonef_track_SA_eta_python;
      }
    void set_trig_EF_trigmuonef_track_SA_phi_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmuonef_track_SA_phi_python = x;
      trig_EF_trigmuonef_track_SA_phi        = &trig_EF_trigmuonef_track_SA_phi_python;
      }
    void set_trig_EF_trigmugirl_track_CB_pt_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmugirl_track_CB_pt_python = x;
      trig_EF_trigmugirl_track_CB_pt        = &trig_EF_trigmugirl_track_CB_pt_python;
      }
    void set_trig_EF_trigmugirl_track_CB_eta_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmugirl_track_CB_eta_python = x;
      trig_EF_trigmugirl_track_CB_eta        = &trig_EF_trigmugirl_track_CB_eta_python;
      }
    void set_trig_EF_trigmugirl_track_CB_phi_python(std::vector<std::vector<float> > &x) {
      trig_EF_trigmugirl_track_CB_phi_python = x;
      trig_EF_trigmugirl_track_CB_phi        = &trig_EF_trigmugirl_track_CB_phi_python;
      }
    void set_trig_EF_trigmuonef_track_MuonType_python(std::vector<std::vector<int> > &x) {
      trig_EF_trigmuonef_track_MuonType_python = x;
      trig_EF_trigmuonef_track_MuonType        = &trig_EF_trigmuonef_track_MuonType_python;
      }
    void set_trig_L2_combmuonfeature_eta_python(std::vector<float> &x) {
      trig_L2_combmuonfeature_eta_python = x;
      trig_L2_combmuonfeature_eta        = &trig_L2_combmuonfeature_eta_python;
      }
    void set_trig_L2_combmuonfeature_phi_python(std::vector<float> &x) {
      trig_L2_combmuonfeature_phi_python = x;
      trig_L2_combmuonfeature_phi        = &trig_L2_combmuonfeature_phi_python;
      }
    void set_trig_L2_muonfeature_eta_python(std::vector<float> &x) {
      trig_L2_muonfeature_eta_python = x;
      trig_L2_muonfeature_eta        = &trig_L2_muonfeature_eta_python;
      }
    void set_trig_L2_muonfeature_phi_python(std::vector<float> &x) {
      trig_L2_muonfeature_phi_python = x;
      trig_L2_muonfeature_phi        = &trig_L2_muonfeature_phi_python;
      }
    void set_trig_L1_mu_eta_python(std::vector<float> &x) {
      trig_L1_mu_eta_python = x;
      trig_L1_mu_eta        = &trig_L1_mu_eta_python;
      }
    void set_trig_L1_mu_phi_python(std::vector<float> &x) {
      trig_L1_mu_phi_python = x;
      trig_L1_mu_phi        = &trig_L1_mu_phi_python;
      }
    void set_trig_L1_mu_thrName_python(std::vector<std::string> &x) {
      trig_L1_mu_thrName_python = x;
      trig_L1_mu_thrName        = &trig_L1_mu_thrName_python;
      }
    /* muon isolation */
    void set_trig_RoI_EF_mu_TrigMuonEFIsolationContainer_python(std::vector<std::vector<int> > &x) {
      trig_RoI_EF_mu_TrigMuonEFIsolationContainer_python = x;
      trig_RoI_EF_mu_TrigMuonEFIsolationContainer        = &trig_RoI_EF_mu_TrigMuonEFIsolationContainer_python;
      }
    void set_trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus_python(std::vector<std::vector<int> > &x) {
      trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus_python = x;
      trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus        = &trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus_python;
      }

    /* workaround for TriggerMenuAnalysis-00-02-86 */
    void set_trig_EF_trigmuonef_EF_mu8_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu8_python = x;
      trig_EF_trigmuonef_EF_mu8        = &trig_EF_trigmuonef_EF_mu8_python;
      }
    void set_trig_EF_trigmuonef_EF_mu15_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu15_python = x;
      trig_EF_trigmuonef_EF_mu15        = &trig_EF_trigmuonef_EF_mu15_python;
      }
    void set_trig_EF_trigmuonef_EF_mu24i_tight_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu24i_tight_python = x;
      trig_EF_trigmuonef_EF_mu24i_tight        = &trig_EF_trigmuonef_EF_mu24i_tight_python;
      }
    void set_trig_EF_trigmuonef_EF_mu36_tight_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu36_tight_python = x;
      trig_EF_trigmuonef_EF_mu36_tight        = &trig_EF_trigmuonef_EF_mu36_tight_python;
      }
    void set_trig_EF_trigmuonef_EF_mu18_tight_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu18_tight_python = x;
      trig_EF_trigmuonef_EF_mu18_tight = &trig_EF_trigmuonef_EF_mu18_tight_python;
      }
    void set_trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS_python = x;
      trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS        = &trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS_python;
      }
    void set_trig_EF_trigmuonef_EF_2mu13_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_2mu13_python = x;
      trig_EF_trigmuonef_EF_2mu13        = &trig_EF_trigmuonef_EF_2mu13_python;
      }
    void set_trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight_python = x;
      trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight        = &trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight_python;
      }
    void set_trig_EF_trigmuonef_EF_mu24_tight_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu24_tight_python = x;
      trig_EF_trigmuonef_EF_mu24_tight        = &trig_EF_trigmuonef_EF_mu24_tight_python;
      }
    void set_trig_EF_trigmuonef_EF_mu20it_tight_python(std::vector<int> &x) {
      trig_EF_trigmuonef_EF_mu20it_tight_python = x;
      trig_EF_trigmuonef_EF_mu20it_tight        = &trig_EF_trigmuonef_EF_mu20it_tight_python;
      }

    
    void config_python() {};


    //
    //  methods for athena users
    //
    void set_trig_DB_SMK(const unsigned int *x) {trig_DB_SMK = x;}
    void set_trig_Nav_n(const int *x) {trig_Nav_n = x;}
    void set_trig_Nav_chain_ChainId(const std::vector<short> *x) {trig_Nav_chain_ChainId = x;}
    void set_trig_Nav_chain_RoIType(const std::vector<std::vector<int> > *x) {trig_Nav_chain_RoIType = x;}
    void set_trig_Nav_chain_RoIIndex(const std::vector<std::vector<int> > *x) {trig_Nav_chain_RoIIndex = x;}

    /* electron */
    void set_trig_RoI_EF_e_egammaContainer_egamma_Electrons(const std::vector<std::vector<int> > *x) {trig_RoI_EF_e_egammaContainer_egamma_Electrons = x;}
    void set_trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus(const std::vector<std::vector<int> > *x) {trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus = x;}
    void set_trig_EF_el_n(const int *x) {trig_EF_el_n = x;}
    void set_trig_EF_el_eta(const std::vector<float> *x) {trig_EF_el_eta = x;}
    void set_trig_EF_el_phi(const std::vector<float> *x) {trig_EF_el_phi = x;}
    void set_trig_EF_el_Et(const std::vector<float> *x) {trig_EF_el_Et = x;}

    /* muon */
    void set_trig_RoI_EF_mu_Muon_ROI(const std::vector<int> *x) {trig_RoI_EF_mu_Muon_ROI = x;}
    void set_trig_RoI_EF_mu_TrigMuonEFInfoContainer(const std::vector<std::vector<int> > *x) {trig_RoI_EF_mu_TrigMuonEFInfoContainer = x;}
    void set_trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus(const std::vector<std::vector<int> > *x) {trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus = x;}
    void set_trig_RoI_L2_mu_CombinedMuonFeature(const std::vector<int> *x) {trig_RoI_L2_mu_CombinedMuonFeature = x;}
    void set_trig_RoI_L2_mu_CombinedMuonFeatureStatus(const std::vector<int> *x) {trig_RoI_L2_mu_CombinedMuonFeatureStatus = x;}
    void set_trig_RoI_L2_mu_MuonFeature(const std::vector<int> *x) {trig_RoI_L2_mu_MuonFeature = x;}
    void set_trig_RoI_L2_mu_Muon_ROI(const std::vector<int> *x) {trig_RoI_L2_mu_Muon_ROI = x;}
    void set_trig_EF_trigmuonef_track_CB_pt(const std::vector<std::vector<float> > *x) {trig_EF_trigmuonef_track_CB_pt = x;}
    void set_trig_EF_trigmuonef_track_CB_eta(const std::vector<std::vector<float> > *x) {trig_EF_trigmuonef_track_CB_eta = x;}
    void set_trig_EF_trigmuonef_track_CB_phi(const std::vector<std::vector<float> > *x) {trig_EF_trigmuonef_track_CB_phi = x;}
    void set_trig_EF_trigmuonef_track_SA_pt(const std::vector<std::vector<float> > *x) {trig_EF_trigmuonef_track_SA_pt = x;}
    void set_trig_EF_trigmuonef_track_SA_eta(const std::vector<std::vector<float> > *x) {trig_EF_trigmuonef_track_SA_eta = x;}
    void set_trig_EF_trigmuonef_track_SA_phi(const std::vector<std::vector<float> > *x) {trig_EF_trigmuonef_track_SA_phi = x;}
    void set_trig_EF_trigmugirl_track_CB_pt(const std::vector<std::vector<float> > *x) {trig_EF_trigmugirl_track_CB_pt = x;}
    void set_trig_EF_trigmugirl_track_CB_eta(const std::vector<std::vector<float> > *x) {trig_EF_trigmugirl_track_CB_eta = x;}
    void set_trig_EF_trigmugirl_track_CB_phi(const std::vector<std::vector<float> > *x) {trig_EF_trigmugirl_track_CB_phi = x;}
    void set_trig_EF_trigmuonef_track_MuonType(const std::vector<std::vector<int> > *x) {trig_EF_trigmuonef_track_MuonType = x;}
    void set_trig_L2_combmuonfeature_eta(const std::vector<float> *x) {trig_L2_combmuonfeature_eta = x;}
    void set_trig_L2_combmuonfeature_phi(const std::vector<float> *x) {trig_L2_combmuonfeature_phi = x;}
    void set_trig_L2_muonfeature_eta(const std::vector<float> *x) {trig_L2_muonfeature_eta = x;}
    void set_trig_L2_muonfeature_phi(const std::vector<float> *x) {trig_L2_muonfeature_phi = x;}
    void set_trig_L1_mu_eta(const std::vector<float> *x) {trig_L1_mu_eta = x;}
    void set_trig_L1_mu_phi(const std::vector<float> *x) {trig_L1_mu_phi = x;}
    void set_trig_L1_mu_thrName(const std::vector<std::string> *x) {trig_L1_mu_thrName = x;}
    /* muon isolation */
    void set_trig_RoI_EF_mu_TrigMuonEFIsolationContainer(const std::vector<std::vector<int> > *x) {trig_RoI_EF_mu_TrigMuonEFIsolationContainer = x;}
    void set_trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus(const std::vector<std::vector<int> > *x) {trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus = x;}

    /* workaround for TriggerMenuAnalysis-00-02-86 */
    void set_trig_EF_trigmuonef_EF_mu8(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu8 = x;}
    void set_trig_EF_trigmuonef_EF_mu15(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu15 = x;}
    void set_trig_EF_trigmuonef_EF_mu24i_tight(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu24i_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu36_tight(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu36_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu18_tight(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu18_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS = x;}
    void set_trig_EF_trigmuonef_EF_2mu13(const std::vector<int> *x) {trig_EF_trigmuonef_EF_2mu13 = x;}
    void set_trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu24_tight(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu24_tight = x;}
    void set_trig_EF_trigmuonef_EF_mu20it_tight(const std::vector<int> *x) {trig_EF_trigmuonef_EF_mu20it_tight = x;}

#endif // __TriggerNavigationVariables_i__

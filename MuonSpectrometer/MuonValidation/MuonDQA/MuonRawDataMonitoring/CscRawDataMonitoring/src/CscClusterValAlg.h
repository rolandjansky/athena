/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscClusterValAlg_H
#define CscClusterValAlg_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CscClusterization/ICscStripFitter.h"
#include "CscCalibTools/ICscCalibTool.h"

#include <vector>
#include <string>

class TH1;
class TH1F;
class TH2F;

class CscClusterValAlg : public ManagedMonitorToolBase {

  public: 

    // constructor.
    CscClusterValAlg(const std::string &type, const std::string &name, const IInterface *parent);

    // destructor.
    ~CscClusterValAlg();

    // initialize
    StatusCode initialize();

    // book histograms
    virtual StatusCode bookHistograms();
     
    // fill histograms
    virtual StatusCode fillHistograms(); 

    // finalize
    virtual StatusCode procHistograms(){return StatusCode::SUCCESS;}

    float stripsSum_EA = 0.0F;
    float stripsSum_EAtest = 0.0F;
    float stripsSum_EC = 0.0F;
    float stripsSum_ECtest = 0.0F;

  private:

    // initialize histograms
    void initHistograms();
    
    // helper to fill histograms
    void FillCSCClusters( const Muon::CscPrepDataContainer* cols , const Muon::CscStripPrepDataContainer* strips );

    // register histograms
    void bookClusterHistograms();
    
    //Trigger aware monitoring
    bool evtSelTriggersPassed();

    // Properties.
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscClusterKey{this,"CSCClusterKey","CSC_Clusters","CSC clusters"};
    SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_cscPRDKey{this,"CSCPrepRawDataKey","CSC_Measurements","CSC PRDs"};
    std::string m_cscClusterPath, m_cscGenPath;
    unsigned int m_qmaxADCCut;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // Strip fitter.
    ToolHandle<ICscStripFitter> m_stripFitter;

    // calibration tool
    ToolHandle<ICscCalibTool> m_cscCalibTool;

    //!< TDT handle
    ToolHandle<Trig::TrigDecisionTool> m_trigDec; 

  private:

    std::vector<std::string> m_sampSelTriggers;
    bool m_doEvtSel;

    // hitmaps
    TH2F *m_h2csc_clus_hitmap = nullptr;
    TH2F *m_h2csc_clus_hitmap_noise = nullptr;
    TH2F *m_h2csc_clus_hitmap_signal = nullptr; 
    TH2F *m_h2csc_clus_segmap_signal = nullptr;
    
    TH2F *m_h2csc_clus_eta_vs_phi_hitmap = nullptr;
    TH2F *m_h2csc_clus_r_vs_z_hitmap = nullptr;
    TH2F *m_h2csc_clus_y_vs_x_hitmap = nullptr;

    // layer occupancy
    TH1F *m_h1csc_clus_occupancy_signal_EA = nullptr;
    TH1F *m_h1csc_clus_occupancy_signal_EC = nullptr;

    // q_max of cluster
    TH2F *m_h2csc_clus_qmax = nullptr;
    TH2F *m_h2csc_clus_qmax_noise = nullptr;
    TH2F *m_h2csc_clus_qmax_signal = nullptr;
    
    TH2F *m_h2csc_clus_qmax_signal_EA = nullptr;
    TH1F *m_h1csc_clus_qmax_signal_EA_count = nullptr;
    
    TH2F *m_h2csc_clus_qmax_signal_EC = nullptr;
    TH1F *m_h1csc_clus_qmax_signal_EC_count = nullptr;

    // q_sum = q_max + q_left + q_right of cluster
    TH2F *m_h2csc_clus_qsum = nullptr;
    TH2F *m_h2csc_clus_qsum_noise = nullptr;
    TH2F *m_h2csc_clus_qsum_signal = nullptr;
    
    TH2F *m_h2csc_clus_qsum_signal_EA = nullptr;
    TH1F *m_h1csc_clus_qsum_signal_EA_count = nullptr;
    TH1F *m_h1csc_clus_qsum_signal_EA_lfitmean = nullptr;
    
    TH2F *m_h2csc_clus_qsum_signal_EC = nullptr;
    TH1F *m_h1csc_clus_qsum_signal_EC_count = nullptr;
    TH1F *m_h1csc_clus_qsum_signal_EC_lfitmean = nullptr;

    // sampling time - eta cluster
    TH1F *m_h1csc_clus_precision_time = nullptr;
    TH1F *m_h1csc_clus_precision_time_noise = nullptr;
    TH1F *m_h1csc_clus_precision_time_signal = nullptr;
    TH1F *m_h1csc_clus_precision_time_signal_EA = nullptr;
    TH1F *m_h1csc_clus_precision_time_signal_EC = nullptr;

    // sampling time - phi cluster
    TH1F *m_h1csc_clus_transverse_time = nullptr;
    TH1F *m_h1csc_clus_transverse_time_noise = nullptr;
    TH1F *m_h1csc_clus_transverse_time_signal = nullptr;

    // cluster charge - eta cluster
    TH1F *m_h1csc_clus_precision_charge = nullptr;
    TH1F *m_h1csc_clus_precision_charge_noise = nullptr;
    TH1F *m_h1csc_clus_precision_charge_signal = nullptr;

    // cluster charge - phi cluster
    TH1F *m_h1csc_clus_transverse_charge = nullptr;
    TH1F *m_h1csc_clus_transverse_charge_noise = nullptr;
    TH1F *m_h1csc_clus_transverse_charge_signal = nullptr;

    // cluster count - phi layer // 
    TH2F *m_h2csc_clus_phicluscount = nullptr;           
    TH2F *m_h2csc_clus_phicluscount_signal = nullptr;    
    TH2F *m_h2csc_clus_phicluscount_noise = nullptr;     

    // cluster count - eta layer // 
    TH2F *m_h2csc_clus_etacluscount = nullptr;           
    TH2F *m_h2csc_clus_etacluscount_signal = nullptr;    
    TH2F *m_h2csc_clus_etacluscount_noise = nullptr;   

    TH1F *m_h1csc_clus_count = nullptr; 
    TH1F *m_h1csc_clus_count_signal = nullptr; 
    TH1F *m_h1csc_clus_count_noise = nullptr; 

    TH1F *m_h1csc_clus_count_perlayer = nullptr;

    // cluster width - eta layer
    TH2F *m_h2csc_clus_etacluswidth = nullptr;           
    TH2F *m_h2csc_clus_etacluswidth_signal = nullptr;    
    TH2F *m_h2csc_clus_etacluswidth_noise = nullptr;     

    // cluster width - phi layer
    TH2F *m_h2csc_clus_phicluswidth = nullptr;           
    TH2F *m_h2csc_clus_phicluswidth_signal = nullptr;    
    TH2F *m_h2csc_clus_phicluswidth_noise = nullptr;  

    //totaLl cluster width
    TH1F *m_h1csc_clus_totalWidth_EA = nullptr;
    TH1F *m_h1csc_clus_totalWidth_EC = nullptr;    


    // correlation plots
    // charge correlation b/w eta/phi clusters -- not done
    TH2F *m_h2csc_clus_eta_vs_phi_charge = nullptr;
    TH2F *m_h2csc_clus_eta_vs_phi_charge_noise = nullptr;
    TH2F *m_h2csc_clus_eta_vs_phi_charge_signal = nullptr;

    // hit correlation b/w eta/phi clusters
    TH2F *m_h2csc_clus_eta_vs_phi_cluscount = nullptr;
    TH2F *m_h2csc_clus_eta_vs_phi_cluscount_noise = nullptr;
    TH2F *m_h2csc_clus_eta_vs_phi_cluscount_signal = nullptr;

    // width correlation b/w eta/phi clusters
    TH2F *m_h2csc_clus_eta_vs_phi_cluswidth = nullptr;
    TH2F *m_h2csc_clus_eta_vs_phi_cluswidth_signal = nullptr;
    TH2F *m_h2csc_clus_eta_vs_phi_cluswidth_noise = nullptr;


    MonGroup *m_cscclus_oviewEA, *m_cscclus_oviewEC;

    std::vector<TH1*> m_cscClusExpert  , 
                      m_cscClusShift   , 
                      m_cscClusOviewEA , 
                      m_cscClusOviewEC ;

};

#endif

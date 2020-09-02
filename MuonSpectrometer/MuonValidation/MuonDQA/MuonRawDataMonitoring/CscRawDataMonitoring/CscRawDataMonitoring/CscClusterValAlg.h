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

    float stripsSum_EA;
    float stripsSum_EAtest;
    float stripsSum_EC;
    float stripsSum_ECtest;

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
    TH2F *m_h2csc_clus_hitmap;
    TH2F *m_h2csc_clus_hitmap_noise;
    TH2F *m_h2csc_clus_hitmap_signal; 
    TH2F *m_h2csc_clus_segmap_signal;
    
    TH2F *m_h2csc_clus_eta_vs_phi_hitmap;
    TH2F *m_h2csc_clus_r_vs_z_hitmap;
    TH2F *m_h2csc_clus_y_vs_x_hitmap;

    // layer occupancy
    TH1F *m_h1csc_clus_occupancy_signal_EA;
    TH1F *m_h1csc_clus_occupancy_signal_EC;

    // q_max of cluster
    TH2F *m_h2csc_clus_qmax;
    TH2F *m_h2csc_clus_qmax_noise;
    TH2F *m_h2csc_clus_qmax_signal;
    
    TH2F *m_h2csc_clus_qmax_signal_EA;
    TH1F *m_h1csc_clus_qmax_signal_EA_count;
    
    TH2F *m_h2csc_clus_qmax_signal_EC;
    TH1F *m_h1csc_clus_qmax_signal_EC_count;

    // q_sum = q_max + q_left + q_right of cluster
    TH2F *m_h2csc_clus_qsum;
    TH2F *m_h2csc_clus_qsum_noise;
    TH2F *m_h2csc_clus_qsum_signal;
    
    TH2F *m_h2csc_clus_qsum_signal_EA;
    TH1F *m_h1csc_clus_qsum_signal_EA_count;
    TH1F *m_h1csc_clus_qsum_signal_EA_lfitmean;
    
    TH2F *m_h2csc_clus_qsum_signal_EC;
    TH1F *m_h1csc_clus_qsum_signal_EC_count;
    TH1F *m_h1csc_clus_qsum_signal_EC_lfitmean;

    // sampling time - eta cluster
    TH1F *m_h1csc_clus_precision_time;
    TH1F *m_h1csc_clus_precision_time_noise;
    TH1F *m_h1csc_clus_precision_time_signal;
    TH1F *m_h1csc_clus_precision_time_signal_EA;
    TH1F *m_h1csc_clus_precision_time_signal_EC;

    // sampling time - phi cluster
    TH1F *m_h1csc_clus_transverse_time;
    TH1F *m_h1csc_clus_transverse_time_noise;
    TH1F *m_h1csc_clus_transverse_time_signal;

    // cluster charge - eta cluster
    TH1F *m_h1csc_clus_precision_charge;
    TH1F *m_h1csc_clus_precision_charge_noise;
    TH1F *m_h1csc_clus_precision_charge_signal;

    // cluster charge - phi cluster
    TH1F *m_h1csc_clus_transverse_charge;
    TH1F *m_h1csc_clus_transverse_charge_noise;
    TH1F *m_h1csc_clus_transverse_charge_signal;

    // cluster count - phi layer // 
    TH2F *m_h2csc_clus_phicluscount;           
    TH2F *m_h2csc_clus_phicluscount_signal;    
    TH2F *m_h2csc_clus_phicluscount_noise;     

    // cluster count - eta layer // 
    TH2F *m_h2csc_clus_etacluscount;           
    TH2F *m_h2csc_clus_etacluscount_signal;    
    TH2F *m_h2csc_clus_etacluscount_noise;   

    TH1F *m_h1csc_clus_count; 
    TH1F *m_h1csc_clus_count_signal; 
    TH1F *m_h1csc_clus_count_noise; 

    TH1F *m_h1csc_clus_count_perlayer;

    // cluster width - eta layer
    TH2F *m_h2csc_clus_etacluswidth;           
    TH2F *m_h2csc_clus_etacluswidth_signal;    
    TH2F *m_h2csc_clus_etacluswidth_noise;     

    // cluster width - phi layer
    TH2F *m_h2csc_clus_phicluswidth;           
    TH2F *m_h2csc_clus_phicluswidth_signal;    
    TH2F *m_h2csc_clus_phicluswidth_noise;  

    //totaLl cluster width
    TH1F *m_h1csc_clus_totalWidth_EA;
    TH1F *m_h1csc_clus_totalWidth_EC;    


    // correlation plots
    // charge correlation b/w eta/phi clusters -- not done
    TH2F *m_h2csc_clus_eta_vs_phi_charge;
    TH2F *m_h2csc_clus_eta_vs_phi_charge_noise;
    TH2F *m_h2csc_clus_eta_vs_phi_charge_signal;

    // hit correlation b/w eta/phi clusters
    TH2F *m_h2csc_clus_eta_vs_phi_cluscount;
    TH2F *m_h2csc_clus_eta_vs_phi_cluscount_noise;
    TH2F *m_h2csc_clus_eta_vs_phi_cluscount_signal;

    // width correlation b/w eta/phi clusters
    TH2F *m_h2csc_clus_eta_vs_phi_cluswidth;
    TH2F *m_h2csc_clus_eta_vs_phi_cluswidth_signal;
    TH2F *m_h2csc_clus_eta_vs_phi_cluswidth_noise;


    MonGroup *m_cscclus_oviewEA, *m_cscclus_oviewEC;

    std::vector<TH1*> m_cscClusExpert  , 
                      m_cscClusShift   , 
                      m_cscClusOviewEA , 
                      m_cscClusOviewEC ;

};

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscPrdValAlg_H
#define CscPrdValAlg_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CscClusterization/ICscStripFitter.h"

class TH1;
class TH1F;
class TH2F;

class CscPrdValAlg: public ManagedMonitorToolBase  {
  
 public:
  
  CscPrdValAlg (const std::string & type, const std::string & name,
		const IInterface* parent);
  ~CscPrdValAlg();

  StatusCode initialize();

  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode procHistograms(){return StatusCode::SUCCESS;}
  StatusCode checkHists(bool fromFinalise);

 private:

  // initialize histograms
  void initHistograms();

  // register histograms
  void bookPrdHistograms();

  size_t m_cscNoiseCut;
  SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_cscPrdKey{this,"CSCPrepRawDataKey","CSC_Measurements","CSC PRDs"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
  std::string m_cscPRDPath, m_cscGenPath;
  bool m_mapxyrz;

  // Strip fitter
  ToolHandle<ICscStripFitter> m_stripFitter;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  StatusCode fillLumiBlock();
  int m_lumiblock = 0;

 private:

  TH2F *m_h2csc_prd_hitmap = nullptr;                 // sector+0.2*layer vs. All hits
  TH2F *m_h2csc_prd_hitmap_signal = nullptr;          // sector+0.2*layer vs. Hits above threshold
  TH2F *m_h2csc_prd_hitmap_noise = nullptr;           // sector+0.2*layer vs. Hits below threshold

  TH2F *m_h2csc_prd_hitmap_norm = nullptr;                 // sector+0.2*layer vs. All hits
  TH2F *m_h2csc_prd_hitmap_norm_signal = nullptr;          // sector+0.2*layer vs. Hits above threshold
  TH2F *m_h2csc_prd_hitmap_norm_noise = nullptr;           // sector+0.2*layer vs. Hits below threshold

  TH2F *m_h2csc_prd_hitmap_signal_EA = nullptr;
  TH1F *m_h1csc_prd_hitmap_signal_EA_count = nullptr;
  TH1F *m_h1csc_prd_hitmap_signal_EA_occupancy = nullptr;

  TH2F *m_h2csc_prd_hitmap_norm_signal_EA = nullptr;

  TH2F *m_h2csc_prd_hitmap_signal_EC = nullptr;
  TH1F *m_h1csc_prd_hitmap_signal_EC_count = nullptr;
  TH1F *m_h1csc_prd_hitmap_signal_EC_occupancy = nullptr;

  TH2F *m_h2csc_prd_hitmap_norm_signal_EC = nullptr;

  TH2F *m_h2csc_prd_occvslb_EA = nullptr;
  TH2F *m_h2csc_prd_occvslb_EC = nullptr;

  TH2F *m_h2csc_prd_eta_vs_phi_hitmap = nullptr;
  TH2F *m_h2csc_prd_r_vs_z_hitmap = nullptr;
  TH2F *m_h2csc_prd_y_vs_x_hitmap = nullptr;

  TH2F *m_h2csc_prd_phicluswidth = nullptr;           // sector+0.2*layer vs. phi-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_phicluswidth_signal = nullptr;    // sector+0.2*layer vs. phi-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_phicluswidth_noise = nullptr;     // sector+0.2*layer vs. phi-cluster width (#of strips per cluster)

  TH2F *m_h2csc_prd_etacluswidth = nullptr;           // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_etacluswidth_signal = nullptr;    // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_etacluswidth_noise = nullptr;     // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)

  TH2F *m_h2csc_prd_phicluscount = nullptr;           // sector+0.2*layer vs. phi-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_phicluscount_signal = nullptr;    // sector+0.2*layer vs. phi-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_phicluscount_noise = nullptr;     // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)

  TH2F *m_h2csc_prd_etacluscount = nullptr;           // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_etacluscount_signal = nullptr;    // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_etacluscount_noise = nullptr;     // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)


  TH1F *m_h1csc_prd_maxdiffamp = nullptr;             // max amplitude per cluster (ADC count)


  MonGroup *m_cscprd_oviewEA, *m_cscprd_oviewEC;

  // Correlation plots
  TH2F *m_h2csc_prd_eta_vs_phi_cluscount = nullptr;
  TH2F *m_h2csc_prd_eta_vs_phi_cluscount_signal = nullptr;
  TH2F *m_h2csc_prd_eta_vs_phi_cluscount_noise = nullptr;

  TH2F *m_h2csc_prd_eta_vs_phi_cluswidth = nullptr;
  TH2F *m_h2csc_prd_eta_vs_phi_cluswidth_signal = nullptr;
  TH2F *m_h2csc_prd_eta_vs_phi_cluswidth_noise = nullptr;

  std::vector<TH1 *>    m_regHShift   , 
                        m_regHExpert  ,
                        m_regHOviewEA ,
                        m_regHOviewEC ,
                        m_regHLumi ;

};

#endif

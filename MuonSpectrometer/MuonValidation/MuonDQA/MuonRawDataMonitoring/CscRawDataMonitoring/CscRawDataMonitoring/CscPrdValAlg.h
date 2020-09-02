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
  int m_lumiblock;

 private:

  TH2F *m_h2csc_prd_hitmap;                 // sector+0.2*layer vs. All hits
  TH2F *m_h2csc_prd_hitmap_signal;          // sector+0.2*layer vs. Hits above threshold
  TH2F *m_h2csc_prd_hitmap_noise;           // sector+0.2*layer vs. Hits below threshold

  TH2F *m_h2csc_prd_hitmap_norm;                 // sector+0.2*layer vs. All hits
  TH2F *m_h2csc_prd_hitmap_norm_signal;          // sector+0.2*layer vs. Hits above threshold
  TH2F *m_h2csc_prd_hitmap_norm_noise;           // sector+0.2*layer vs. Hits below threshold

  TH2F *m_h2csc_prd_hitmap_signal_EA;
  TH1F *m_h1csc_prd_hitmap_signal_EA_count;
  TH1F *m_h1csc_prd_hitmap_signal_EA_occupancy;

  TH2F *m_h2csc_prd_hitmap_norm_signal_EA;

  TH2F *m_h2csc_prd_hitmap_signal_EC;
  TH1F *m_h1csc_prd_hitmap_signal_EC_count;
  TH1F *m_h1csc_prd_hitmap_signal_EC_occupancy;

  TH2F *m_h2csc_prd_hitmap_norm_signal_EC;

  TH2F *m_h2csc_prd_occvslb_EA;
  TH2F *m_h2csc_prd_occvslb_EC;

  TH2F *m_h2csc_prd_eta_vs_phi_hitmap;
  TH2F *m_h2csc_prd_r_vs_z_hitmap;
  TH2F *m_h2csc_prd_y_vs_x_hitmap;

  TH2F *m_h2csc_prd_phicluswidth;           // sector+0.2*layer vs. phi-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_phicluswidth_signal;    // sector+0.2*layer vs. phi-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_phicluswidth_noise;     // sector+0.2*layer vs. phi-cluster width (#of strips per cluster)

  TH2F *m_h2csc_prd_etacluswidth;           // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_etacluswidth_signal;    // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  TH2F *m_h2csc_prd_etacluswidth_noise;     // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)

  TH2F *m_h2csc_prd_phicluscount;           // sector+0.2*layer vs. phi-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_phicluscount_signal;    // sector+0.2*layer vs. phi-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_phicluscount_noise;     // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)

  TH2F *m_h2csc_prd_etacluscount;           // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_etacluscount_signal;    // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  TH2F *m_h2csc_prd_etacluscount_noise;     // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)


  TH1F *m_h1csc_prd_maxdiffamp;             // max amplitude per cluster (ADC count)


  MonGroup *m_cscprd_oviewEA, *m_cscprd_oviewEC;

  // Correlation plots
  TH2F *m_h2csc_prd_eta_vs_phi_cluscount;
  TH2F *m_h2csc_prd_eta_vs_phi_cluscount_signal;
  TH2F *m_h2csc_prd_eta_vs_phi_cluscount_noise;

  TH2F *m_h2csc_prd_eta_vs_phi_cluswidth;
  TH2F *m_h2csc_prd_eta_vs_phi_cluswidth_signal;
  TH2F *m_h2csc_prd_eta_vs_phi_cluswidth_noise;

  std::vector<TH1 *>    m_regHShift   , 
                        m_regHExpert  ,
                        m_regHOviewEA ,
                        m_regHOviewEC ,
                        m_regHLumi ;

};

#endif

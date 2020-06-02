/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscRdoValAlg_H
#define CscRdoValAlg_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

class TH1F;
class TH2F;

class CscRdoValAlg: public ManagedMonitorToolBase  
{

 public:
  
  CscRdoValAlg (const std::string & type, const std::string & name, 
		 const IInterface* parent);

  ~CscRdoValAlg();

  StatusCode initialize();

  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode procHistograms(){return StatusCode::SUCCESS;}
  StatusCode checkHists(bool fromFinalise);

 private:

  // initialize histograms
  void initHistograms();

  // register histograms
  void bookRdoHistograms();

  size_t m_cscNoiseCut;
  SG::ReadHandleKey<CscRawDataContainer> m_cscRdoKey{this,"CSCRawDataKey","CSCRDO","CSC RDO"};
  std::string m_cscRDOPath, m_cscGenPath;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  // CSC RDO Decoder
  ToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool;
     
 private:
 
  TH2F *m_h2csc_rdo_hitmap;                 // sector+0.2*layer vs. All hits
  TH2F *m_h2csc_rdo_hitmap_signal;          // sector+0.2*layer vs. Hits above threshold
  TH2F *m_h2csc_rdo_hitmap_noise;           // sector+0.2*layer vs. Hits below threshold
  
  TH2F *m_h2csc_rdo_hitmap_norm;                 // sector+0.2*layer vs. All hits
  TH2F *m_h2csc_rdo_hitmap_norm_signal;          // sector+0.2*layer vs. Hits above threshold
  TH2F *m_h2csc_rdo_hitmap_norm_noise;           // sector+0.2*layer vs. Hits below threshold
  
  TH2F *m_h2csc_rdo_hitmap_signal_EA;
  TH1F *m_h1csc_rdo_hitmap_signal_EA_count;
  TH1F *m_h1csc_rdo_hitmap_signal_EA_occupancy;
  
  TH2F *m_h2csc_rdo_hitmap_norm_signal_EA;
  
  TH2F *m_h2csc_rdo_hitmap_signal_EC;
  TH1F *m_h1csc_rdo_hitmap_signal_EC_count;
  TH1F *m_h1csc_rdo_hitmap_signal_EC_occupancy;
  
  TH2F *m_h2csc_rdo_hitmap_norm_signal_EC;
  
  TH2F *m_h2csc_rdo_phicluswidth;           // sector+0.2*layer vs. phi-cluster width (#of strips per cluster) 
  TH2F *m_h2csc_rdo_phicluswidth_signal;    // sector+0.2*layer vs. phi-cluster width (#of strips per cluster) 
  TH2F *m_h2csc_rdo_phicluswidth_noise;     // sector+0.2*layer vs. phi-cluster width (#of strips per cluster) 
  
  TH2F *m_h2csc_rdo_phicluswidth_signal_EA;
  TH1F *m_h1csc_rdo_phicluswidth_signal_EA_count;
  TH1F *m_h1csc_rdo_phicluswidth_signal_EA_occupancy;
  
  TH2F *m_h2csc_rdo_phicluswidth_signal_EC;
  TH1F *m_h1csc_rdo_phicluswidth_signal_EC_count;
  TH1F *m_h1csc_rdo_phicluswidth_signal_EC_occupancy;
  
  TH2F *m_h2csc_rdo_etacluswidth;           // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  TH2F *m_h2csc_rdo_etacluswidth_signal;    // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  TH2F *m_h2csc_rdo_etacluswidth_noise;     // sector+0.2*layer vs. eta-cluster width (#of strips per cluster)
  
  TH2F *m_h2csc_rdo_etacluswidth_signal_EA;
  TH1F *m_h1csc_rdo_etacluswidth_signal_EA_count;
  TH1F *m_h1csc_rdo_etacluswidth_signal_EA_occupancy;
  TH2F *m_h2csc_rdo_etacluswidth_signal_EC;
  TH1F *m_h1csc_rdo_etacluswidth_signal_EC_count;
  TH1F *m_h1csc_rdo_etacluswidth_signal_EC_occupancy;

  TH2F *m_h2csc_rdo_phicluscount;           // sector+0.2*layer vs. phi-cluster count (#of clusters per layer) 
  TH2F *m_h2csc_rdo_phicluscount_signal;    // sector+0.2*layer vs. phi-cluster count (#of clusters per layer) 
  TH2F *m_h2csc_rdo_phicluscount_noise;     // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  
  TH2F *m_h2csc_rdo_etacluscount;           // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  TH2F *m_h2csc_rdo_etacluscount_signal;    // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  TH2F *m_h2csc_rdo_etacluscount_noise;     // sector+0.2*layer vs. eta-cluster count (#of clusters per layer)
  

  TH1F *m_h1csc_rdo_maxdiffamp;             // max amplitude per cluster (ADC count)

  MonGroup *m_cscrdo_oviewEA, *m_cscrdo_oviewEC;

  // Correlation plots
  TH2F *m_h2csc_rdo_eta_vs_phi_cluscount;
  TH2F *m_h2csc_rdo_eta_vs_phi_cluscount_signal;
  TH2F *m_h2csc_rdo_eta_vs_phi_cluscount_noise;
  
  TH2F *m_h2csc_rdo_eta_vs_phi_cluswidth;
  TH2F *m_h2csc_rdo_eta_vs_phi_cluswidth_signal;

  std::vector<TH1 *>    m_regHShift   ,
                        m_regHExpert  ,
                        m_regHOviewEA ,
                        m_regHOviewEC ;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : TGCStandaloneTracksMon
// Author:   A.Ishikawa(Kobe)
// Sept. 2008
//
// DESCRIPTION:
// Subject: MDT TGC Correlation (moved from MuonRawMonitoring Feb.2009)
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGCStandaloneTracksMon_H
#define TGCStandaloneTracksMon_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "DataModel/DataLink.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonDQAUtils/TGCDQAUtils.h"
 
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

//#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
 
#include "TH1F.h"
#include "TH2F.h"
#include <sstream>
#include <string.h>
#include <vector>
 
class TFile;
class TgcIdHelper;
template <class ConcreteAlgorithm> class AlgFactory;
/////////////////////////////////////////////////////////////////////////////

class TGCStandaloneTracksMon: public ManagedMonitorToolBase {
 
 public:

  TGCStandaloneTracksMon ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~TGCStandaloneTracksMon();
  StatusCode initialize(); 
 
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:
    
  // Functions declaration
  bool findTGCTrack(const Muon::TgcPrepDataContainer* tgc_container,
                    const Muon::TgcCoinDataContainer* tgc_trigger_container,
                    LinearTrack& wireTrack);
  
  void checkMDT(const Muon::MdtPrepDataContainer* mdt_hit_container);

  void correlation(const Muon::MdtPrepDataContainer* mdt_hit_container,
                   const Muon::TgcCoinDataContainer* tgc_trigger_container,
                   LinearTrack& wireTrack,
                   bool findTrack);
  int numberOfSL(const Muon::TgcCoinDataContainer* tgctrgcontainer);

  // private function to convert local roi numbering to global eta, phi numbering
  void roi2etaphi(Muon::TgcCoinData& cd, //input
                  int& eta, int& phi);//output
  int phi2sector(int phi,int ef);
  int roiphi2mdtSector(int roiphi,int ef);
  int roitotalphi2sectorphi(int phi);
  int stationGasGap2layer(int station, int GasGap);
  bool LinearFit(std::vector<XYPosition>& positions,
                 LinearTrack& t);
  //double& a,
  //double& b,
  //double& chi2,
  //int& ndf);
  bool LinearFitLoop(std::vector<XYPosition>& positions,
                     double pchi2road,
                     double& a,
                     double& b,
                     double& largestpchi2,
                     double& chi2,
                     int& ndf,
                     int& nlayer);
  bool LinearFitLoop2(std::vector<XYPosition>& positions,
                      double pchi2road,
                      double& a,
                      double& b,
                      XYPosition*& largest,
                      double& chi2,
                      int& ndf,
                      int& nlayer,
                      int& nstation);

  void NoiseReductionForStation1(std::vector<XYPosition>& positions);

  MuonDQAHistMap m_stationHists;

  StoreGateSvc* m_eventStore;
  
  ActiveStoreSvc* m_activeStore;

  // Handle for the trig decision tool
  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;

  // Trigger items
  std::vector<std::string> m_muon_triggers;
  
  // user flag to switch trigger on or off
  bool m_useTrigger;
  bool m_triggerAware;

  mutable MsgStream      m_log;                  //!< Gaudi message stream - msgstream as private member (-> speed)
  bool                   m_debuglevel;           //!< private member to control debug messages  
 
  const MuonGM::MuonDetectorManager* m_muonMgr;

  const MdtIdHelper* m_mdtIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  
  //const ITGCcablingSvc* m_cabling;
    
  static const int numberOfTube[5][6][8];//[StationName][abs(StationEta)-1][StationPhi-1]
  
  double MDTz(int ac,
              int stationName,
              int multiLayer,
              int tubeLayer);


  //Declare Properties  
  
  //bool m_checkCabling;

  std::string m_key_mdt;
  std::string m_tgcPrepDataContainerName;
  std::string m_outputCoinCollectionLocation;

  int m_MdtAdcCut;
  int m_MdtTdcMaxCut;
  int m_MdtTdcMinCut;

  Muon::TgcCoinData* theSL;
  double SLr;
  double SLz;
  double SLeta;
  double SLphi;
  std::vector<Identifier> WireHits;
  std::vector<Identifier> StripHits;

  std::vector<XYPosition> wires;
  std::vector<XYPosition> strips;

  double MDTZpos[2][2][3];


  TH1F* mdtvstgclv1_mdt_adc;//MDT ADC
  TH1F* mdtvstgclv1_mdt_tdc;//MDT TDC
  TH1F* mdtvstgclv1_mdt_tdc_adc_cut;//MDT TDC

  TH2F* mdtvstgclv1_mdt_tdc_vs_adc;//MDT TDC vs ADC

  TH2* mdtvstgclv1_adcsector[2];//

  //correlation 
  TH2* mdtvstgclv1_eta[2];//eta correlation
  TH2* mdtvstgclv1_eta3st[2];//eta correlation
  TH2* mdtvstgclv1_eta3st_sector[2][16];//[side][sector]

  TH1* mdtvstgclv1_etadiff[2];
  TH1* mdtvstgclv1_etadiff3st[2];//eta correlation
  TH1* mdtvstgclv1_etadiff3st_sector[2][16];//eta correlation

  //correlation with ptcut
  TH2* mdtvstgclv1_eta_ptcut[2];//eta correlation
  TH2* mdtvstgclv1_eta3st_ptcut[2];//eta correlation
  TH2* mdtvstgclv1_eta3st_sector_ptcut[2][16];//[side][sector]

  TH1* mdtvstgclv1_etadiff_ptcut[2];
  TH1* mdtvstgclv1_etadiff3st_ptcut[2];//eta correlation
  TH1* mdtvstgclv1_etadiff3st_sector_ptcut[2][16];//eta correlation


  //tracking
  TH1* mdtvstgclv1_eff[2];

  TH1* mdtvstgclv1_nwire_around_roi[2];
  TH1* mdtvstgclv1_nwire_around_roi_ptcut[2];


  TH1* mdtvstgclv1_chi2overndf[2];
  TH1* mdtvstgclv1_chi2overndf_3st[2];

  TH1* mdtvstgclv1_chi2overndf_ptcut[2];
  TH1* mdtvstgclv1_chi2overndf_3st_ptcut[2];

  TH1* mdtvstgclv1_nhit[2];
  TH1* mdtvstgclv1_nhit_3st[2];

  TH1* mdtvstgclv1_nhit_ptcut[2];
  TH1* mdtvstgclv1_nhit_3st_ptcut[2];

  TH1* mdtvstgclv1_pull[2];
  TH1* mdtvstgclv1_pull3st[2];

  TH1* mdtvstgclv1_pull_ptcut[2];
  TH1* mdtvstgclv1_pull3st_ptcut[2];

  TH1* mdtvstgclv1_deltaeta[2][3];
  TH1* mdtvstgclv1_deltaeta3st[2][3];

  TH1* mdtvstgclv1_deltaeta_ptcut[2][3];
  TH1* mdtvstgclv1_deltaeta3st_ptcut[2][3];

  TH1* mdtvstgclv1_a[2];
  TH1* mdtvstgclv1_b[2];
  TH1* mdtvstgclv1_a_3st[2];
  TH1* mdtvstgclv1_b_3st[2];

  TH1* mdtvstgclv1_a_ptcut[2];
  TH1* mdtvstgclv1_b_ptcut[2];
  TH1* mdtvstgclv1_a_3st_ptcut[2];
  TH1* mdtvstgclv1_b_3st_ptcut[2];

  TH1* mdtvstgclv1_nlayer[2];
  TH1* mdtvstgclv1_nstation[2];
  TH2* mdtvstgclv1_nlayervsnhit[2];//

  TH1* mdtvstgclv1_nlayer_ptcut[2];
  TH1* mdtvstgclv1_nstation_ptcut[2];
  TH2* mdtvstgclv1_nlayervsnhit_ptcut[2];//



};

#endif
 

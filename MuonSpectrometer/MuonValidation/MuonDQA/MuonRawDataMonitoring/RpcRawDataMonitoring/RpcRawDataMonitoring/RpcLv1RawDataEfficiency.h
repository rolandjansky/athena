/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcLv1RawDataEfficiency
// Author:  U. Schnoor (ulrike.schnoor@cern.ch) - P. Anger (panger@cern.ch)
// September 2012
//
// DESCRIPTION:
// Subject: RPCLV1-->Efficiency Offline Muon Data Quality
// RPCLv1 Sector Hits vs LB
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RpcLv1RawDataEfficiency_H
#define RpcLv1RawDataEfficiency_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"

// STL includes
#include <sstream>
#include <string.h>
#include <vector>
#include <map>

// ROOT includes
#include <TH2I.h>
#include <inttypes.h> 

//================================================================================================================================
template <class ConcreteAlgorithm> class AlgFactory;

//================================================================================================================================
class OfflineMuon {
 public:
  OfflineMuon():m_pt(0.0),m_eta(0.0),m_phi(0.0),m_q(0.0) {
    m_pt_default_if_max = 50.1;
    m_pt_max = 50.;
  }
    ~OfflineMuon(){}

    // Getter
    float Pt()          const {return m_pt;}
    float TruncatedPt() const {return (m_pt > m_pt_max ? m_pt_default_if_max*m_q : m_pt); } 
    float Eta()         const {return m_eta;}
    float Phi()         const {return m_phi;}
    float Q()           const {return m_q;}
    int   QIndex()      const {return (m_q < 0 ? 1 : 0); }
    float PhiSeg()      const {return (m_phi<-CLHEP::pi/12. ? m_phi+2*CLHEP::pi : m_phi); }

    unsigned int ChargeIndex() const { return (m_q >= 0 ? 0 : 1); }

    // Setter
    void SetPtEtaPhiQ( float pt, float eta, float phi, float q) {
      SetPt(pt);
      SetEta(eta);
      SetPhi(phi);
      SetQ(q);}
    void SetPt ( float pt  ) { m_pt = pt;   }
    void SetEta( float eta ) { m_eta = eta; }
    void SetPhi( float phi ) { m_phi = phi; }
    void SetQ  ( float q   ) { m_q   = q;   }

    // Calculater
    float DeltaR( OfflineMuon mu2 ) const {
      return sqrt( (m_eta-mu2.Eta())*(m_eta-mu2.Eta())+(m_phi-mu2.Phi())*(m_phi-mu2.Phi()) ); }

 private:
    float m_pt;   // allways in GeV
    float m_eta;
    float m_phi;
    float m_q;

    float m_pt_default_if_max;
    float m_pt_max;
};


//================================================================================================================================
class CoincidenceData {
 public:
  CoincidenceData():m_lowpt(0),m_highpt(0),m_threshold(0),m_phi(0.0),m_eta(0.0){}
    ~CoincidenceData(){}

    // Getter
    bool  IsLow()        const { return ( (m_lowpt==1) ? true : false ); } 
    int   LowHighIndex() const { return ( (m_lowpt==1) ? 0 : 1 ); } // because only valid stuff is inside this works
    int   Threshold()    const { return m_threshold; }   // each coin matrix has three thresholds
    float Phi()          const { return m_phi; }
    float Eta()          const { return m_eta; }
  
    // Calculator
    float DeltaR(  OfflineMuon offmu ) const {
      float fDeltaEta = m_eta-offmu.Eta();
      float fDeltaPhi = acos(cos(m_phi-offmu.Phi()));
      return sqrt( fDeltaEta*fDeltaEta + fDeltaPhi*fDeltaPhi ); }

    // Setter
    void SetThresholdLowHigh( int threshold, int low_pt, int high_pt ) {
      m_threshold = threshold; 
      m_lowpt = low_pt;
      m_highpt = high_pt;}
    void SetEtaPhi( float eta, float phi ) {
      m_eta = eta;
      m_phi = phi;}

 private:
    int   m_lowpt;
    int   m_highpt;
    int   m_threshold;
    float m_phi;
    float m_eta;
};

// for sorting Trigger according to dR to offline muons, shortest dR first
struct CompareDR {
  CompareDR( const OfflineMuon& offmu) :  m_offmu(offmu) {}
  const OfflineMuon& m_offmu;
  bool operator()( const CoincidenceData* coin1, const CoincidenceData* coin2) {
    return (coin1->DeltaR( m_offmu ) < coin2->DeltaR( m_offmu ) ); }
};


//================================================================================================================================
class RpcLv1RawDataEfficiency: public ManagedMonitorToolBase {
 
 public:

  RpcLv1RawDataEfficiency ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~RpcLv1RawDataEfficiency()=default;

  StatusCode initialize(); 

  StatusCode readRpcCoinDataContainer();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms( );

 private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    
  
  // Trigger type stuff
  StatusCode StoreTriggerType();
  int GetTriggerType() { return m_trigtype; }
  // helper function for the different types of histograms
  std::stringstream m_ss; 

  //for Sector Hits histograms

  int m_trigtype;
  int m_event;
  int m_lumiblock;
  int m_BCID;

  bool m_isMC;

  SG::ReadHandleKey<Muon::RpcCoinDataContainer> m_rpcCoinKey{this,"RpcCoinKey","RPC_triggerHits","RPC coincidences"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","EventInfo"};
  SG::ReadHandleKey<RpcSectorLogicContainer> m_sectorLogicContainerKey{this,"RPCSec","RPC_SECTORLOGIC","RPC sector logic"};

  // muon informations for offline muons and trigger hits
  std::vector<OfflineMuon>   m_OfflineMuons;
  std::vector<CoincidenceData*> m_CoincidenceData;

  // Declare Histograms
  TH2I* m_rpclv1_sectorhits_A[6];
  TH2I* m_rpclv1_sectorhits_C[6];
  TH2I* m_rpclv1_sectorhits_all[6];

};

#endif

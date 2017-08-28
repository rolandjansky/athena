/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonGeoModel/MYSQL.h"

#include "muonEvent/MuonContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "AthenaBaseComps/AthAlgorithm.h"  

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

#include "MuonTrigCoinData/RpcCoinData.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataCollection.h"

// STL includes
#include <sstream>
#include <string.h>
#include <vector>
#include <map>


// ROOT includes
#include <TError.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 
#include "TGraphAsymmErrors.h"


//================================================================================================================================
class TFile;
class RpcIdHelper;
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
  virtual ~RpcLv1RawDataEfficiency();

  StatusCode initialize(); 
  StatusCode finalize();   

  StatusCode readRpcCoinDataContainer();

  // virtual StatusCode GetHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms( );
  virtual StatusCode procHistograms();  

 private:
  StatusCode readOfflineMuonContainer(std::string key);
  // Retrieving information and data
  ActiveStoreSvc* m_activeStore; // to get all the other information
  const RpcIdHelper* m_rpcIdHelper; 
  const RpcSectorLogicContainer* m_sectorLogicContainer; 
  const MuonGM::MuonDetectorManager* m_muonMgr; // to retrieve coincidence informations
  
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

  // muon informations for offline muons and trigger hits
  std::vector<OfflineMuon>   m_OfflineMuons;
  std::vector<CoincidenceData*> m_CoincidenceData;

  // Declare Histograms
  TH2I* m_rpclv1_sectorhits_A[6];
  TH2I* m_rpclv1_sectorhits_C[6];
  TH2I* m_rpclv1_sectorhits_all[6];

};

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Author:   A.Ishikawa(Kobe), M.King(Kobe)
// Jan.  2011
//
// DESCRIPTION:
// Subject: MDT TGC Correlation-->Offline Muon Data Quality  based on RPCLV1
//          TGC Efficiency     -->TGC Efficiency plots including EIFI by comparing with MDT Segments
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MdtVsTgcRawDataValAlg_H
#define MdtVsTgcRawDataValAlg_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"

#include "SegmTrack.h"

#include <string>
#include <vector>

template <class ConcreteAlgorithm> class AlgFactory;

class MdtVsTgcRawDataValAlg: public ManagedMonitorToolBase {
public:
  
  MdtVsTgcRawDataValAlg ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~MdtVsTgcRawDataValAlg();
  StatusCode initialize(); 

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
   
 private:
    
  // Functions declaration
  void correlation(const Muon::MdtPrepDataContainer* mdt_hit_container, const Muon::TgcCoinDataContainer* tgc_trigger_container);
  int numberOfSL(const Muon::TgcCoinDataContainer* tgctrgcontainer);

  // private function to convert local roi numbering to global eta, phi numbering
  void roi2etaphi(const Muon::TgcCoinData& cd, //input
                  int& eta, int& phi);//output
  int phi2sector(int phi,int ef);
  int roiphi2mdtSector(int roiphi,int ef);
  int roitotalphi2sectorphi(int phi);
  int stationGasGap2layer(int station, int GasGap);

  MuonDQAHistMap m_stationHists;

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  //Declare Properties  
  bool m_checkCabling;
  bool m_tgclv1file;

  std::string m_chamberName;
  std::string m_StationSize;
  int m_sector;
  int m_side;
  int m_lastEvent;
  int m_cosmicStation;
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgc_PrepDataContainerName{this,"TgcPrepDataContainer","TGC_Measurements","TGC PRDs"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_tgc_CoinContainerName{this,"OutputCoinCollection","TrigT1CoinDataCollection","TGC coincidences"};
  SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdt_PrepDataContainerName{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};
  SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_mdt_SegmentCollectionName{this,"MdtSegmentCollection","MuonSegments","muon segments"};

  int m_MdtAdcCut;
  int m_MdtTdcCut;

  //////////////////////////////////////////////////
  // Variables and Functions for TGC Efficiency Maps
  
  // Array of all TREs, indexed by location.  Used to find which sectors tracks pass through in tgceffcalc
  void prepareTREarray(const MuonGM::MuonDetectorManager* MuonDetMgrDS);
  const MuonGM::TgcReadoutElement* m_TREarray[8][2][9][49]; // [StationName][AC][StationEta][StationPhi]
  // Functions used to standardize the way TGC stations are indexed by the program
  int  TGCgetlayer(int stationName, int g);
  int  TGClayer2stationindex(int l);
  int  TGCstationname2stationindex(int stationName);
  
  /////////////////////////
  // Functions to Map TGC & MDT data
  StatusCode bookmaphists(MonGroup &mdtvstgclv1_expert_a,                   // Book Mapping histograms
                          MonGroup &mdtvstgclv1_expert_c);
  void maphists(const xAOD::MuonSegmentContainer *m_newsegment,             // Fills TGC&MDT data positions
                const Muon::TgcPrepDataContainer *tgc_prepcontainer);

  void maphistsfinalize();                                                  // Finalize histograms used in maphists
  // number of~ hists
  TH1 *m_mvt_cutspassed[2];        // [AC]
  // distributions
  TH2 *m_mdt_segmmap[2][4];        // [AC][StationIndex]
  
  /////////////////////////
  // Functions to Calculate Efficiency
  StatusCode bookeffhists(MonGroup &mdtvstgclv1_expert_a,                                  // Book Efficiency histograms
                          MonGroup &mdtvstgclv1_expert_c);
  void tgceffcalc(const xAOD::MuonSegmentContainer *m_newsegment,                      // Fills efficiency histograms using subsidiary functions
                  const Muon::TgcPrepDataContainer *tgc_prepcontainer);

  void SortMDTSegments(const xAOD::MuonSegmentContainer *m_newsegment,                     // Sorts MDT segments into stations
                       std::vector<const Muon::MuonSegment*>   (&sortedSegments)[2][4]);


  void DQCheckMDTSegments(std::vector<const Muon::MuonSegment*> (&sortedSegments)[2][4],              // Runs checks on DQ of segments
                          std::vector<const Muon::MuonSegment*> (&disqualifiedSegments)[2][4]);
  void MatchMDTSegments(std::vector<const Muon::MuonSegment*> (&sortedSegments)[2][4],                // Matches up segments in different stations into SegmTracks
                        std::vector<const Muon::MuonSegment*> (&disqualifiedSegments)[2][4],
                        std::vector<SegmTrack>          (&matchedSegments)[2]);
  void CheckTGConTrack(std::vector<SegmTrack>                (&matchedSegments)[2],             // Uses SegmTracks to look for TGC PRD 
                       const Muon::TgcPrepDataContainer *tgc_prepcontainer);
  void MidstationOnlyCheck(std::vector<const Muon::MuonSegment*>       (&sortedSegments)[2][4],       // Matches up segments in different stations into SegmTracks
                           std::vector<const Muon::MuonSegment*>       (&disqualifiedSegments)[2][4],
                           const Muon::TgcPrepDataContainer *tgc_prepcontainer);
  void tgceffcalcfinalize();                                                               // Finalize histograms used in tgceffcalc
  // Subsidiary functions used for managing StationMap style histograms
  int  getStationMapIndex(int x, int l, int stationFE, int stationEta, int stationPhi);
  void labelStationMap(TH2 *h2, int i=-1, int k=-1);
  void putBox(TH2* h2, float x1, float y1, float x2, float y2);
  void BlankPhi24(TH2 *h2, int binx);
  void BlankStationMap(TH2 *h2, int ws);
  
  // efficiencies
  TH2* m_eff_stationmapbase[2][2][4];     // [AC][WireStrip][EffNumDenomError]
  TH2* m_eff_stationmapmid[2][2][4];      // [AC][WireStrip][EffNumDenomError]
  TH2* m_eff_stationmap[2][2][4];         // [AC][WireStrip][EffNumDenomError] //Filled in postprocessor
  // sagittas
  TH1* m_mvt_extrprdsag[2][4][2][2][4];   // [AC][TGCStation][FE][WireStrip][RhoEtaPhi]
  TH1* m_mvt_extrprdsag2[2][4][2][2][4];  // [AC][TGCStation][FE][WireStrip][RhoEtaPhi]
  TH1* m_tgc_prdcompsag[2][2][4];         // [AC][TGCStation][RhoEtaPhiZ]
  TH1* m_mdt_segmmatchsag[2][4][4][4];    // [AC][MDTStation][MDTStation][RhoEtaPhiThe]
  TH1* m_mdt_segmposdirsag[2][4][4];      // [AC][MDTStation][RhoEtaPhiThe]
  TH1* m_mdt_trackdirdirsag[2][4][4][4];  // [AC][MDTStation][MDTStation][RhoEtaPhiThe]
  TH1* m_mdt_trackchecksag[2][4][4][4][2];// [AC][MDTStation][MDTStation][RhoEtaPhiThe]
};

#endif
 

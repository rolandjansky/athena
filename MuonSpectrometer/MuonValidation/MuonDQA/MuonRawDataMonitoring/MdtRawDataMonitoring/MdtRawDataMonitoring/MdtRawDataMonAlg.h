
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtRawDataMonitoring
// Author:   N. Benekos(Illinois) - G. Dedes(MPI) - Orin Harris (University of Washington) 
// Author:   M. Biglietti (INFN Roma3)
//
// DESCRIPTION:
// Subject: MDT-->Offline Muon Data Quality
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef MdtRawDataMonAlg_H
#define MdtRawDataMonAlg_H

//Core Include
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/ToolHandle.h" 
#include "AsgTools/ToolHandleArray.h"

//Helper Includes
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"
#include "MdtRawDataMonitoring/MDTNoisyTubes.h"
#include "MdtRawDataMonitoring/MDTChamber.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkSegment/SegmentCollection.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
//standard library includes
#include <fstream> 
#include <cstdlib>
#include <iostream>

namespace Muon {
  class MdtPrepData;
}

//stl includes
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

//root includes
class TH2;
struct MDTOverviewHistogramStruct;
struct MDTSummaryHistogramStruct;
struct MDTSegmentHistogramStruct;

namespace monAlg{
  enum {L1_UNKNOWN, L1_BARREL, L1_ENDCAP};


  class TubeTraversedBySegment {
  public:
    TubeTraversedBySegment(std::string hn, int tb, bool ih, IdentifierHash idh) 
      { hardware_name = hn; tubeBin = tb; isHit = ih; idHash = idh; }
    std::string hardware_name;
    int tubeBin;
    bool isHit;
    IdentifierHash idHash;
  };
  
  // Be careful here -- changes to this can break whether insertions to set are unique 
  // JG remove expensive hardware_name comparison w/ idHash comparison
  // Hope I was careful ;)
  struct TubeTraversedBySegment_cmp {
    bool operator()(const TubeTraversedBySegment & A, const TubeTraversedBySegment & B) const {
      if( A.idHash > B.idHash ) {
	return true;
      }
      else {
      if( (A.tubeBin > B.tubeBin) && (A.idHash == B.idHash) ) {
        return true;
      }
      else {
        if( (A.isHit > B.isHit) && (A.tubeBin == B.tubeBin) && (A.idHash == B.idHash) ) {
          return true;
        }
      }
    }
      return false;
    }
  };
}

//template <class ConcreteAlgorithm> class AlgFactory; 
class MdtRawDataMonAlg: public AthMonitorAlgorithm {
 public:
  
  MdtRawDataMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
 
  virtual ~MdtRawDataMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx ) const override;     

 private: 

  TH2* m_mdthitspermultilayerLumi[4][4];
  TH2* m_mdthitsperchamber_InnerMiddleOuterLumi[2];
  TH2* m_mdthitsperML_byLayer[3];//These are alternative Global hit coverage plots

  MDTNoisyTubes* m_masked_tubes;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  virtual void  fillMDTOverviewVects(const Muon::MdtPrepData*, bool &isNoiseBurstCandidate, MDTOverviewHistogramStruct& vects) const;
  virtual void  fillMDTOverviewHistograms(const MDTOverviewHistogramStruct& vects) const;
  virtual StatusCode  fillMDTSummaryVects( const Muon::MdtPrepData*, const std::set<std::string>&, bool &isNoiseBurstCandidate, bool trig_barrel, bool trig_endcap, MDTSummaryHistogramStruct (&vects)[4][4][16][4][4] ) const;
  virtual StatusCode  fillMDTSummaryHistograms( const MDTSummaryHistogramStruct (&vects)[4][4][16][4][4], int lb) const;
  virtual StatusCode  fillMDTHistograms( const Muon::MdtPrepData* ) const;//fill chamber by chamber histos


  StatusCode handleEvent_effCalc_fillVects(const Trk::SegmentCollection* segms, MDTSegmentHistogramStruct (&vects)[4][4][16]) const;
  virtual StatusCode  fillMDTSegmentHistograms( const MDTSegmentHistogramStruct (&vects)[4][4][16]) const;

  //MDTRawDataUtils_cxx
  bool AinB( int A, std::vector<int> & B ) const;
  virtual StatusCode  binMdtGlobal( TH2* &, char ecap );
  virtual StatusCode  binMdtRegional( TH2* &, std::string &xAxis);
  virtual StatusCode  binMdtGlobal_byLayer( TH2*, TH2*, TH2*);
  virtual StatusCode binMdtOccVsLB(TH2* &h, int region, int layer);
  virtual StatusCode binMdtOccVsLB_Crate(TH2* &h, int region, int crate);
  void ChamberTubeNumberCorrection(int & tubeNum, const std::string & hardware_name, int tubePos, int numLayers) const;
  void CorrectTubeMax(const std::string & hardware_name, int & numTubes) const;
  void CorrectLayerMax(const std::string & hardware_name, int & numLayers) const;
  virtual StatusCode  fillMDTMaskedTubes(IdentifierHash, const std::string &, TH1F_LW*& h);//DEV not used at moment, should be revised
  int get_bin_for_LB_hist(int region, int layer, int phi, int eta, bool isBIM) const;
  int get_bin_for_LB_crate_hist(int region, int layer, int phi, int eta, std::string chamber) const;
  // private function to initialize the selection of a certain region
  void mdtchamberId();    
  //private function to find mdt mezz cards
  int mezzmdt(Identifier) const;
  int GetTubeMax(const Identifier & digcoll_id, const std::string & hardware_name);

  bool isATLASReady() { return m_atlas_ready; }
  void setIsATLASReady();
  StatusCode GetTimingInfo();//here
  void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

  bool m_atlas_ready;

  SG::ReadHandleKey<Trk::SegmentCollection> m_segm_type{this,"Eff_segm_type","MuonSegments","muon segments"};

  std::string returnString(int i) const{
    std::stringstream ss;
    std::string s;
    ss << i;
    ss >> s;
    return s;
  }

  std::vector<Identifier> m_chambersId;
  std::vector<IdentifierHash> m_chambersIdHash;
  //  std::map<std::string,float> m_hitsperchamber_map;//DEV to be put back?
  std::map<std::string,int> m_tubesperchamber_map; 

  bool m_doMdtESD ; 

  // to book or not bookMDTTDCplots -->   /Chambers/tmp/ directory
  bool m_doChamberHists;
  bool m_maskNoisyTubes;

  SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_key_mdt{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc{this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_l1RoiKey{this,"L1RoiKey","LVL1MuonRoIs","L1 muon ROIs"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey{this,"MuonKey","Muons","muons"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};

  //Define configurable adccut and TGC/RPC keys
  float m_ADCCut;

  //Chamber by Chamber Plots
  std::vector< MDTChamber* >* m_hist_hash_list;
  void clear_hist_map(bool reallocate=true);
  
  std::string getChamberName(const Muon::MdtPrepData*) const;
  std::string getChamberName(Identifier) const;
  StatusCode getChamber(IdentifierHash id, MDTChamber* &chamber) const;
  //Control for which histograms to add
  bool m_do_mdtchamberstatphislice;
  bool m_do_mdtChamberHits;
  bool m_do_mdttdccut_sector;
  //  bool m_do_mdttdc;
  //  bool m_do_mdttdccut_ML1;
  //  bool m_do_mdttdccut_ML2;
  //  bool m_do_mdtadc_onSegm_ML1;
  //  bool m_do_mdtadc_onSegm_ML2;
  //  bool m_do_mdtadc;
  //  bool m_do_mdttdcadc;
  //  bool m_do_mdtlayer;
  //  bool m_do_mdttube;
  //  bool m_do_mdtmezz;
  //  bool m_do_mdt_effEntries;
  //  bool m_do_mdt_effCounts;
  //  bool m_do_mdt_effPerTube;
  //  bool m_do_mdt_DRvsDT;
  //  bool m_do_mdt_DRvsDRerr;
  //  bool m_do_mdt_DRvsSegD;
  //

  // NEW
  float  m_nb_hits;        //minimum number of hits in segment
  float  m_chi2_cut;       //track chi2 cut;
  float  m_HighOccThreshold; //minimum number of hits to consider an event a possible noise burst
  bool   m_BMGpresent;
  int    m_BMGid;
  std::map<Identifier, std::vector<Identifier> > m_DeadChannels;

};
    
#endif

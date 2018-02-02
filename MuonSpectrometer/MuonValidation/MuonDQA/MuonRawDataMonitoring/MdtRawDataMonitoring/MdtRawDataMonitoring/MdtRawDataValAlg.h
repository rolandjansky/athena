/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtRawDataMonitoring
// Author:   N. Benekos(Illinois) - G. Dedes(MPI) - Orin Harris (University of Washington) 
// Sept. 2007
//
// DESCRIPTION:
// Subject: MDT-->Offline Muon Data Quality
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef MdtRawDataValAlg_H
#define MdtRawDataValAlg_H

//Core Include
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h" 

//Helper Includes
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"
#include "MdtRawDataMonitoring/MDTMonGroupStruct.h"
#include "MdtRawDataMonitoring/MDTNoisyTubes.h"
#include "MdtRawDataMonitoring/MDTChamber.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "TrkSegment/SegmentCollection.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//standard library includes
#include <fstream> 
#include <cstdlib>
#include <iostream>

class Identifier;
class IdentifierHash;
class MdtIdHelper;
class MuonDQAHistList;

namespace Muon {
  class MdtPrepData;
  class MuonIdHelperTool;
}

//stl includes
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

//root includes
class TH1;
class TH2;
class TString;
class TH1F_LW;
class TH2F_LW;
class TColor;

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

template <class ConcreteAlgorithm> class AlgFactory; 
class MdtRawDataValAlg: public ManagedMonitorToolBase {
 public:
  
  MdtRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent ); 
 
  virtual ~MdtRawDataValAlg();
  StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent(/* bool newLumiBlock, bool newRun */);
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
     
 private: 

  std::string m_title;

  MDTMonGroupStruct* m_mg;
  MDTNoisyTubes* m_masked_tubes;
  ActiveStoreSvc* m_activeStore;

  const MdtIdHelper* m_mdtIdHelper;  
  ToolHandle<Muon::MuonIdHelperTool>  m_idHelper;
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
  const MuonGM::MuonDetectorManager*  p_MuonDetectorManager ; //!< Pointer On MuonDetectorManager

  virtual StatusCode  bookMDTHistograms( MDTChamber* chamber, Identifier digcoll_id );//book chamber by chamber histos
  virtual StatusCode  fillMDTHistograms( const Muon::MdtPrepData* );//fill chamber by chamber histos
  virtual StatusCode  bookMDTSummaryHistograms( bool newLumiBlock, bool newRun);//Those over barrel/encap layer etc.
  //virtual StatusCode  fillMDTSummaryHistograms( const Muon::MdtPrepData*, bool &isNoiseBurstCandidate);
  virtual StatusCode  fillMDTSummaryHistograms( const Muon::MdtPrepData*, std::set<std::string>, bool &isNoiseBurstCandidate);
  virtual StatusCode  bookMDTOverviewHistograms( bool newLumiBlock, bool newRun);
  virtual StatusCode  fillMDTOverviewHistograms(const Muon::MdtPrepData*, bool &isNoiseBurstCandidate);
  StatusCode handleEvent_effCalc(const Trk::SegmentCollection* segms);//, const Muon::MdtPrepDataContainer* mdt_container );

  //MDTRawDataUtils_cxx
  bool AinB( int A, std::vector<int> & B );
  virtual StatusCode  binMdtGlobal( TH2* &, char ecap );
  virtual StatusCode  binMdtRegional( TH2* &, std::string &xAxis);
  virtual StatusCode  binMdtGlobal_byLayer( TH2*, TH2*, TH2*);
  virtual StatusCode binMdtOccVsLB(TH2* &h, int region, int layer);
  virtual StatusCode binMdtOccVsLB_Crate(TH2* &h, int region, int crate);
  void TubeID_to_ID_L_ML(int & tubeID, const std::string & hardware_name, int & tube, int & layer, int & ML, int max);
  void ChamberTubeNumberCorrection(int & tubeNum, const std::string & hardware_name, int tubePos, int numLayers);
  void CorrectTubeMax(const std::string & hardware_name, int & numTubes);
  void CorrectLayerMax(const std::string & hardware_name, int & numLayers);
  virtual StatusCode  bookMDTHisto_overview( TH1*&, TString, TString, TString, int, float, float, MonGroup&);
  virtual StatusCode  bookMDTHisto_chambers( TH1F_LW*&, TString, TString, TString, int, float, float, MonGroup&);
  virtual StatusCode  bookMDTHisto_overview_2D( TH2*& h, TString, TString, TString, int, float, float, int, float, float, MonGroup&);
  virtual StatusCode  bookMDTHisto_chambers_2D( TH2F_LW*& h, TString, TString, TString, int, float, float, int, float, float, MonGroup&);
  virtual StatusCode  bookMDTHisto_OccVsLB( TH2*& h, TString, TString, TString, int, float, float, int, float, float, MonGroup&);
  virtual StatusCode  fillMDTMaskedTubes(IdentifierHash, const std::string &, TH1F_LW*& h);
  void putBox(TH2* h, float x1, float y1, float x2, float y2);
  void putLine(TH2* h, float x1, float y1, float x2, float y2, Color_t c=kBlack);
  int get_bin_for_LB_hist(int region, int layer, int phi, int eta, bool isBIM);
  int get_bin_for_LB_crate_hist(int region, int layer, int phi, int eta, std::string chamber);
  // private function to initialize the selection of a certain region
  void mdtchamberId();    
  //private function to find mdt mezz cards
  int mezzmdt(Identifier);
  int GetTubeMax(const Identifier & digcoll_id, const std::string & hardware_name);
  StatusCode StoreTriggerType();
  void StoreTriggerType(int type);
  int GetTriggerType() { return m_trigtype; }
  bool HasTrigBARREL() { return m_trig_BARREL; }
  bool HasTrigENDCAP() { return m_trig_ENDCAP; }
  bool isATLASReady() { return m_atlas_ready; }
  void setIsATLASReady();
  StatusCode fillLumiBlock();
  StatusCode GetTimingInfo();
  StatusCode GetEventNum();
  void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

  ToolHandleArray<IDQFilterTool> m_DQFilterTools;
  bool m_atlas_ready;
  int m_lumiblock;
  int m_eventNum; 
  int m_firstEvent;
  uint32_t m_time;
  uint32_t m_firstTime;
  int m_numberOfEvents;
  //int m_time;
  //


  const Trk::SegmentCollection* m_segms;
  std::string m_segm_type;
  /*   StatusCode analyseSegments_effCalc(); */

  std::string returnString(int i){
    std::stringstream ss;
    std::string s;
    ss << i;
    ss >> s;
    return s;
  }

  int returnInt(std::string s){
    std::stringstream ss(s);
    int n;
    ss >> n;
    return n;
  }

  std::vector<Identifier> m_chambersId;
  std::vector<IdentifierHash> m_chambersIdHash;
  std::set<std::string> m_hardware_name_list;
  std::set<int> m_hashId_list;
  std::map<std::string,float> m_hitsperchamber_map;

  std::map<std::string,float> m_tubesperchamber_map;

  bool m_doMdtESD ; 

  //bool m_booked;
  // to book or not bookMDTTDCplots -->   /Chambers/tmp/ directory
  bool m_doChamberHists;
  bool m_isOnline;
  bool m_maskNoisyTubes;
  bool m_chamber_2D;  //Set this to true/false in the Job Options in order to see/not see chamber by chamber 2d adc vs tdc plots.  

  std::string m_chamberName;
  std::string m_StationSize;
  std::string m_key_mdt;
  std::string m_key_rpc;
  int m_StationEta;
  int m_StationPhi;

  int m_trigtype;
  bool m_trig_BARREL;
  bool m_trig_ENDCAP;
  //Define configurable adccut and TGC/RPC keys
  float m_ADCCut;
  float m_ADCCut_Bkgrd;
//  float m_TDCCut_Bkgrd;
  float m_curTime;
//  int m_TGCKey;
//  int m_RPCKey;

  
  //nobody looks at these histograms --> remove for now, to improve memory consumption
  //TH1* mdtevents_RPCtrig; // Total number of MDT digits RPCtrig
  //TH1* mdtevents_TGCtrig; // Total number of MDT digits TGCtrig
  //TH1* mdthitsvseventnum;
  //TH1* mdthitsvseventnumcut;
  //TH1* overalltdccut_RPCtrig; // all chambers tdc superimposed with adc cut
  //TH1* overalltdccut_TGCtrig; // all chambers tdc superimposed with adc cut
    
  //From Old BS
  TH2* m_overalltdcadcLumi; // all chambers tdc vs adc superimposed
  TH2* m_overalltdcadcPRLumi[4]; // all chambers tdc vs adc superimposed
  //  TH1* overalltdc; // all chambers tdc superimposed
  TH1* m_overalltdccutLumi; // all chambers tdc superimposed with adc cut
  TH1* m_overalltdccut_segm_Lumi; // all chambers tdc superimposed with adc cut
  TH1* m_overalladc_segm_Lumi; // all chambers adc on segm
  TH1* m_overalladc_Lumi; // all chambers adc
  TH1* m_overalltdccut_segm_PR_Lumi[4]; // all chambers tdc superimposed with adc cut per region
  TH1* m_overalltdccutPRLumi[4]; // all chambers tdc superimposed with adc cut per region
  TH1* m_overalladc_segm_PR_Lumi[4]; // all chambers adc superimposed per region
  TH1* m_overalladcPRLumi[4]; // all chambers adc superimposed per region
  TH1* m_overalladccutPRLumi[4]; // all chambers adc superimposed per region with adc noise cut
  TH1* m_overalltdccutPRLumi_RPCtrig[4]; // all chambers tdc superimposed with adc cut per region
  TH1* m_overalltdccutPRLumi_TGCtrig[4]; // all chambers tdc superimposed with adc cut per region
  
  
  TH2* m_overalltdcadcHighOcc; // all chambers tdc vs adc superimposed, events with > m_HighOccThreshold hits
  TH1* m_overalltdcHighOcc; // all chambers tdc superimposed, events with > m_HighOccThreshold hits
  TH1* m_overalltdcHighOcc_ADCCut; // all chambers tdc (with ADC>80) superimposed, events with > m_HighOccThreshold hits
  TH1* m_overalladc_HighOcc; // all chambers adc superimposed, events with > m_HighOccThreshold hits
  TH2* m_overalltdcadcPR_HighOcc[4]; // all chambers tdc vs adc superimposed
  TH1* m_overalltdcPR_HighOcc[4]; // all chambers tdc superimposed per region
  TH1* m_overalltdcPR_HighOcc_ADCCut[4]; // all chambers tdc superimposed with adc cut per region
  TH1* m_overalladcPR_HighOcc[4]; // all chambers tdc superimposed with adc cut per region

  TH2* m_overall_mdt_DRvsDT;
  //  TH2* overall_mdt_DRvsDRerr;
  TH2* m_overall_mdt_DRvsSegD;
  TH2* m_overallPR_mdt_DRvsDT[4];
  //  TH2* overallPR_mdt_DRvsDRerr[4];
  TH2* m_overallPR_mdt_DRvsSegD[4];
  TH2* m_MdtNHitsvsRpcNHits;  
  

  TH1* m_mdteventscutLumi; // Total number of MDT digits with a cut on ADC
  TH1* m_mdteventscutLumi_big; // Total number of MDT digits with a cut on ADC (for high mult. evt)
  TH1* m_mdteventsLumi; // Total number of MDT digits without a cut on ADC
  TH1* m_mdteventsLumi_big; // Total number of MDT digits without a cut on ADC (for high mult. evt)

  TH1* m_mdtglobalhitstime;
  

  TH1* m_nummdtchamberswithhits; // Number of MDT chambers with hits
  TH1* m_nummdtchamberswithhits_ADCCut; // Number of MDT chambers with hits
  TH1* m_nummdtchamberswithHighOcc; // Number of MDT chambers with > 1% occupancy

  TH1* m_mdtchamberstat;
  TH1* m_mdtchamberstatphislice[16];
  TH1* m_mdtChamberHits[4][4][16];
  TH2* m_mdtxydet[3];
  TH2* m_mdtrzdet[3]; 
  TH2* m_mdthitspermultilayerLumi[4][4];
  TH2* m_mdteffpermultilayer[4][4];
  TH2* m_mdthitsperchamber_InnerMiddleOuterLumi[2];
  TH2* m_mdthitsperchamber_InnerMiddleOuter_HighOcc[2];
  TH2* m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[2];
  TH2* m_mdteffperchamber_InnerMiddleOuter[4];
  TH2* m_mdthitsperML_byLayer[3];//These are alternative Global hit coverage plots
  TH2* m_mdtoccvslb[4][3];
  TH2* m_mdtoccvslb_by_crate[4][4];
  TH2* m_mdtoccvslb_ontrack_by_crate[4][4];

  /////End from old BS

  ///////////For t0 calculations//////////
  TH1* m_mdttdccut_sector[4][4][16]; ////  [endcap/barrel A/C][layer][sector]
  //These can be left to the post-processing
/*   TH1* mdttdccut_t0[4]; //Just four plot */
/*   TH1* mdttdccut_tmax[4]; //Just four plot */
/*   TH1* mdttdccut_tdrift[4]; //Just four plot */

  //Chamber by Chamber Plots
  //  std::map< IdentifierHash, MDTChamber* >* m_hist_map;
  std::vector< MDTChamber* >* m_hist_hash_list;
  void clear_hist_map(bool reallocate=true);
  
  std::string getChamberName(const Muon::MdtPrepData*);
  std::string getChamberName(Identifier);
  //  std::string getChamberName(IdentifierHash);
  StatusCode getChamber(IdentifierHash id, MDTChamber* &chamber);
 
  //Control for which histograms to add
  bool m_do_mdtchamberstatphislice;
  bool m_do_mdtChamberHits;
  bool m_do_mdttdccut_sector;
  bool m_do_mdttdc;
  bool m_do_mdttdccut_ML1;
  bool m_do_mdttdccut_ML2;
  bool m_do_mdtadc_onSegm_ML1;
  bool m_do_mdtadc_onSegm_ML2;
  bool m_do_mdttdccut_RPCtrig_ML1;
  bool m_do_mdttdccut_TGCtrig_ML1;
  bool m_do_mdttdccut_RPCtrig_ML2;
  bool m_do_mdttdccut_TGCtrig_ML2;
  bool m_do_mdtadc;
  bool m_do_mdttdcadc;
  bool m_do_mdtmultil;
  bool m_do_mdtlayer;
  bool m_do_mdttube;
  bool m_do_mdttube_bkgrd;
  bool m_do_mdttube_fornoise;
  bool m_do_mdttube_masked;
  bool m_do_mdtmezz;
  bool m_do_mdt_effEntries;
  bool m_do_mdt_effCounts;
  bool m_do_mdt_effPerTube;
  bool m_do_mdt_DRvsDT;
  bool m_do_mdt_DRvsDRerr;
  bool m_do_mdt_DRvsSegD;
  bool m_do_mdttubenoise; 
  bool m_do_mdttdctube; 
  //

  // NEW
  float  m_nb_hits;        //minimum number of hits in segment
  float  m_road_width;     //road width for pattern recognition
  float  m_chi2_cut;       //track chi2 cut;
  float  m_HighOccThreshold; //minimum number of hits to consider an event a possible noise burst
  bool   m_BMGpresent;
  int    m_BMGid;
  std::map<Identifier, std::vector<Identifier> > m_DeadChannels;

};
    
#endif

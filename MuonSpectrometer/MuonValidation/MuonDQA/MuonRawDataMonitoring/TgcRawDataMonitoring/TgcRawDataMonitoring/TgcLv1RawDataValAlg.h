/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcLv1RawDataValAlg
// Authors:  A.Ishikawa(Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TgcLv1RawDataValAlg_H
#define TgcLv1RawDataValAlg_H

#include <sstream>
#include <string.h>
#include <vector>
#include <map>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/DataHandle.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataCollection.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"

class TFile;
class TgcIdHelper;
template <class ConcreteAlgorithm> class AlgFactory;
/////////////////////////////////////////////////////////////////////////////

class TgcLv1RawDataValAlg: public ManagedMonitorToolBase {

  public:

    TgcLv1RawDataValAlg ( const std::string & type, const std::string & name, const IInterface* parent );
    virtual ~TgcLv1RawDataValAlg();
    StatusCode initialize(); 
    // StatusCode finalize();   

    //virtual StatusCode bookHistograms();
    virtual StatusCode bookHistogramsRecurrent();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:
    // Stores
    //StoreGateSvc*   m_eventStore;
    StoreGateSvc*   m_detStore;
    ActiveStoreSvc* m_activeStore;

    // Muon Detector Manager
    const MuonGM::MuonDetectorManager* m_muonMgr;
    // TGC Id Helper
    const TgcIdHelper* m_tgcIdHelper;

    // Event Properties
    int m_event;
    int m_lumiblock;
    int m_BCID;

    // Express Stream flags and chain pointer
    bool m_found_express_stream;
    bool m_found_nonmuon_express_chain;
    std::pair<std::vector<std::string>, std::vector<float> > m_expressChains;

    // Keys and Locations for retrieving collections
    std::string m_hitContainerLocation;
    std::string m_hitContainerLocationPrevious;
    std::string m_hitContainerLocationNext;
    std::string m_coinCollectionLocation;
    std::string m_coinCollectionLocationPrevious;
    std::string m_coinCollectionLocationNext;
    std::string m_L1muonRoIName;
    std::string m_L1emtauRoIName;
    std::string m_L1jetRoIName;

    // Number of Muon Algorithms to use
    int m_nMuonAlgorithms;

    // Enum style indexes
    static const int PREV=0, CURR=1, NEXT=2, TOTA=3; //PCNT index

    ///////////////////////////////////////////////////////////////////////////
    // ReadContainer
    ///////////////////////////////////////////////////////////////////////////
    // Manages the vectors which store data read from Coin and Muon Containers
    // These vectors are used to fill histograms by the other sections
    // They are filled by the readXXX functions

    // Trigger RoI etas and phis
    std::vector<float> m_L1TGCetas,  m_L1TGCphis;
    std::vector<float> m_L1RPCetas,  m_L1RPCphis;
    std::vector<float> m_L1Caloetas, m_L1Calophis;

    // Vectors to read data into from collections
    // Offline Reconstructed Muon: muid
    std::vector<float> m_muid_pt;
    std::vector<float> m_muid_eta;
    std::vector<float> m_muid_phi;
    std::vector<float> m_muid_q;

    // Muon Trigger: Low pT
    std::vector<int>   m_lpt_delta[2][3];    //[ws][pcn]
    std::vector<int>   m_lpt_tid[2][3];      //[ws][pcn]trackletID
    std::vector<int>   m_lpt_sub[2][3];      //[ws][pcn]
    std::vector<float> m_lpt_eta[2][3];      //[ws][pcn]
    std::vector<float> m_lpt_phi[2][3];      //[ws][pcn]
    std::vector<int>   m_lpt_etain[2][3];    //[ws][pcn]
    std::vector<int>   m_lpt_etaout[2][3];   //[ws][pcn]
    std::vector<int>   m_lpt_phi48[2][3];    //[ws][pcn]
    std::vector<bool>  m_lpt_isAside[2][3];  //[ws][pcn]
    std::vector<bool>  m_lpt_isForward[2][3];//[ws][pcn]
    int                m_nLptWire[4];        //[pcnt]
    int                m_nLptStrip[4];       //[pcnt]
    // Muon Trigger: High pT
    std::vector<int>   m_hpt_delta[2][3];    //[ws][pcn]
    std::vector<int>   m_hpt_tid[2][3];      //[ws][pcn]trackletID
    std::vector<int>   m_hpt_sub[2][3];      //[ws][pcn]
    std::vector<float> m_hpt_eta[2][3];      //[ws][pcn]
    std::vector<float> m_hpt_phi[2][3];      //[ws][pcn]
    std::vector<int>   m_hpt_etain[2][3];    //[ws][pcn]
    std::vector<int>   m_hpt_etaout[2][3];   //[ws][pcn]
    std::vector<int>   m_hpt_phi48[2][3];    //[ws][pcn]
    std::vector<bool>  m_hpt_isAside[2][3];  //[ws][pcn]
    std::vector<bool>  m_hpt_isForward[2][3];//[ws][pcn]
    int                m_nHptWire[4];        //[pcnt]
    int                m_nHptStrip[4];       //[pcnt]
    std::vector<bool>  m_hpt_isInner[2][3];//[ws][pcn] flag for inner coin
    std::vector<int>   m_hpt_inner[2][3];//[ws][pcn]
    // Muon Trigger: SL
    std::vector<int>   m_sl_pt[3];           //[pcn]
    std::vector<float> m_sl_eta[3];          //[pcn]
    std::vector<float> m_sl_phi[3];          //[pcn]
    std::vector<int>   m_sl_tidw[3];         //[pcn]trackletID for wire
    std::vector<int>   m_sl_tids[3];         //[pcn]trackletID for strip
    std::vector<int>   m_sl_roi[3];          //[pcn]
    std::vector<int>   m_sl_etaout[3];       //[pcn]
    std::vector<int>   m_sl_phi48[3];        //[pcn]
    std::vector<bool>  m_sl_isAside[3];      //[pcn]
    std::vector<bool>  m_sl_isForward[3];    //[pcn]
    std::vector<int>   m_sl_etaroi[3];       //[pcn]
    std::vector<int>   m_sl_phiroi[3];       //[pcn]
    int                m_nSL[4];             //[pcnt]

    void       clearVectorsArrays();
    void       readTgcCoinDataContainer(const Muon::TgcCoinDataContainer* tgc_coin_container, int pcn);
    StatusCode readOfflineMuonContainer(std::string key,
	std::vector<float>* mu_pt, std::vector<float>* mu_eta,
	std::vector<float>* mu_phi,std::vector<float>* mu_q);
    StatusCode readL1TriggerType();
    StatusCode readEventInfo();


    ///////////////////////////////////////////////////////////////////////////
    // NumberOfTrigger
    ///////////////////////////////////////////////////////////////////////////
    // Manages trigger rates, event rates etc. histograms


    // EIFI Coin
    TH2* m_tgclv1hptstripinlbvssecteifilumi[2]; //[AC]	
    TH2* m_tgclv1hptstripinlbvssecteifisect[2]; //[AC]	
    // Trigger Rate histograms
    TH2* m_tgclv1lptwireinlbvssect[2];  //[AC]
    TH2* m_tgclv1lptstripinlbvssect[2]; //[AC]
    TH2* m_tgclv1hptwireinlbvssect[2];  //[AC]
    TH2* m_tgclv1hptstripinlbvssect[2]; //[AC]
    TH2* m_tgclv1slinlbvssect[2];       //[AC]
    // Number of Trigger histograms, not produced in GM
    TH1* m_tgclv1numberoflptwire[4];    //[PCNT]
    TH1* m_tgclv1numberoflptstrip[4];   //[PCNT]
    TH1* m_tgclv1numberofhptwire[4];    //[PCNT]
    TH1* m_tgclv1numberofhptstrip[4];   //[PCNT]
    TH1* m_tgclv1numberofsl[4];         //[PCNT]
    // pT Threshold profile
    TH1* m_tgclv1pt[2];                 //[AC]
    // RoI eta vs phi maps
    TH2* m_tgclv1roietavsphi[2];        //[AC]
    TH2* m_tgclv1roietavsphipt[2][6];   //[AC][pT]
    TH2* m_tgclv1roietavsphiAll;        
    TH2* m_tgclv1roietavsphiptAll[6];   //[pT]
    // nTrigger Phi Profile histograms
    TH1* m_tgclv1lptsectwire[2];    //[AC]
    TH1* m_tgclv1lptsectstrip[2];   //[AC]
    TH1* m_tgclv1hptsectwire[2];    //[AC]
    TH1* m_tgclv1hptsectstrip[2];   //[AC]
    TH1* m_tgclv1slsect[2];         //[AC]
    // inEta&outEta vs Phi maps
    TH2* m_tgclv1lptwirechamber[2]; //[AC]
    TH2* m_tgclv1lptstripchamber[2];//[AC]
    TH2* m_tgclv1hptwirechamber[2]; //[AC]
    TH2* m_tgclv1hptstripchamber[2];//[AC]
    TH2* m_tgclv1slchamber[2];      //[AC]
    TH2* m_tgclv1slchamberAll; 

    // Express Stream
    TH2* m_tgclv1_SL_vs_muonchain_ES;

    // book histograms
    StatusCode bookHistogramsNumberOfTriggersAndProfile();

    // event profiles per LB
    TH1* m_tgclv1eventsinlb;
    TH1* m_tgclv1eventsinbcid;
    // SL profiles per LB
    TH1* m_tgclv1slinlb[2];             //[AC]
    TH1* m_tgclv1slinbcid[2];           //[AC]
    TH1* m_tgclv1slpereventlb[2];       //[AC]
    TH1* m_tgclv1slpereventbcid[2];     //[AC]
    TH1* m_tgclv1slinlbpt[2][6];        //[AC][pt]
    TH1* m_tgclv1slinbcidpt[2][6];      //[AC][pt]
    TH1* m_tgclv1slpereventlbpt[2][6];  //[AC][pt]
    TH1* m_tgclv1slpereventbcidpt[2][6];//[AC][pt]

    // book histograms
    StatusCode bookHistogramsTriggerRate();

    // fill histograms
    void fillNumberOfTrigger();


    ///////////////////////////////////////////////////////////////////////////
    // TriggerTiming
    ///////////////////////////////////////////////////////////////////////////
    // Manages Trigger Timing and "Current Fraction" histograms

    // [trig] = TGC/RPChi/RPClo/L1Calo
    // flag for maximum trigger in each chamber at TGC3 for each Trigger
    bool m_hasLpTtrigger[2][2][6][48][3];//[ws][eta][phi48][PCN] //flag only
    //bool m_hasHpTtrigger[2][2][6][48][3];//[ws][eta][phi48][PCN] //flag only
    int  m_maxSLtrigger[2][6][48][3];    //[AC][eta][phi48][PCN] //highest trigger found, none=0
    // Trigger timing flag and type counters
    int m_earliestTrigger;
    int m_L1TriggerType[4];//0:TGC, 1:RPClo, 2:RPChi, 3:L1Calo

    // SL Timing
    // profiles per side
    TH1* m_tgclv1sltiming[2];                      //[AC]
    TH1* m_tgclv1sltimingptcut[2];                 //[AC] pT>1
    TH1* m_tgclv1sltimingpt[2][6];                 //[AC][pT]
    // maps
    TH2* m_tgclv1sltimingmap[2];                   //[AC]
    TH2* m_tgclv1slcurrentfractionmap[2];          //[AC]
    TH2* m_tgclv1sltimingmapptcut[2];              //[AC] pT>1
    TH2* m_tgclv1slcurrentfractionmapptcut[2];     //[AC] pT>1
    // profile per sector
    TH1* m_tgclv1sltimingsector[2][12];            //[AC][sect]
    TH1* m_tgclv1sltimingptcutsector[2][12];       //[AC][sect] pT>1
    // profile per trigger type
    TH1* m_tgclv1sltimingtrg[2][4];                //[AC][trig]
    TH1* m_tgclv1sltimingptcuttrg[2][4];           //[AC][trig] pT>1
    TH1* m_tgclv1sltimingpttrg[2][6][4];           //[AC][pT][trig]
    // profile per sector-trigger
    TH1* m_tgclv1sltimingsectortrg[2][12][4];      //[AC][sect][trig]
    TH1* m_tgclv1sltimingptcutsectortrg[2][12][4]; //[AC][sect][trig] pT>1
    // associated offline tracks
    TH1* m_tgclv1sltimingtrack[2][2];              //[AC][muid]
    TH1* m_tgclv1sltimingptcuttrack[2][2];         //[AC][muid]

    // LpT Timing
    // profiles
    TH1* m_tgclv1lpttiming[2];                      //[AC]
    TH1* m_tgclv1lpttimingptcut[2];                 //[AC] pT>1
    TH1* m_tgclv1lpttimingpt[2][6];                 //[AC][pT]
    // maps
    TH2* m_tgclv1lpttimingmap[2];                   //[AC]
    TH2* m_tgclv1lptcurrentfractionmap[2];          //[AC]
    TH2* m_tgclv1lpttimingmapptcut[2];              //[AC] pT>1
    TH2* m_tgclv1lptcurrentfractionmapptcut[2];     //[AC] pT>1
    // profile per sector
    TH1* m_tgclv1lpttimingsector[2][12];            //[AC][sect]
    TH1* m_tgclv1lpttimingptcutsector[2][12];       //[AC][sect] pT>1
    // profile per trigger type
    TH1* m_tgclv1lpttimingtrg[2][4];                //[AC][trig]
    TH1* m_tgclv1lpttimingptcuttrg[2][4];           //[AC][trig] pT>1
    TH1* m_tgclv1lpttimingpttrg[2][6][4];           //[AC][pt][trig]
    // profile per sector-trigger
    TH1* m_tgclv1lpttimingsectortrg[2][12][4];      //[AC][sect][trig]
    TH1* m_tgclv1lpttimingptcutsectortrg[2][12][4]; //[AC][sect][trig] pT>1
    // associated offline tracks
    TH1* m_tgclv1lpttimingtrack[2][2];              //[AC][muid]
    TH1* m_tgclv1lpttimingptcuttrack[2][2];         //[AC][muid]

    // SL vs LpT Timing
    TH2* m_tgclv1slvslpttiming[2];                      //[AC]
    TH2* m_tgclv1slvslpttimingptcut[2];                 //[AC] pT>1
    TH2* m_tgclv1slvslpttimingsector[2][12];            //[AC][sect]
    TH2* m_tgclv1slvslpttimingptcutsector[2][12];       //[AC][sect] pT>1
    TH2* m_tgclv1slvslpttimingsectortrg[2][12][4];      //[AC][sect][trig]
    TH2* m_tgclv1slvslpttimingptcutsectortrg[2][12][4]; //[AC][sect][trig] pT>1

    // Express Stream
    // Sector profile of Current fraction histograms
    TH1* m_tgclv1_SL_trigger_timing_ES[2];       //[AC]
    TH1* m_tgclv1_SL_trigger_timing_num_ES[2];   //[AC]
    TH1* m_tgclv1_SL_trigger_timing_denom_ES[2]; //[AC]
    TH1* m_tgclv1_LPT_trigger_timing_ES[2];      //[AC]
    TH1* m_tgclv1_LPT_trigger_timing_num_ES[2];  //[AC]
    TH1* m_tgclv1_LPT_trigger_timing_denom_ES[2];//[AC]
    // Express Stream General Monitoring, (same histograms as above)
    TH1* m_tgclv1_SL_trigger_timing_ES_GM[2]; //[AC]
    TH1* m_tgclv1_LPT_trigger_timing_ES_GM[2];//[AC]

    // book histograms
    StatusCode bookHistogramsTiming();

    // fill histograms
    void fillTriggerTiming(int pt);
    void fillTriggerTiming0(int pt);
    void fillTriggerTiming1(int pt);
    void fillTriggerTimingAssociatedWithTrack( int ms,
	std::vector<float>* mu_pt, std::vector<float>* mu_eta,
	std::vector<float>* mu_phi,std::vector<float>* mu_q);


    ///////////////////////////////////////////////////////////////////////////
    // Efficiency
    ///////////////////////////////////////////////////////////////////////////
    // Manages Efficiency maps and profile histograms, also trigger turn-on curves

    // deltar between tracks in event
    TH1* m_tgclv1deltar[2][6][2];                   //[AC][pT][charge][muid]
    // Turn-on Curves
    TH1* m_tgclv1turnon[2][6][3][2];                //[AC][pT][charge][muid]
    TGraphAsymmErrors* m_tgclv1turnontg[2][6][3][2];//[AC][pT][charge][muid]
    TH1* m_tgclv1turnonnum[2][6][3][2];             //[AC][pT][charge][muid]
    TH1* m_tgclv1turnondenom[2][6][3][2];           //[AC][pT][charge][muid]
    // LV1 trigger efficiency maps (wrt offline muons)
    TH2* m_tgclv1effetavsphi[2][6][3][2][3];        //[AC][pT][charge][muid][pcn]
    TH2* m_tgclv1effetavsphinum[2][6][3][2][3];     //[AC][pT][charge][muid][pcn]
    TH2* m_tgclv1effetavsphidenom[2][6][3][2];      //[AC][pT][charge][muid]

    // Express Stream
    // Turn-on curves
    TH1* m_tgclv1turnon_ES[2][6];                //[AC][pT] muid
    TGraphAsymmErrors* m_tgclv1turnontg_ES[2][6];//[AC][pT] muid
    TH1* m_tgclv1turnonnum_ES[2][6];             //[AC][pT] muid
    TH1* m_tgclv1turnondenom_ES[2][6];           //[AC][pT] muid
    // 6PT thresholds as x axis
    TH1* m_tgclv1_plateau_eff_counting_ES[2];      //[AC] muid
    TH1* m_tgclv1_plateau_eff_counting_num_ES[2];  //[AC] muid
    TH1* m_tgclv1_plateau_eff_counting_denom_ES[2];//[AC] muid
    TH1* m_tgclv1_plateau_eff_counting_ES_GM[2];   //[AC] muid

    // book histograms
    StatusCode bookHistogramsEfficiency();

    // fill histograms
    void fillEfficiency(int ms,
	std::vector<float>* mu_pt, std::vector<float>* mu_eta,
	std::vector<float>* mu_phi,std::vector<float>* mu_q);


    ///////////////////////////////////////////////////////////////////////////
    // CoincidenceWindow
    ///////////////////////////////////////////////////////////////////////////
    // Manages Coincidence Window histograms (is not currently used)

    // Coincidence windows
    TH2* m_tgclv1cw[2][9][6];                  //[AC][cwmod][pT]
    TH2* m_tgclv1cwoffline[2][9][6][2];        //[AC][cwmod][pT][muid]
    TH2* m_tgclv1cwrejectedoffline[2][9][6][2];//[AC][cwmod][pT][muid]

    // book histograms
    StatusCode bookHistogramsCoincidenceWindow();

    // fill histograms
    void fillCoincidenceWindow(int ms,
	std::vector<float>* mu_pt, std::vector<float>* mu_eta,
	std::vector<float>* mu_phi,std::vector<float>* mu_q);


    ///////////////////////////////////////////////////////////////////////////
    // Summary
    ///////////////////////////////////////////////////////////////////////////
    // Manages summary histograms, these are all filled in the process histograms section
    // (And more importantly in the PostProcessor)

    //summary of SL and LowPT timing
    TH1* m_tgclv1summaryofsltiming[2]; //[AC]
    TH1* m_tgclv1summaryoflpttiming[2];//[AC]

    TH1* m_tgclv1summaryofsltimingperchambertype[6]; //[chambertype]
    TH1* m_tgclv1summaryoflpttimingperchambertype[6];//[chambertype]

    // book histograms
    StatusCode bookHistogramsSummary();


    ///////////////////////////////////////////////////////////////////////////
    // LowStat
    ///////////////////////////////////////////////////////////////////////////
    // Manages Low Statistics (per lumiblock) histograms

    //===
    TH2* m_tgclv1slchamberlowstat[2];    //[AC]
    TH1* m_tgclv1sltimingptcutlowstat[2];//[AC]

    // book histograms
    StatusCode bookHistogramsLowStat();


    ///////////////////////////////////////////////////////////////////////////
    // Functions
    ///////////////////////////////////////////////////////////////////////////
    // Contains functions to convert various detector numberings, set histogram labels
    // and other miscellaneous functions

    // reads etaroi and phi roi from TgcCoinData
    void roi2etaphi(const Muon::TgcCoinData& cd, //input
	int& etaroi, int& phiroi);//output

    // calculates sector number from phi index and forward/endcap index
    int phi2sector(int phi48,int fe);

    // calculates layer index from station index and gasgap index
    int stationGasGap2layer(int station, int gasgap);

    // sets axis and bin labels for 1D/2D BCID (timing) histograms
    void setTH1TitleLabelBCID(TH1* h);
    void setTH2TitleLabelBCID(TH2* h);

    // calculates deltaR between two eta-phi points
    float deltaR(float eta1, float phi1, float eta2, float phi2);

    // calculates RoI index from wire&strip ids and submatrix(?)
    int trackletIdSub2RoI(int tidw, int subw, int tids, int subs, bool isForward, int phi);

    // functions to blank out non-existent chambers
    void putBox(TH2* h2, float x1, float y1, float x2, float y2);
    void BlankPhi24(TH2 *h2, int binx);
    void BlankStationMap(TH2 *h2);
};

#endif


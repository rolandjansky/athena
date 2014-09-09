/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *  @file HLTIDtrkMonTool.cxx
 *
 *  Contact:
 *  @author Erkcan.Ozcan (AT cern DOT ch) 
 *  @author Stefan.Ask (AT cern DOT ch) 
 *  
 *  Comment: see .h file
 *
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigSteeringEvent/HLTResult.h"   
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TMath.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

#include "TrigIDtrkMonitoring/HLTIDtrkMonTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"

// #include "TrigInDetAnalysisUtils/TIDA_newtracking.h"


//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

using namespace std;
using namespace CLHEP;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

HLTIDtrkMonTool::HLTIDtrkMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  :IHLTMonTool(type, name, parent),
   m_doMonCommon(true),
   m_DeltaR(0.05),
   m_PtMin(1000.),
   m_EtaMax(2.5),
   m_Z0Max(2000.),
   m_D0Max(1000.),
   m_SiHitMin(3)
{

  //+++ EF slices/configurations histograms
  m_EFSliceName.assign(13,"");
  declareProperty("doMonCommon",      m_doMonCommon);
  //+++ InsideOut
  declareProperty("EFCollectionType", m_EFType = "EFID");
  declareProperty("BjetCollName",     m_EFSliceName[0]   = "NOTPRODUCED");  // n_Bjet     or NOTPRODUCED
  declareProperty("BphysicsCollName", m_EFSliceName[1]   = "NOTPRODUCED");  // n_Bphysics or NOTPRODUCED
  declareProperty("ElectronCollName", m_EFSliceName[2]   = "NOTPRODUCED");  // n_Electron or NOTPRODUCED
  declareProperty("FullScanCollName", m_EFSliceName[3]   = "NOTPRODUCED");  // n_FullScan or NOTPRODUCED
  declareProperty("MuonCollName",     m_EFSliceName[4]   = "NOTPRODUCED");  // n_Muon     or NOTPRODUCED
  declareProperty("TauCollName",      m_EFSliceName[5]   = "NOTPRODUCED");  // n_Tau      or NOTPRODUCED
  declareProperty("MinBiasCollName",  m_EFSliceName[12]  = "NOTPRODUCED");  // n_minBias  or NOTPRODUCED
  //+++ OutsideIn
  declareProperty("TRTOnlyElectronCollName",  m_EFSliceName[6]   = "NOTPRODUCED");  // TRTOnly_Electron  or NOTPRODUCED
  declareProperty("TRTOnlyTauCollName",       m_EFSliceName[7]   = "NOTPRODUCED");  // TRTOnly_Tau       or NOTPRODUCED
  declareProperty("TRTOnlyMuonCollName",      m_EFSliceName[8]   = "NOTPRODUCED");  // TRTOnly_Muon      or NOTPRODUCED
  declareProperty("TRTOnlyMinBiasCollName",   m_EFSliceName[9]   = "NOTPRODUCED");  // TRTOnly_MinBias   or NOTPRODUCED
  declareProperty("CombinedElectronCollName", m_EFSliceName[10]  = "NOTPRODUCED"); // Combined_Electron or NOTPRODUCED
  //+++ Cosmics (InsideOut)
  declareProperty("CosmicsCollName",  m_EFSliceName[11]  = "NOTPRODUCED");  // CosmicsN or NOTPRODUCED

  //+++ L2/EF/Offline Collections to be compared
  m_MatchPairs.push_back("EFOff");
  m_MatchPairs.push_back("IDSOff");
  m_MatchPairs.push_back("SiTOff");
  m_MatchPairs.push_back("TRTOff");
  m_MatchPairs.push_back("L2Off");  
  declareProperty("IDSCANCollectionType",  m_IDSCANType  = "IDSCAN");
  declareProperty("SiTrackCollectionType", m_SiTrackType = "SiTrack");
  declareProperty("TRTCollectionType",     m_TRTType     = "TRT");
  declareProperty("OfflineCollection",     m_OfflineCollection = "TrackParticleCandidate");
  declareProperty("CombEFCollection",      m_CombEFSlice      = "IOTRT_CosmicsN");
  declareProperty("CombIDSCANCollection",  m_CombIDSCANSlice  = "Cosmics");
  declareProperty("CombSiTrackCollection", m_CombSiTrackSlice = "Cosmics");
  declareProperty("CombTRTCollection",     m_CombTRTSlice     = "SegmentFinder");
  declareProperty("DeltaRMatch", m_DeltaR);
  declareProperty("PtMin",       m_PtMin);
  declareProperty("EtaMax",      m_EtaMax);
  declareProperty("Z0Max",       m_Z0Max);
  declareProperty("D0Max",       m_D0Max);
  declareProperty("SiHitMin",    m_SiHitMin);

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

HLTIDtrkMonTool::~HLTIDtrkMonTool() {
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode HLTIDtrkMonTool::init() {

  //+++ Total number of matched tracks per combination
  totalMatchedTracks.clear();
  for (unsigned int i=0;i<m_MatchPairs.size();i++) totalMatchedTracks.push_back(0);

  //+++ EF Histograms
  m_TrigEFTrack_pt = "TrigEFTrack_pt_";
  m_TrigEFTrack_eta = "TrigEFTrack_eta_";
  m_TrigEFTrack_phi = "TrigEFTrack_phi_";
  m_TrigEFTrack_z0 = "TrigEFTrack_z0_";
  m_TrigEFTrack_d0 = "TrigEFTrack_d0_";
  m_TrigEFTrack_qoverp = "TrigEFTrack_qoverp_";
  m_TrigEFTrack_NPixelHits = "TrigEFTrack_NPixelHits_";
  m_TrigEFTrack_NSctHits = "TrigEFTrack_NSctHits_";
  m_TrigEFTrack_NTRTHits = "TrigEFTrack_NTRTHits_";
  m_TrigEFTrack_ChiNdof = "TrigEFTrack_ChiNdof_";
  m_TrigEFROI_NTracks = "TrigEFROI_NTracks_";
  m_TrigEFROI_NVertices = "TrigEFROI_NVertices_";
  if (m_doMonCommon) {
    m_TrigEffEta = "TrigEffEta_";
    m_TrigEffPhi = "TrigEffPhi_";
    m_TrigEffZ0 = "TrigEffZ0_";
    m_TrigEffD0 = "TrigEffD0_";

    m_TrigOfflHoles = "TrigOfflHoles_";
    m_TrigTrigHoles = "TrigTrigHoles_";
    m_TrigNOfflHoles = "TrigNOfflHoles_";
    m_TrigNTrigHoles = "TrigNTrigHoles_";
  }

  return StatusCode::SUCCESS;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTIDtrkMonTool::book() { 
#else
StatusCode HLTIDtrkMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) {
#endif

  ATH_MSG_DEBUG(" ====== Begin book() ====== ");
  //+++ Standard MonGroup
  //MonGroup * monGroup    = new MonGroup(this, "HLT/IDTrkMon", shift, run);
  MonGroup * monGroup    = new MonGroup(this, "HLT/IDTrkMon", run);
  addMonGroup(monGroup);
  //+++ Track holes MonGroup
  //MonGroup * holeMonGroup = new MonGroup(this, "HLT/IDTrkHole", shift, run);
  MonGroup * holeMonGroup = new MonGroup(this, "HLT/IDTrkHole", run);
  addMonGroup(holeMonGroup);
  //+++ Efficiency MonGroup, in order to merge them properly
  //MonGroup * effMonGroup = new MonGroup(this, "HLT/IDTrkEff", shift, run);
  MonGroup * effMonGroup = new MonGroup(this, "HLT/IDTrkEff", run);
  addMonGroup(effMonGroup);
  //+++ TEST: LowStat MonGroup
  //MonGroup * lowMonGroup = new MonGroup(this, "HLT/IDTrkMonLS", ManagedMonitorToolBase::shift, ManagedMonitorToolBase::lowStat);
  MonGroup * lowMonGroup = new MonGroup(this, "HLT/IDTrkMonLS", ManagedMonitorToolBase::lowStat);
  addMonGroup(lowMonGroup);
  //MonGroup * lowHoleMonGroup = new MonGroup(this, "HLT/IDTrkHoleLS", ManagedMonitorToolBase::shift, ManagedMonitorToolBase::lowStat);
  MonGroup * lowHoleMonGroup = new MonGroup(this, "HLT/IDTrkHoleLS", ManagedMonitorToolBase::lowStat);
  addMonGroup(lowHoleMonGroup);
  //MonGroup * lowEffMonGroup = new MonGroup(this, "HLT/IDTrkEffLS", ManagedMonitorToolBase::shift, ManagedMonitorToolBase::lowStat);
  MonGroup * lowEffMonGroup = new MonGroup(this, "HLT/IDTrkEffLS", ManagedMonitorToolBase::lowStat);
  addMonGroup(lowEffMonGroup);
  //MonGroup * JayTest = new MonGroup(this, "HLT/JayTest", ManagedMonitorToolBase::shift, ManagedMonitorToolBase::lowStat);
  //addMonGroup(JayTest);

  if (newRun || newLowStatInterval) {
    for (unsigned int i=0; i<m_EFSliceName.size(); i++){

      double tmpNPixHits = 10.;
      if (m_EFSliceName[i] == "IOTRT_CosmicsN") tmpNPixHits = 15.;
      double tmpNSctHits = 15.;
      if (m_EFSliceName[i] == "IOTRT_CosmicsN") tmpNSctHits = 35.;
      double tmpNTRTHits = 50.;
      if (m_EFSliceName[i] == "IOTRT_CosmicsN") tmpNTRTHits = 75.;

      double tmpZ0 = 400.;
      if (m_EFSliceName[i] == "IOTRT_CosmicsN") tmpZ0 = 1400.;
      double tmpD0 = 6.;
      if (m_EFSliceName[i] == "IOTRT_CosmicsN") tmpD0 = 1000.;

      if (m_EFSliceName[i] == "" || m_EFSliceName[i] == "NOTPRODUCED") continue;
      //+++ EF histrograms
      std::string tmpVariable1 = m_TrigEFTrack_pt + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,0.,40000.),"HLT/IDTrkMon");
      // addHistogram(new TH1F("JayTest123", "JayTest123",    100,0.,40000.),"HLT/JayTest"); // JWH
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,0.,40000.),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_eta + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-5.,5.),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-5.,5.),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_phi + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-3.2,3.2),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-3.2,3.2),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_z0  + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-tmpZ0,tmpZ0),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-tmpZ0,tmpZ0),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_d0 + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-tmpD0,tmpD0),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-tmpD0,tmpD0),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_qoverp + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-0.01,0.01),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-0.01,0.01),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_NPixelHits + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    int(tmpNPixHits),0.,tmpNPixHits),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    int(tmpNPixHits),0.,tmpNPixHits),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_NSctHits + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    int(tmpNSctHits),0.,tmpNSctHits),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    int(tmpNSctHits),0.,tmpNSctHits),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_NTRTHits + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    int(tmpNTRTHits),0.,tmpNTRTHits),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    int(tmpNTRTHits),0.,tmpNTRTHits),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFTrack_ChiNdof + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,0.,10.),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,0.,10.),"HLT/IDTrkMonLS");
      tmpVariable1 = m_TrigEFROI_NTracks + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkMon");
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkMonLS");
      tmpVariable1 =  m_TrigEFROI_NVertices + m_EFSliceName[i];
      if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    10,0.,10.),"HLT/IDTrkMon"); 
      addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    10,0.,10.),"HLT/IDTrkMonLS"); 
    }

    double tmpZ0 = 400.;
    double tmpD0 = 6.;

    //+++ Common histograms
    if (m_doMonCommon) {
      for (unsigned int i=0; i<m_MatchPairs.size(); i++){
	std::string tmpVariable1 =  m_TrigOfflHoles + m_MatchPairs[i];
	if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHole");
	addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHoleLS"); 
	tmpVariable1 =  m_TrigTrigHoles + m_MatchPairs[i];
	if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHole");
	addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHoleLS"); 
	tmpVariable1 =  m_TrigNOfflHoles + m_MatchPairs[i];
	if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHole");
	addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHoleLS"); 
	tmpVariable1 =  m_TrigNTrigHoles + m_MatchPairs[i];
	if (newRun) addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHole");
	addHistogram(new TH1F(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,0.,20.),"HLT/IDTrkHoleLS"); 

	tmpVariable1 =  m_TrigEffEta + m_MatchPairs[i];
	if (newRun) addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-5.,5.,0.,110.),"HLT/IDTrkEff"); 
	addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,-5.,5.,0.,110.),"HLT/IDTrkEffLS"); 
	tmpVariable1 =  m_TrigEffPhi + m_MatchPairs[i];
	if (newRun) addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-3.2,3.2,0.,110.),"HLT/IDTrkEff"); 
	addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,-3.2,3.2,0.,110.),"HLT/IDTrkEffLS"); 
	tmpVariable1 =  m_TrigEffZ0 + m_MatchPairs[i];
	if (newRun) addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-tmpZ0,tmpZ0,0.,110.),"HLT/IDTrkEff"); 
	addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,-tmpZ0,tmpZ0,0.,110.),"HLT/IDTrkEffLS"); 
	tmpVariable1 =  m_TrigEffD0 + m_MatchPairs[i];
	if (newRun) addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    100,-tmpD0,tmpD0,0.,110.),"HLT/IDTrkEff"); 
	addHistogram(new TProfile(tmpVariable1.c_str(),    tmpVariable1.c_str(),    20,-tmpD0,tmpD0,0.,110.),"HLT/IDTrkEffLS"); 
      }
    }

    // Tracking Parameters TrigIDScan
    if (newRun) {
      this->setCurrentMonGroup("HLT/IDTrkMon");
      addHistogram(new TH1F("TrigIDScan_z0",    "IDScan z0",    100,-tmpZ0,tmpZ0));
      addHistogram(new TH1F("TrigIDScan_pt",    "IDScan pt",    100, 0.,40000.));
      addHistogram(new TH1F("TrigIDScan_a0",    "IDScan a0",    100,-tmpD0,tmpD0)); // bug
      addHistogram(new TH1F("TrigIDScan_phi0",  "IDScan phi0",  100,-3.2, 3.2));
      addHistogram(new TH1F("TrigIDScan_TRTtrk","IDScan TRTtrk",100, 0.,75.));
      addHistogram(new TH1F("TrigIDScan_SiHit", "IDScan SiHit",  12,-0.5,11.5));
      addHistogram(new TH1F("TrigIDScan_eta",   "IDScan eta",   100,-5.,  5.));
    }
    this->setCurrentMonGroup("HLT/IDTrkMonLS");
    addHistogram(new TH1F("TrigIDScan_z0",    "IDScan z0",    100,-tmpZ0,tmpZ0));
    addHistogram(new TH1F("TrigIDScan_pt",    "IDScan pt",    100, 0.,40000.));
    addHistogram(new TH1F("TrigIDScan_a0",    "IDScan a0",    100,-tmpD0,tmpD0)); // bug
    addHistogram(new TH1F("TrigIDScan_phi0",  "IDScan phi0",  100,-3.2, 3.2));
    addHistogram(new TH1F("TrigIDScan_TRTtrk","IDScan TRTtrk",100, 0.,75.));
    addHistogram(new TH1F("TrigIDScan_SiHit", "IDScan SiHit",  12,-0.5,11.5));
    addHistogram(new TH1F("TrigIDScan_eta",   "IDScan eta",   100,-5.,  5.));

    // Tracking Parameters SiTrack
    if (newRun) {
      this->setCurrentMonGroup("HLT/IDTrkMon");
      addHistogram(new TH1F("TrigSiTrack_z0",    "SiTrack z0",    100,-tmpZ0,tmpZ0));
      addHistogram(new TH1F("TrigSiTrack_pt",    "SiTrack pt",    100, 0.,40000.));
      addHistogram(new TH1F("TrigSiTrack_a0",    "SiTrack a0",    100,-tmpD0,tmpD0)); // bug
      addHistogram(new TH1F("TrigSiTrack_phi0",  "SiTrack phi0",  100,-3.2, 3.2));
      addHistogram(new TH1F("TrigSiTrack_eta",   "SiTrack eta",   100,-5.,  5.));
      addHistogram(new TH1F("TrigSiTrack_TRTtrk","SiTrack TRTtrk",100, 0.,75.));
      addHistogram(new TH1F("TrigSiTrack_SiHit", "SiTrack SiHit",  12,-0.5,11.5));
    }
    this->setCurrentMonGroup("HLT/IDTrkMonLS");
    addHistogram(new TH1F("TrigSiTrack_z0",    "SiTrack z0",    100,-tmpZ0,tmpZ0));
    addHistogram(new TH1F("TrigSiTrack_pt",    "SiTrack pt",    100, 0.,40000.));
    addHistogram(new TH1F("TrigSiTrack_a0",    "SiTrack a0",    100,-tmpD0,tmpD0)); // bug
    addHistogram(new TH1F("TrigSiTrack_phi0",  "SiTrack phi0",  100,-3.2, 3.2));
    addHistogram(new TH1F("TrigSiTrack_eta",   "SiTrack eta",   100,-5.,  5.));
    addHistogram(new TH1F("TrigSiTrack_TRTtrk","SiTrack TRTtrk",100, 0.,75.));
    addHistogram(new TH1F("TrigSiTrack_SiHit", "SiTrack SiHit",  12,-0.5,11.5));

    // Tracking Parameters TRTSegmentFinder
    if (newRun) {
      this->setCurrentMonGroup("HLT/IDTrkMon");
      addHistogram(new TH1F("TrigTRTxk_z0",    "TRTxk z0",    100,-tmpZ0,tmpZ0));
      addHistogram(new TH1F("TrigTRTxk_pt",    "TRTxk pt",    100, 0.,40000.));
      addHistogram(new TH1F("TrigTRTxk_a0",    "TRTxk a0",    100,-tmpD0,tmpD0));
      addHistogram(new TH1F("TrigTRTxk_phi0",  "TRTxk phi0",  100,-3.2, 3.2));
      addHistogram(new TH1F("TrigTRTxk_eta",   "TRTxk eta",   100,-5.,  5.));
      addHistogram(new TH1F("TrigTRTxk_TRTtrk","TRTxk TRTtrk",100, 0.,75.));
    }
    this->setCurrentMonGroup("HLT/IDTrkMonLS");
    addHistogram(new TH1F("TrigTRTxk_z0",    "TRTxk z0",    100,-tmpZ0,tmpZ0));
    addHistogram(new TH1F("TrigTRTxk_pt",    "TRTxk pt",    100, 0.,40000.));
    addHistogram(new TH1F("TrigTRTxk_a0",    "TRTxk a0",    100,-tmpD0,tmpD0));
    addHistogram(new TH1F("TrigTRTxk_phi0",  "TRTxk phi0",  100,-3.2, 3.2));
    addHistogram(new TH1F("TrigTRTxk_eta",   "TRTxk eta",   100,-5.,  5.));
    addHistogram(new TH1F("TrigTRTxk_TRTtrk","TRTxk TRTtrk",100, 0.,75.));

    if (newRun) {
      TH1F *hFakeContrib = new TH1F("FakeContribution", "FakeContribution", 7, 0.5, 7.5);
      hFakeContrib->GetXaxis()->SetBinLabel(1,"IDS");
      hFakeContrib->GetXaxis()->SetBinLabel(2,"SiT");
      hFakeContrib->GetXaxis()->SetBinLabel(3,"IDS or SiT");
      hFakeContrib->GetXaxis()->SetBinLabel(4,"TRTxK");
      hFakeContrib->GetXaxis()->SetBinLabel(5,"Any L2");
      hFakeContrib->GetXaxis()->SetBinLabel(6,"EF");
      hFakeContrib->GetXaxis()->SetBinLabel(7,"Any HLT");
      addHistogram(hFakeContrib,"HLT/IDTrkMon");
    }
    TH1F *hFakeContrib2 = new TH1F("FakeContribution", "FakeContribution", 7, 0.5, 7.5);
    hFakeContrib2->GetXaxis()->SetBinLabel(1,"IDS");
    hFakeContrib2->GetXaxis()->SetBinLabel(2,"SiT");
    hFakeContrib2->GetXaxis()->SetBinLabel(3,"IDS or SiT");
    hFakeContrib2->GetXaxis()->SetBinLabel(4,"TRTxK");
    hFakeContrib2->GetXaxis()->SetBinLabel(5,"Any L2");
    hFakeContrib2->GetXaxis()->SetBinLabel(6,"EF");
    hFakeContrib2->GetXaxis()->SetBinLabel(7,"Any HLT");
    addHistogram(hFakeContrib2,"HLT/IDTrkMonLS");

  } else if ( newEventsBlock || newLumiBlock ) {
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG(" ====== End book() ====== ");
  
  return StatusCode::SUCCESS;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode HLTIDtrkMonTool::fill() {

  ATH_MSG_DEBUG(" ====== Begin fillHists() ====== ");
  

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Start of Level 2 Code
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  int m_maxTracks=200;

  const DataHandle<TrigInDetTrackCollection> trackCollection;
  const DataHandle<TrigInDetTrackCollection> lastTrackCollection;
  StatusCode sc_idtrk = m_storeGate->retrieve(trackCollection,lastTrackCollection);
  
  if (sc_idtrk.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve HLT IDtrk");
  }
  else { // Number of Hits
    ATH_MSG_DEBUG(" ====== START HLT IDtrk MonTool ====== ");
    
    int nTrtHits = 0;
    int nSiHits = 0;
    
    for (int iTrackColl=0; trackCollection != lastTrackCollection;++trackCollection, ++iTrackColl) {
      ATH_MSG_DEBUG(" found  "<< iTrackColl << ". Track Collection with RoI ID "
                              << trackCollection->RoI_ID());

      //loop over tracks
      TrigInDetTrackCollection::const_iterator track= trackCollection->begin();
      TrigInDetTrackCollection::const_iterator lastTrack= trackCollection->end();
      int nTracks=0;
      for (; track != lastTrack; track++) {
        if(nTracks >= static_cast<int>(m_maxTracks)) continue;
        nTracks++;
	// Track Parameters (if present)
	if ((*track)->param()) {
	  nTrtHits=0;
	  nSiHits=0;
	  if ((*track)->algorithmId() ==  TrigInDetTrack::IDSCANID || 
	      (*track)->algorithmId() ==  TrigInDetTrack::SITRACKID) {
	    nTrtHits=(*track)->NStrawHits();
	    nSiHits=(*track)->NPixelSpacePoints()+(*track)->NSCT_SpacePoints();
	  } else if ((*track)->algorithmId() ==  TrigInDetTrack::TRTXKID) {
	    // TRTxK uses a trick for KINE
	    //m_kineRef->push_back((*track)->NStrawTime()/10000);
	    //m_nKineHit->push_back(( (*track)->NStrawTime()%10000 )/100);
	    //m_nTime->push_back( (*track)->NStrawTime()%100);
	    //Andrei
	  } else if ((*track)->algorithmId() ==  TrigInDetTrack::TRTLUTID) {
	    nTrtHits = (*track)->NTRHits();
	    //std::vector<const InDet::TRT_DriftCircle*>* dcData = (*track)->trtDriftCircles();
	    //if (dcData)
	    //  nTrtHits=dcData->size();
	    //else
	    //  ATH_MSG_DEBUG(" No DriftCircle info for track " << nTracks);
	  }
	  ATH_MSG_DEBUG(" found " << nTracks << ". track  algorithmId = "
                            << (*track)->algorithmId() << " with pt = "
                            << (*track)->param()->pT() << "  phi0 = "
                            << (*track)->param()->phi0() << " z0 = "
                            << (*track)->param()->z0() << "  a0 = "
                            << (*track)->param()->a0() << "  eta = "
                            << (*track)->param()->eta());
	  
	  float pt  = (*track)->param()->pT();
	  float eta = (*track)->param()->eta();
	  float phi0= (*track)->param()->phi0();
	  float z0  = (*track)->param()->z0();
	  float a0  = (*track)->param()->a0();

	  
	  //possible algoIDs  enum AlgoId{NULLID=0, SITRACKID=1, IDSCANID=2, TRTLUTID=3, TRTXKID=4};
	  if ((*track)->algorithmId() == TrigInDetTrack::IDSCANID) {
	    //IDScan 
	    this->setCurrentMonGroup("HLT/IDTrkMon");
	    hist("TrigIDScan_z0")->Fill(z0);
	    hist("TrigIDScan_pt")->Fill(pt);
	    hist("TrigIDScan_a0")->Fill(a0);
	    hist("TrigIDScan_phi0")->Fill(phi0);
	    hist("TrigIDScan_eta")->Fill(eta); 
	    hist("TrigIDScan_TRTtrk")->Fill(nTrtHits);
	    hist("TrigIDScan_SiHit")->Fill(nSiHits);
	    this->setCurrentMonGroup("HLT/IDTrkMonLS");
	    hist("TrigIDScan_z0")->Fill(z0);
	    hist("TrigIDScan_pt")->Fill(pt);
	    hist("TrigIDScan_a0")->Fill(a0);
	    hist("TrigIDScan_phi0")->Fill(phi0);
	    hist("TrigIDScan_eta")->Fill(eta); 
	    hist("TrigIDScan_TRTtrk")->Fill(nTrtHits);
	    hist("TrigIDScan_SiHit")->Fill(nSiHits);
	    
	  } else if ((*track)->algorithmId() == TrigInDetTrack::SITRACKID) {
	    //SiTrack
	    this->setCurrentMonGroup("HLT/IDTrkMon");
	    hist("TrigSiTrack_z0")->Fill(z0);
	    hist("TrigSiTrack_pt")->Fill(pt);
	    hist("TrigSiTrack_a0")->Fill(a0);
	    hist("TrigSiTrack_phi0")->Fill(phi0);
	    hist("TrigSiTrack_eta")->Fill(eta);
	    hist("TrigSiTrack_TRTtrk")->Fill(nTrtHits);
	    hist("TrigSiTrack_SiHit")->Fill(nSiHits);
	    this->setCurrentMonGroup("HLT/IDTrkMonLS");
	    hist("TrigSiTrack_z0")->Fill(z0);
	    hist("TrigSiTrack_pt")->Fill(pt);
	    hist("TrigSiTrack_a0")->Fill(a0);
	    hist("TrigSiTrack_phi0")->Fill(phi0);
	    hist("TrigSiTrack_eta")->Fill(eta);
	    hist("TrigSiTrack_TRTtrk")->Fill(nTrtHits);
	    hist("TrigSiTrack_SiHit")->Fill(nSiHits);
	    
	  } else if ((*track)->algorithmId() == TrigInDetTrack::TRTXKID ||
		     (*track)->algorithmId() == TrigInDetTrack::TRTLUTID ) {
	    // TRTxK and TRTSegmentFinder (which uses the TRTLUTID)
	    // We still call the histograms TRTxK since TRTSegmentFinder is in the TRTxK package
	    this->setCurrentMonGroup("HLT/IDTrkMon");
	    hist("TrigTRTxk_z0")->Fill(z0);
	    hist("TrigTRTxk_pt")->Fill(pt);
	    hist("TrigTRTxk_a0")->Fill(a0);
	    hist("TrigTRTxk_phi0")->Fill(phi0);
	    hist("TrigTRTxk_eta")->Fill(eta);
	    hist("TrigTRTxk_TRTtrk")->Fill(nTrtHits);
	    this->setCurrentMonGroup("HLT/IDTrkMonLS");
	    hist("TrigTRTxk_z0")->Fill(z0);
	    hist("TrigTRTxk_pt")->Fill(pt);
	    hist("TrigTRTxk_a0")->Fill(a0);
	    hist("TrigTRTxk_phi0")->Fill(phi0);
	    hist("TrigTRTxk_eta")->Fill(eta);
	    hist("TrigTRTxk_TRTtrk")->Fill(nTrtHits);

	  } //algorithmId
	}   //track->param exists 
      }     // track loop
    }  // track collection loop
  }


  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Start of Event Filter code
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  const DataHandle<Rec::TrackParticleContainer> trackCollection_EF;
  const DataHandle<Rec::TrackParticleContainer> lastTrackCollection_EF;
  StatusCode sc_ideftrk = m_storeGate->retrieve(trackCollection_EF,lastTrackCollection_EF);

  if(sc_ideftrk.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve HLT EF IDtrk");
  }  

  for(; trackCollection_EF != lastTrackCollection_EF;++trackCollection_EF) {

    Rec::TrackParticleContainer::const_iterator track_EF= trackCollection_EF->begin();
    Rec::TrackParticleContainer::const_iterator lastTrack_EF= trackCollection_EF->end();
    int nTracks_EF=0;
    int EFCollIdentifier = -1;
    int N_vertices = 0;
    std::vector<const Trk::VxCandidate*> DistinctVertices;

    std::string collectionKey = trackCollection_EF.key();

    if(collectionKey.find(m_EFType, 0) != std::string::npos ){ 

      for(unsigned int i=0; i<m_EFSliceName.size(); i++){
	if (m_EFSliceName[i] == "") continue;
	if( collectionKey.find(m_EFSliceName[i], 0) != std::string::npos ){
	  EFCollIdentifier = i;
	}
      }
    }

    if (EFCollIdentifier < 0) {
      ATH_MSG_DEBUG("Event without any EF tracks!");
      continue;
    }

    for(; track_EF != lastTrack_EF; track_EF++, nTracks_EF++) {

#ifdef TRKPARAMETERS_MEASUREDPERIGEE_H
      const Trk::MeasuredPerigee* measPer = (*track_EF)->measuredPerigee();
#else
      const Trk::Perigee* measPer = (*track_EF)->measuredPerigee();
#endif

      if (measPer==0) {
        ATH_MSG_WARNING("No measured perigee parameters assigned to the track");
      }
      else{
	float pt  = measPer->pT(); 
	float eta = measPer->eta();


#ifdef TRKPARAMETERS_MEASUREDPERIGEE_H
	HepVector perigeeParams = measPer->parameters();    
	float phi=  perigeeParams[Trk::phi0];
	float z0  = perigeeParams[Trk::z0];
	float d0  = perigeeParams[Trk::d0];
	float qoverp = perigeeParams[Trk::qOverP];
#else
	float phi=  measPer->parameters()[Trk::phi0];
	float z0  = measPer->parameters()[Trk::z0];
	float d0  = measPer->parameters()[Trk::d0];
	float qoverp = measPer->parameters()[Trk::qOverP];
#endif


	std::string tmpVarName = "";
	tmpVarName = m_TrigEFTrack_pt + m_EFSliceName[EFCollIdentifier];
	hist(tmpVarName,"HLT/IDTrkMon")->Fill(pt);
	hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(pt);
	tmpVarName = m_TrigEFTrack_eta + m_EFSliceName[EFCollIdentifier];
	hist(tmpVarName,"HLT/IDTrkMon")->Fill(eta);
	hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(eta);
	tmpVarName = m_TrigEFTrack_phi + m_EFSliceName[EFCollIdentifier];
	hist(tmpVarName,"HLT/IDTrkMon")->Fill(phi);
	hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(phi);
	tmpVarName = m_TrigEFTrack_z0 + m_EFSliceName[EFCollIdentifier];
	hist(tmpVarName,"HLT/IDTrkMon")->Fill(z0);
	hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(z0);
	tmpVarName = m_TrigEFTrack_d0 + m_EFSliceName[EFCollIdentifier];
	hist(tmpVarName,"HLT/IDTrkMon")->Fill(d0);
	hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(d0);
	tmpVarName = m_TrigEFTrack_qoverp + m_EFSliceName[EFCollIdentifier];
	hist(tmpVarName,"HLT/IDTrkMon")->Fill(qoverp);
	hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(qoverp);

	const Trk::TrackSummary* summary = (*track_EF)->trackSummary();
	if (summary!=0) {
	  tmpVarName = m_TrigEFTrack_NPixelHits + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(summary->get(Trk::numberOfPixelHits));
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(summary->get(Trk::numberOfPixelHits));
	  tmpVarName = m_TrigEFTrack_NSctHits + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(summary->get(Trk::numberOfSCTHits));
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(summary->get(Trk::numberOfSCTHits));
	  tmpVarName = m_TrigEFTrack_NTRTHits + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(summary->get(Trk::numberOfTRTHits));
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(summary->get(Trk::numberOfTRTHits));
	} else {
	  tmpVarName = m_TrigEFTrack_NPixelHits + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(-999);
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(-999);
	  tmpVarName = m_TrigEFTrack_NSctHits + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(-999);
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(-999);
	  tmpVarName = m_TrigEFTrack_NTRTHits + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(-999);
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(-999);
	}

	const Trk::VxCandidate* temp = (*track_EF)->reconstructedVertex();
	const Trk::VxCandidate* vertexcand = temp;

	if(vertexcand){
	  bool distinct = true;
	  if(N_vertices==0) {
	    DistinctVertices.push_back(vertexcand);
	    N_vertices++;
	  }
	  else{
	    for(unsigned int i=0;i<DistinctVertices.size();i++){
	      if(vertexcand == DistinctVertices[i]) distinct=false;
	    }	      
	  }
	  if(distinct==true) DistinctVertices.push_back(vertexcand);
	}

	const Trk::FitQuality* fitquality = (*track_EF)->fitQuality();
	if (fitquality!=0) {
	  double TrkChiSquared = fitquality->chiSquared();
	  double TrkNdof = fitquality->doubleNumberDoF();
	  double Chi_per_Ndof = -1.;
	  if (TrkNdof != 0) Chi_per_Ndof = TrkChiSquared/TrkNdof; // JWH
	  tmpVarName = m_TrigEFTrack_ChiNdof + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(Chi_per_Ndof);
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(Chi_per_Ndof);
	} else {
	  tmpVarName = m_TrigEFTrack_ChiNdof + m_EFSliceName[EFCollIdentifier];
	  hist(tmpVarName,"HLT/IDTrkMon")->Fill(-999);
	  hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(-999);
	}
      }
    }// end of loop over tracks

    std::string tmpVarName = "";
    tmpVarName = m_TrigEFROI_NVertices + m_EFSliceName[EFCollIdentifier];
    hist(tmpVarName,"HLT/IDTrkMon")->Fill(DistinctVertices.size());
    hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(DistinctVertices.size());
    tmpVarName = m_TrigEFROI_NTracks + m_EFSliceName[EFCollIdentifier];
    hist(tmpVarName,"HLT/IDTrkMon")->Fill(nTracks_EF);
    hist(tmpVarName,"HLT/IDTrkMonLS")->Fill(nTracks_EF);

  }// end of loop over track collections


  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Start of Combined L2/EF/Offline Comparison
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  if (!m_doMonCommon) return StatusCode::SUCCESS;
  ATH_MSG_VERBOSE("Starting combined L2/EF/Offline monitoring");

  StatusCode sc_comb;

  //+++ Get Offline and EF tracks
  const DataHandle<Rec::TrackParticleContainer> trackCollection_Comb_EF;
  const DataHandle<Rec::TrackParticleContainer> lastTrackCollection_Comb_EF;
  sc_comb = m_storeGate->retrieve(trackCollection_Comb_EF,lastTrackCollection_Comb_EF);

  if(sc_comb.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve EF Comb Tracks");
  }  

  m_EFTrackVectors.clear();
  m_OfflineTrackVectors.clear();
  m_EFRawTrackVectors.clear();
  m_OfflineRawTrackVectors.clear();

  for ( ; trackCollection_Comb_EF != lastTrackCollection_Comb_EF;++trackCollection_Comb_EF) {
      std::string collectionKey = trackCollection_Comb_EF.key();
      if ( (m_EFType != "") && (m_CombEFSlice != "") ) {
	if( collectionKey.find(m_EFType, 0) != std::string::npos ){ 
	  if( collectionKey.find(m_CombEFSlice, 0) != std::string::npos ){
	    m_EFTrackVectors = GetEFVectors(trackCollection_Comb_EF);
	    Rec::TrackParticleContainer::const_iterator track_EF= trackCollection_Comb_EF->begin();
	    Rec::TrackParticleContainer::const_iterator lastTrack_EF= trackCollection_Comb_EF->end();
	    for (; track_EF != lastTrack_EF; ++track_EF) {
	      if ( ((*track_EF)->trackSummary())== 0 || ((*track_EF)->measuredPerigee())==0 ) continue;
	      m_EFRawTrackVectors.push_back((*track_EF));
	    }
	  }
	}
      } 
      if ( m_OfflineCollection != "" ) {
	if ( collectionKey.find(m_OfflineCollection, 0) != std::string::npos ){
	  m_OfflineTrackVectors = GetEFVectors(trackCollection_Comb_EF);
	  Rec::TrackParticleContainer::const_iterator track_EF= trackCollection_Comb_EF->begin();
	  Rec::TrackParticleContainer::const_iterator lastTrack_EF= trackCollection_Comb_EF->end();
	  for (; track_EF != lastTrack_EF; ++track_EF) {
	    if ( ((*track_EF)->trackSummary())== 0 || ((*track_EF)->measuredPerigee())==0 ) continue;
	    m_OfflineRawTrackVectors.push_back((*track_EF));
	  }
	}
      }
  }


  //+++ Get IDSCAN and SiTracks tracks
  const DataHandle<TrigInDetTrackCollection> trackCollection_Comb_L2;
  const DataHandle<TrigInDetTrackCollection> lastTrackCollection_Comb_L2;
  sc_comb = m_storeGate->retrieve(trackCollection_Comb_L2,lastTrackCollection_Comb_L2);
  
  if(sc_comb.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve IDSCAN Comb Tracks");
  } 
  
  m_IDSCANTrackVectors.clear();
  m_SiTrackTrackVectors.clear();
  m_IDSCANRawTrackVectors.clear();
  m_SiTrackRawTrackVectors.clear();
  m_TRTTrackVectors.clear();

  for ( ; trackCollection_Comb_L2 != lastTrackCollection_Comb_L2; ++trackCollection_Comb_L2 ) {
    std::string collectionKey = trackCollection_Comb_L2.key();
    if ( (m_IDSCANType != "") && (m_CombIDSCANSlice != "") ) {
      if( collectionKey.find(m_IDSCANType, 0) != std::string::npos ){ 
	if( collectionKey.find(m_CombIDSCANSlice, 0) != std::string::npos ){
	  m_IDSCANTrackVectors = GetL2Vectors(trackCollection_Comb_L2);
	  TrigInDetTrackCollection::const_iterator track= trackCollection_Comb_L2->begin();
	  TrigInDetTrackCollection::const_iterator lastTrack= trackCollection_Comb_L2->end();
	  for (; track != lastTrack; ++track) m_IDSCANRawTrackVectors.push_back((*track));
	}
      }
    }
    if ( (m_SiTrackType != "") && (m_CombSiTrackSlice != "") ) {
      if ( collectionKey.find(m_SiTrackType, 0) != std::string::npos ) { 
	if( collectionKey.find(m_CombSiTrackSlice, 0) != std::string::npos ){
	  m_SiTrackTrackVectors  = GetL2Vectors(trackCollection_Comb_L2);
	  TrigInDetTrackCollection::const_iterator track= trackCollection_Comb_L2->begin();
	  TrigInDetTrackCollection::const_iterator lastTrack= trackCollection_Comb_L2->end();
	  for (; track != lastTrack; ++track) m_SiTrackRawTrackVectors.push_back((*track));
	}
      }
    }
    if ( (m_TRTType != "") && (m_CombTRTSlice != "") ) {
      if ( collectionKey.find(m_TRTType, 0) != std::string::npos ) { 
	if( collectionKey.find(m_CombTRTSlice, 0) != std::string::npos ){
	  m_TRTTrackVectors  = GetL2Vectors(trackCollection_Comb_L2);
	}
      }
    }
  }


  // SA: Ugly Temporary PariID solution, but OK for now.
  if(m_OfflineTrackVectors.size() > 0){

    //+++ Match tracks: EF / Offline 
    m_OfflineToEF.clear();
    if (m_EFTrackVectors.size() > 0) {
      m_OfflineToEF = MatchTracks(m_OfflineTrackVectors, m_EFTrackVectors, m_DeltaR);
    }
    EfficiencyResidual(0, m_OfflineTrackVectors, m_EFTrackVectors, m_OfflineToEF, "EF");
    if (!(m_OfflineToEF.empty())) TrackHoles(0, m_OfflineRawTrackVectors, m_EFRawTrackVectors, m_OfflineToEF, "EF");
   
    //+++ Match tracks: IDSCAN / Offline 
    m_OfflineToIDSCAN.clear();
    if (m_IDSCANTrackVectors.size() > 0) {
      m_OfflineToIDSCAN = MatchTracks(m_OfflineTrackVectors, m_IDSCANTrackVectors, m_DeltaR);
    }
    EfficiencyResidual(1, m_OfflineTrackVectors, m_IDSCANTrackVectors, m_OfflineToIDSCAN, "IDSCAN");
    if (!(m_OfflineToIDSCAN.empty())) TrackHoles(1, m_OfflineRawTrackVectors, m_IDSCANRawTrackVectors, m_OfflineToIDSCAN, "IDSCAN");
    
    //+++ Match tracks: SiTrack / Offline 
    m_OfflineToSiTrack.clear();
    if (m_SiTrackTrackVectors.size() > 0) {
      m_OfflineToSiTrack = MatchTracks(m_OfflineTrackVectors, m_SiTrackTrackVectors, m_DeltaR);
    }
    EfficiencyResidual(2, m_OfflineTrackVectors, m_SiTrackTrackVectors, m_OfflineToSiTrack, "SiTrack");
    if (!(m_OfflineToSiTrack.empty())) TrackHoles(2, m_OfflineRawTrackVectors, m_SiTrackRawTrackVectors, m_OfflineToSiTrack, "SiTrack");
    
    //+++ Match tracks: TRT / Offline 
    m_OfflineToTRT.clear();
    if (m_TRTTrackVectors.size() > 0) {
      m_OfflineToTRT = MatchTracks(m_OfflineTrackVectors, m_TRTTrackVectors, m_DeltaR);
    }
    EfficiencyResidual(3, m_OfflineTrackVectors, m_TRTTrackVectors, m_OfflineToTRT, "TRT");

    //+++ Efficiency from OR of Algs
    EfficiencyOR(4);
  }

  // Now fill a histogram to give an idea about the fake contributions.
  if ( m_OfflineTrackVectors.empty() ) {
    bool noIDStrack = m_IDSCANTrackVectors.empty();
    bool noSiTtrack = m_SiTrackTrackVectors.empty();
    bool noL2Strack = noIDStrack && noSiTtrack;
    bool noTRTtrack = m_TRTTrackVectors.empty();
    bool noL2track  = noL2Strack && noTRTtrack;
    bool noEFtrack  = m_EFTrackVectors.empty();
    bool noHLTtrack = noL2track && noEFtrack;

    this->setCurrentMonGroup("HLT/IDTrkMon");
    if ( !noIDStrack )  hist("FakeContribution")->Fill(1);
    if ( !noSiTtrack )  hist("FakeContribution")->Fill(2);
    if ( !noL2Strack )  hist("FakeContribution")->Fill(3);
    if ( !noTRTtrack )  hist("FakeContribution")->Fill(4);
    if ( !noL2track  )  hist("FakeContribution")->Fill(5);
    if ( !noEFtrack  )  hist("FakeContribution")->Fill(6);
    if ( !noHLTtrack )  hist("FakeContribution")->Fill(7);
    this->setCurrentMonGroup("HLT/IDTrkMonLS");
    if ( !noIDStrack )  hist("FakeContribution")->Fill(1);
    if ( !noSiTtrack )  hist("FakeContribution")->Fill(2);
    if ( !noL2Strack )  hist("FakeContribution")->Fill(3);
    if ( !noTRTtrack )  hist("FakeContribution")->Fill(4);
    if ( !noL2track  )  hist("FakeContribution")->Fill(5);
    if ( !noEFtrack  )  hist("FakeContribution")->Fill(6);
    if ( !noHLTtrack )  hist("FakeContribution")->Fill(7);
  } else {     
    hist("FakeContribution","HLT/IDTrkMon")->Fill(0);
    hist("FakeContribution","HLT/IDTrkMonLS")->Fill(0);
  }
  return StatusCode::SUCCESS;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode  HLTIDtrkMonTool::proc() { 
#else
StatusCode  HLTIDtrkMonTool::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun ) {
#endif

  StatusCode sc_idtrk = StatusCode::SUCCESS;

  if (endOfRun){

    //}
  
  //+++ Scale the hole histos by the total number of matched tracks (hole rate)
  // only EFOff, IDSOff and SiTOff for the moment
  for (unsigned int i=0;i<m_MatchPairs.size();i++) 
    {
      if (totalMatchedTracks[i]==0) continue;
      std::string tmpOfflHolesName = m_TrigOfflHoles + m_MatchPairs[i];
      std::string tmpTrigHolesName = m_TrigTrigHoles + m_MatchPairs[i];
      ATH_MSG_DEBUG("Scaling " << (float)(totalMatchedTracks[i])
                               << " trigger tracks of type " << i
                               << " by " <<  1./(float)(totalMatchedTracks[i]));
      ((TH1F *) hist(tmpOfflHolesName,"HLT/IDTrkHole"))->Scale(1./(float)(totalMatchedTracks[i]));
      ((TH1F *) hist(tmpOfflHolesName,"HLT/IDTrkHoleLS"))->Scale(1./(float)(totalMatchedTracks[i]));
      ((TH1F *) hist(tmpTrigHolesName,"HLT/IDTrkHole"))->Scale(1./(float)(totalMatchedTracks[i])); 
      ((TH1F *) hist(tmpTrigHolesName,"HLT/IDTrkHoleLS"))->Scale(1./(float)(totalMatchedTracks[i]));
    } 
  }
  //+++ Not removing the histograms is not the end of the world.
  if (sc_idtrk != StatusCode::SUCCESS) sc_idtrk = StatusCode::RECOVERABLE;
  return sc_idtrk;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

template <class track1, class track2>
std::map<int, vector<int> > HLTIDtrkMonTool::MatchTracks( const std::vector< track1 > & precisetracks, 
							  const std::vector< track2 > & coarsetracks, 
							  double deltaR ) {

  map< int, vector<int> > precise2coarse;

  // Loop over the precise tracks
  for (unsigned int ipt=0; ipt<precisetracks.size(); ++ipt) {

    TLorentzVector prTrk = precisetracks[ipt];

    vector<int> matchVector(0);

    for (unsigned int ict=0; ict<coarsetracks.size(); ++ict) {

      double delr = prTrk.DeltaR( coarsetracks[ict] );

      if ( delr < deltaR ) {

	bool inserted(false);

	// try to insert the coarse track in order of minimum deltaR
	for( vector<int>::iterator trkItr = matchVector.begin() ;
	     trkItr != matchVector.end(); ++trkItr )
	  if ( delr < prTrk.DeltaR( coarsetracks[*trkItr] ) ) {
	    matchVector.insert( trkItr, ict );
	    inserted = true;
	    break; }

	if ( !inserted )
	  matchVector.push_back(ict);

      }

    } // end of loop over coarse tracks

    //if no tracks are matched, move on to next precise track. No map entry for that precise track.
    ATH_MSG_DEBUG("Matched offline track " << ipt << " with "
                                           << matchVector.size() 
                                           << " trigger tracks");
    if( matchVector.empty() ) continue;

    precise2coarse.insert( std::pair<int, vector<int> >(ipt, matchVector) );
  }

  bool updated = true;
  while ( updated ) {

    updated = false;

    for(map<int, vector<int> >::iterator mapItr1 = precise2coarse.begin();
	mapItr1 != precise2coarse.end(); ++mapItr1) {

      // When no match entries are found in the map, remove them
      vector<int> matchVect1 = mapItr1->second;
      if ( matchVect1.empty() ) {
	updated = true;
	precise2coarse.erase( mapItr1 );
	break; }

      // Loop over the map to see if there is any other precise track
      //   that was matched to the same coarse track
      for(map<int, vector<int> >::iterator mapItr2 = precise2coarse.begin();
	  mapItr2 != precise2coarse.end(); ++mapItr2) {

	if ( mapItr1->first <= mapItr2->first ) {
	  continue;
	}

	vector<int> matchVect2 = mapItr2->second;

	// Fix SA 15/01/10 
	if ( matchVect2.empty() ) continue; 
	if ( matchVect1.front() != matchVect2.front() ) continue;

	double delr1 =
	  precisetracks[mapItr1->first].DeltaR(coarsetracks[matchVect1[0]]);
	double delr2 =
	  precisetracks[mapItr2->first].DeltaR(coarsetracks[matchVect2[0]]);

	if ( delr2 > delr1 ){
	  //Fix JL 9/10/08
	  if(! mapItr2->second.empty())  (mapItr2->second).erase((mapItr2->second).begin());
	  else  ATH_MSG_DEBUG("Warning, vector is empty");
	}
	else{
	  //Fix JL 9/10/08
	if(! mapItr1->second.empty())  (mapItr1->second).erase((mapItr1->second).begin());
	else  ATH_MSG_DEBUG("Warning, vector is empty");
	}

	updated = true;
	break;

      } // end of internal loop over the match map
    } // end of loop over the match map
  } // end of while loop

  return(precise2coarse);

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

vector< simpleTrack > HLTIDtrkMonTool::GetEFVectors( const DataHandle<Rec::TrackParticleContainer> trackCollection_EF) {

  vector< simpleTrack > EFVectors;

  Rec::TrackParticleContainer::const_iterator track_EF= trackCollection_EF->begin();
  Rec::TrackParticleContainer::const_iterator lastTrack_EF= trackCollection_EF->end();

  for(; track_EF != lastTrack_EF; ++track_EF) {

    const Trk::TrackSummary* summary = (*track_EF)->trackSummary();

#ifdef TRKPARAMETERS_MEASUREDPERIGEE_H
    const Trk::MeasuredPerigee* measPer = (*track_EF)->measuredPerigee();
#else
    const Trk::Perigee* measPer = (*track_EF)->measuredPerigee();
#endif
    if (summary == 0 || measPer==0) {
      ATH_MSG_DEBUG("No measured perigee pr trck summary parameters assigned to the track");
    }
    else {

      float pt  = measPer->pT(); 
      float eta = measPer->eta();
 
#ifdef TRKPARAMETERS_MEASUREDPERIGEE_H
      HepVector perigeeParams = measPer->parameters();    
      float phi=  perigeeParams[Trk::phi0];
      float z0  = perigeeParams[Trk::z0];
      float d0  = perigeeParams[Trk::d0];
      //      float qoverp = perigeeParams[Trk::qOverP];
#else
      float phi=  measPer->parameters()[Trk::phi0];
      float z0  = measPer->parameters()[Trk::z0];
      float d0  = measPer->parameters()[Trk::d0];
      //      float qoverp = measPer->parameters()[Trk::qOverP];
#endif

      float nPixHits = summary->get(Trk::numberOfPixelHits);
      float nSctHits = summary->get(Trk::numberOfSCTHits);
      float nTrtHits = summary->get(Trk::numberOfTRTHits);

      simpleTrack temp;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      EFVectors.push_back(temp);

    }
  }

  return(EFVectors);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

vector< simpleTrack > HLTIDtrkMonTool::GetL2Vectors(const DataHandle<TrigInDetTrackCollection> trackCollection_L2) {

  vector< simpleTrack > L2Vectors;

  TrigInDetTrackCollection::const_iterator track= trackCollection_L2->begin();
  TrigInDetTrackCollection::const_iterator lastTrack= trackCollection_L2->end();

  for (; track != lastTrack; track++) {

    float pt  = (*track)->param()->pT();
    float eta = (*track)->param()->eta();
    float phi = (*track)->param()->phi0();
    float z0  = (*track)->param()->z0();
    float d0  = (*track)->param()->a0();
    float nPixHits = 0; // Not used
    float nSctHits = 0; // Not used
    float nTrtHits = 0; // Not used


    simpleTrack temp;
    temp.SetPtEtaPhiM(pt,eta,phi,0.);
    temp.z0 = z0;
    temp.d0 = d0;
    temp.nPixHits = nPixHits;
    temp.nSctHits = nSctHits;
    temp.nTrtHits = nTrtHits;
    L2Vectors.push_back(temp);

  }

  return(L2Vectors);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

template <class tTrack>
void HLTIDtrkMonTool::TrackHoles(int PairID, 
				 //const std::vector< Rec::TrackParticle > & offline, 
				 //const std::vector< tTrack > & trigger, 
				 const std::vector< const Rec::TrackParticle* > offline, 
				 const std::vector< tTrack* > trigger, 
				 std::map<int, vector<int> > & offline2trigger,
				 std::string /*type*/ )
{
  //+++ Loop over offline tracks
  ATH_MSG_DEBUG("begin TrackHoles with " << offline.size() << " offline and "
                                         << trigger.size() << " trigger tracks");
  for(unsigned int i = 0; i < offline.size(); i++) {
    
    const Trk::TrackSummary* summary = offline[i]->trackSummary();

#ifdef TRKPARAMETERS_MEASUREDPERIGEE_H
    const Trk::MeasuredPerigee* measPer = offline[i]->measuredPerigee();
#else
    const Trk::Perigee* measPer = offline[i]->measuredPerigee();
#endif


    //+++ Offline selection
    //    ATH_MSG_DEBUG("track " << i << " : pT=" << measPer->pT() << " ,eta="
    //                       << measPer->eta() << " ,z0=" << perigeeParams[Trk::z0]
    //                       << " ,d0=" << perigeeParams[Trk::d0] << " ,SiHits="
    //                       << 2*(summary->get(Trk::numberOfPixelHits))+summary->get(Trk::numberOfSCTHits));
    if (fabs(measPer->pT()) < m_PtMin) continue;
    if (fabs(measPer->eta()) > m_EtaMax) continue;

#ifdef TRKPARAMETERS_MEASUREDPERIGEE_H
    HepVector perigeeParams = measPer->parameters();    
    if (fabs(perigeeParams[Trk::z0]) > m_Z0Max) continue;
    if (fabs(perigeeParams[Trk::d0]) > m_D0Max) continue;
#else
    if (fabs(measPer->parameters()[Trk::z0]) > m_Z0Max) continue;
    if (fabs(measPer->parameters()[Trk::d0]) > m_D0Max) continue;
#endif
    float nPixHits = summary->get(Trk::numberOfPixelHits);
    float nSctHits = summary->get(Trk::numberOfSCTHits);
    float tmp_SiHits = 2 * nPixHits + nSctHits;
    if (tmp_SiHits < m_SiHitMin && m_MatchPairs[PairID] != "TRTOff") continue;  // Exception for TRTSegFinder

    //+++ Check if matched
    bool tmpPassedAny = false;
    const std::map<int,std::vector<int> >::iterator iter = offline2trigger.find(i);
    if (iter!=offline2trigger.end()){
      if (iter->second.size() > 0) tmpPassedAny = true ;
    }

    if (!tmpPassedAny) continue;
    totalMatchedTracks[PairID]++;
    ATH_MSG_DEBUG("offline track " << i << " matched. Total matched tracks = "
                                   << totalMatchedTracks[PairID]);

    //+++ fish out hitPattern for offline track
    //    bool* oLayers = getPattern((*offline[i]));
    bool oLayers[19];
    getPattern((*offline[i]),oLayers);
    //for(int k=0; k<19; k++) ATH_MSG_DEBUG("offline layer " << k << " has pattern " << oLayers[k]);
    
    //+++ fish out hitPattern for closest matched trigger track
    int tTrackIndex = (offline2trigger.find(i))->second.front();
    //bool* tLayers = getPattern((*trigger[tTrackIndex]));
    bool tLayers[19];
    getPattern((*trigger[tTrackIndex]),tLayers);
    //for(int k=0; k<19; k++) ATH_MSG_DEBUG("trigger layer " << k << " has pattern " << tLayers[k]);
  
    //+++ Fill histos ---
    std::string tmpOfflHolesName = m_TrigOfflHoles + m_MatchPairs[PairID];
    std::string tmpTrigHolesName = m_TrigTrigHoles + m_MatchPairs[PairID];
    std::string tmpNOfflHolesName = m_TrigNOfflHoles + m_MatchPairs[PairID];
    std::string tmpNTrigHolesName = m_TrigNTrigHoles + m_MatchPairs[PairID];
    int nOfflHoles=0;
    int nTrigHoles=0;
    for (int k=0;k<19;k++)
      {
	if (tLayers[k] && !oLayers[k]) 
	  {
	    nOfflHoles++;
	    ((TH1F *) hist(tmpOfflHolesName,"HLT/IDTrkHole"))->Fill((float)(k+1));
	    ((TH1F *) hist(tmpOfflHolesName,"HLT/IDTrkHoleLS"))->Fill((float)(k+1));
	  }    
	if (!tLayers[k] && oLayers[k]) 
	  {
	    nTrigHoles++;
	    ((TH1F *) hist(tmpTrigHolesName,"HLT/IDTrkHole"))->Fill((float)(k+1));
	    ((TH1F *) hist(tmpTrigHolesName,"HLT/IDTrkHoleLS"))->Fill((float)(k+1));
	  }
      }
    ((TH1F *) hist(tmpNOfflHolesName,"HLT/IDTrkHole"))->Fill((float)(nOfflHoles));
    ((TH1F *) hist(tmpNOfflHolesName,"HLT/IDTrkHoleLS"))->Fill((float)(nOfflHoles));
    ((TH1F *) hist(tmpNTrigHolesName,"HLT/IDTrkHole"))->Fill((float)(nTrigHoles));
    ((TH1F *) hist(tmpNTrigHolesName,"HLT/IDTrkHoleLS"))->Fill((float)(nTrigHoles));
  }
  
  return;
}



//bool* HLTIDtrkMonTool::getPattern(const Rec::TrackParticle& track)
void HLTIDtrkMonTool::getPattern(const Rec::TrackParticle& track, bool oLayers[19])
{
  const Trk::TrackSummary* summary = track.trackSummary();
  //  bool oLayers[19];
  for (int k=0;k<19;k++) oLayers[k] = false;
  if (summary->isHit(Trk::pixelBarrel0)) oLayers[0] = true;
  if (summary->isHit(Trk::pixelBarrel1)) oLayers[1] = true;
  if (summary->isHit(Trk::pixelBarrel2)) oLayers[2] = true;
  if (summary->isHit(Trk::sctBarrel0)) oLayers[3] = true;
  if (summary->isHit(Trk::sctBarrel1)) oLayers[4] = true;
  if (summary->isHit(Trk::sctBarrel2)) oLayers[5] = true;
  if (summary->isHit(Trk::sctBarrel3)) oLayers[6] = true;
  if (summary->isHit(Trk::pixelEndCap0)) oLayers[7] = true;
  if (summary->isHit(Trk::pixelEndCap1)) oLayers[8] = true;
  if (summary->isHit(Trk::pixelEndCap2)) oLayers[9] = true;
  if (summary->isHit(Trk::sctEndCap0)) oLayers[10] = true;
  if (summary->isHit(Trk::sctEndCap1)) oLayers[11] = true;
  if (summary->isHit(Trk::sctEndCap2)) oLayers[12] = true;
  if (summary->isHit(Trk::sctEndCap3)) oLayers[13] = true;
  if (summary->isHit(Trk::sctEndCap4)) oLayers[14] = true;
  if (summary->isHit(Trk::sctEndCap5)) oLayers[15] = true;
  if (summary->isHit(Trk::sctEndCap6)) oLayers[16] = true;
  if (summary->isHit(Trk::sctEndCap7)) oLayers[17] = true;
  if (summary->isHit(Trk::sctEndCap8)) oLayers[18] = true; 
  //return (*oLayers);
}




//bool* HLTIDtrkMonTool::getPattern(const TrigInDetTrack& track)
void HLTIDtrkMonTool::getPattern(const TrigInDetTrack& track, bool tLayers[19])
{
  //  bool tLayers[19];
  for (int k=0;k<19;k++) tLayers[k] = false;
  long hitPattern = track.HitPattern();
  for (int k=0;k<19;k++)
    {
      long mask = 1 << k;
      if ( (hitPattern & mask) != 0 ) tLayers[k] = true;
    }
  //return tLayers;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

template <class track1, class track2>
void HLTIDtrkMonTool::EfficiencyResidual(int PairID, 
					 const std::vector< track1 > & precise, 
					 const std::vector< track2 > & /*coarse*/, 
					 std::map<int, vector<int> > & precise2coarse,
					 std::string /*type*/ ){

  //+++ Fill Denominator Histogram
  for(unsigned int i=0;i<precise.size();i++){
    
    //+++ Offline selection
    if (fabs(precise[i].Pt()) < m_PtMin) continue;
    if (fabs(precise[i].Eta()) > m_EtaMax) continue;
    if (fabs(precise[i].z0) > m_Z0Max) continue;
    if (fabs(precise[i].d0) > m_D0Max) continue;
    float tmp_SiHits = 2 * precise[i].nPixHits + precise[i].nSctHits;
    if (tmp_SiHits < m_SiHitMin && m_MatchPairs[PairID] != "TRTOff") continue;  // Exception for TRTSegFinder

    //+++ Fill Nominator
    bool tmpPassedAny = false;
    const std::map<int,std::vector<int> >::iterator iter = precise2coarse.find(i);
    if (iter!=precise2coarse.end()){
      if (iter->second.size() > 0) tmpPassedAny = true ;
    }
    
    double PreciseEta = precise[i].Eta();
    std::string tmpEtaName = m_TrigEffEta + m_MatchPairs[PairID];
    double PrecisePhi = precise[i].Phi();
    std::string tmpPhiName = m_TrigEffPhi + m_MatchPairs[PairID];
    double PreciseZ0 = precise[i].z0;
    std::string tmpZ0Name = m_TrigEffZ0 + m_MatchPairs[PairID];
    double PreciseD0 = precise[i].d0;
    std::string tmpD0Name = m_TrigEffD0 + m_MatchPairs[PairID];

    if ( tmpPassedAny ) {
      ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEff"))->Fill(PreciseEta,100.,1.);
      ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEff"))->Fill(PrecisePhi,100.,1.);
      ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEff"))->Fill(PreciseZ0,100.,1.);
      ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEff"))->Fill(PreciseD0,100.,1.);
      ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEffLS"))->Fill(PreciseEta,100.,1.);
      ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEffLS"))->Fill(PrecisePhi,100.,1.);
      ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEffLS"))->Fill(PreciseZ0,100.,1.);
      ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEffLS"))->Fill(PreciseD0,100.,1.);
    } else {
      ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEff"))->Fill(PreciseEta,0.,1.);
      ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEff"))->Fill(PrecisePhi,0.,1.);
      ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEff"))->Fill(PreciseZ0,0.,1.);
      ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEff"))->Fill(PreciseD0,0.,1.);
      ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEffLS"))->Fill(PreciseEta,0.,1.);
      ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEffLS"))->Fill(PrecisePhi,0.,1.);
      ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEffLS"))->Fill(PreciseZ0,0.,1.);
      ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEffLS"))->Fill(PreciseD0,0.,1.);
    }
  }

  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void HLTIDtrkMonTool::EfficiencyOR( int PairID ) {

  //+++ Fill Denominator Histogram
  for(unsigned int i=0;i<m_OfflineTrackVectors.size();i++){
    
    //+++ Offline selection
    if (fabs(m_OfflineTrackVectors[i].Pt()) < m_PtMin) continue;
    if (fabs(m_OfflineTrackVectors[i].Eta()) > m_EtaMax) continue;
    if (fabs(m_OfflineTrackVectors[i].z0) > m_Z0Max) continue;
    if (fabs(m_OfflineTrackVectors[i].d0) > m_D0Max) continue;

    //+++ Fill Nominator
    bool tmpPassedAny = false;
    std::map<int,std::vector<int> >::iterator iter = m_OfflineToIDSCAN.find(i);
    if (iter!=m_OfflineToIDSCAN.end()){
      if (iter->second.size() > 0) tmpPassedAny = true ;
    } 
    iter = m_OfflineToSiTrack.find(i);
    if (iter!=m_OfflineToSiTrack.end()){
      if (iter->second.size() > 0) tmpPassedAny = true ;
    } 
    iter = m_OfflineToTRT.find(i);
    if (iter!=m_OfflineToTRT.end()){
      if (iter->second.size() > 0) tmpPassedAny = true ;
    } 
    
    double PreciseEta = m_OfflineTrackVectors[i].Eta();
    std::string tmpEtaName = m_TrigEffEta + m_MatchPairs[PairID];
    double PrecisePhi = m_OfflineTrackVectors[i].Phi();
    std::string tmpPhiName = m_TrigEffPhi + m_MatchPairs[PairID];
    double PreciseZ0 = m_OfflineTrackVectors[i].z0;
    std::string tmpZ0Name = m_TrigEffZ0 + m_MatchPairs[PairID];
    double PreciseD0 = m_OfflineTrackVectors[i].d0;
    std::string tmpD0Name = m_TrigEffD0 + m_MatchPairs[PairID];

     if ( tmpPassedAny ){ 
       ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEff"))->Fill(PreciseEta,100.,1.);
       ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEff"))->Fill(PrecisePhi,100.,1.);
       ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEff"))->Fill(PreciseZ0,100.,1.);
       ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEff"))->Fill(PreciseD0,100.,1.);
       ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEffLS"))->Fill(PreciseEta,100.,1.);
       ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEffLS"))->Fill(PrecisePhi,100.,1.);
       ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEffLS"))->Fill(PreciseZ0,100.,1.);
       ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEffLS"))->Fill(PreciseD0,100.,1.);
     } else {
       ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEff"))->Fill(PreciseEta,0.,1.);
       ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEff"))->Fill(PrecisePhi,0.,1.);
       ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEff"))->Fill(PreciseZ0,0.,1.);
       ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEff"))->Fill(PreciseD0,0.,1.);
       ((TProfile *) hist(tmpEtaName,"HLT/IDTrkEffLS"))->Fill(PreciseEta,0.,1.);
       ((TProfile *) hist(tmpPhiName,"HLT/IDTrkEffLS"))->Fill(PrecisePhi,0.,1.);
       ((TProfile *) hist(tmpZ0Name,"HLT/IDTrkEffLS"))->Fill(PreciseZ0,0.,1.);
       ((TProfile *) hist(tmpD0Name,"HLT/IDTrkEffLS"))->Fill(PreciseD0,0.,1.);
     }
     
  }  
  
  return;
}


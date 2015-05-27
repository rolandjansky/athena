/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************
//
// NAME:     HLTBjetMonTool.cxx
// PACKAGE:  Trigger/TrigMonitoring/TrigBjetMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@cern.ch
// 
// ***************************************************

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"   //EN

#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"

#include "JetUtils/JetCaloQualityUtils.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "Particle/TrackParticleContainer.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrigParticle/TrigEFBjet.h"
#include "TrigParticle/TrigEFBjetContainer.h"

#include "TrigBjetMonitoring/TaggerHelper.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <math.h>

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

#include "xAODTracking/VertexContainer.h"

#include "xAODTracking/TrackParticle.h"

#include "xAODEventInfo/EventInfo.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"



#include "TrigBjetMonitoring/HLTBjetMonTool.h"

#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "GaudiKernel/ListItem.h"

namespace Analysis { class MuonContainer; }



//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::HLTBjetMonTool(const std::string & type, const std::string & name, const IInterface* parent) :
  IHLTMonTool(type, name, parent),
  m_trackJetFinderTool("TrigTrackJetFinderTool"),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty ("monitoring_bjet",       m_TriggerChainBjet);          
  declareProperty ("monitoring_mujet",      m_TriggerChainMujet);          

}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::~HLTBjetMonTool() {

 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::init() {


  StatusCode sc = m_trackJetFinderTool.retrieve();
  if(sc.isFailure()) {
    *m_log << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
    return StatusCode::FAILURE;
  } else
    *m_log << MSG::VERBOSE << "Retrieved tool " << m_trackJetFinderTool << endreq;

  *m_log << MSG::VERBOSE << "in HLTBjetMonTool::init - retrieved tool: " << m_trackJetFinderTool << endreq;

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::proc(){
#else
  StatusCode HLTBjetMonTool::proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun){  
#endif
  
    *m_log << MSG::VERBOSE << "in HLTBjetMonTool::proc" << endreq;

  return StatusCode::SUCCESS;

  }


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::book(){
#else
  StatusCode HLTBjetMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun){  
#endif

  
    *m_log << MSG::VERBOSE<< "HLTBjetMonTool::book" << endreq;
    *m_log << MSG::INFO<< " entered HLTBjetMonTool::book" << endreq;

    addMonGroup(new MonGroup(this,"HLT/BjetMon", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
 
    *m_log << MSG::INFO<< "in HLTBjetMonTool::book added directory HLT/BjetMon, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED << endreq;

    *m_log << MSG::INFO<< "in HLTBjetMonTool::book newRun: " << newRun << endreq;
  

    if(newRun){
      
      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN

      *m_log << MSG::INFO<< "  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED << endreq;


      // Primary vertices
      //    Off-line
      addHistogram(new TH1F("nPV","Number of offline PV per event", 20, 0., 20.));
      addHistogram(new TH1F("PVx","offline xPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVy","offline yPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVz","offline zPV", 200, -100., 100.));
      //    On-line
      addHistogram(new TH1F("nPV_tr","Number of online PV per event", 20, 0., 20.));
      addHistogram(new TH1F("PVx_tr","online xPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVy_tr","online yPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVz_tr","online zPV", 200, -100., 100.));
      addHistogram(new TH1F("diffzPV0offPVon","z difference of the 1st offline and online PV", 200, -1., 1.));
      // Tracks online   
      addHistogram(new TH1F("nTrack","Number of tracks", 20, 0., 20.));
      addHistogram(new TH1F("d0","d0 of tracks", 200, -2., 2.));
      addHistogram(new TH1F("z0","z0 of tracks", 200, -100., 100.));
      addHistogram(new TH1F("ed0","err_d0 of tracks", 200, 0., 1.));
      addHistogram(new TH1F("ez0","err_z0 of tracks", 200, 0., 5.));
      addHistogram(new TH1F("diffz0PV0","z0 of tracks wrt 1st offline PV", 200, -10., 10.));
      addHistogram(new TH1F("sigz0PV","z0-PV/errz0", 200, -20., 20.));
      addHistogram(new TH1F("trkPt","Pt of tracks", 1000, 0., 20000.));
      addHistogram(new TH2F("trkEtaPhi","Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
      //   Jets online
      addHistogram(new TH1F("nJet","Number of jets", 20, 0., 20.));
      addHistogram(new TH1F("jetPt","Pt of jets", 500, 0., 500.));
      addHistogram(new TH2F("jetEtaPhi","Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
      //   B-jets online
      addHistogram(new TH1F("IP3D_pu_tr","IP3D_pu probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pb_tr","IP3D_pb probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pc_tr","IP3D_pc probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("wIP3D_Rbu_tr","LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wSV1_Rbu_tr","LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wCOMB_Rbu_tr","LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wMV1_tr","MV1 discriminant", 200, 0., 1.));
      addHistogram(new TH1F("xMVtx_tr","SV mass", 200, 0., 10000.));
      addHistogram(new TH1F("xNVtx_tr","Number of SV", 10, 0., 10.));



    } // if newRun



    return StatusCode::SUCCESS;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  StatusCode HLTBjetMonTool::fill() {

    *m_log << MSG::INFO<< "====> Entering HLTBjetMonTool::fill()" << endreq;

  
  ////////////////////////////////////
  //setCurrentMonGroup("HLT/BjetMon/Shifter");
  ////////////////////////////////////


  // Trigger (chain) name definition

  int size_TriggerChainBjet =m_TriggerChainBjet.size();
  *m_log << MSG::INFO << " Size of Trigger chain " << size_TriggerChainBjet << endreq;
  if (size_TriggerChainBjet <= 0) {
    return StatusCode::SUCCESS;
  }
  std::string chainName = "Not fired";

  for (int i =0; i<size_TriggerChainBjet; i++){
    if (!getTDT()->isPassed(m_TriggerChainBjet.at(i))){
      *m_log << MSG::INFO << " Trigger chain " << i << " " << m_TriggerChainBjet.at(i) << " not fired." << endreq;
    } else {
      chainName = m_TriggerChainBjet.at(i);
      *m_log << MSG::INFO << " Trigger chain " << i << " " << chainName << " fired." << endreq;
      goto Fired;
    }
  }
  *m_log << MSG::INFO << " Trigger chain of size: " << size_TriggerChainBjet << " " << chainName << " at all - RETURN " << endreq;
  return StatusCode::SUCCESS;

 Fired:

  const xAOD::EventInfo* eventInfo = 0;
  //  CHECK( evtStore()->retrieve( eventInfo) );
  CHECK( evtStore()->retrieve( eventInfo, "EventInfo") );
  bool MCflag = true;
  if ( !eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) MCflag = false;
  *m_log << MSG::INFO << " MCflag: " << MCflag << endreq;

  /////////////////////////////////////////
  //
  // Carlo's method, see /Trigger/TrigAnalysis/TrigBtagAnalysis/trunk/src/TrigBtagValidation.cxx
  //                 and /TrigBtagTuning.cxx particularly L337 and L1192
  //
  /////////////////////////////////////////

  std::string trigItem = chainName;
  ATH_MSG_INFO("PROCESSING TRIGITEM  -  " << trigItem);

  // Get truth jets
  if (MCflag) {
    const xAOD::JetContainer* truthjets = 0;
    ATH_CHECK( evtStore()->retrieve(truthjets,"AntiKt4TruthJets") );
    ATH_MSG_INFO("RETRIEVED TRUTH JETS  - size: " << truthjets->size());
  } // MCflag


  // Get offline PV
  const xAOD::VertexContainer* offlinepv = 0;
  ATH_CHECK( evtStore()->retrieve(offlinepv, "PrimaryVertices") );
  ATH_MSG_INFO("RETRIEVED OFFLINE PV  - size: " << offlinepv->size());
  float offlinepvz = offlinepv->front()->z();
  ATH_MSG_INFO(" 1st zPV a la Carlo: " << offlinepvz);
  hist("nPV","HLT/BjetMon/Shifter")->Fill(offlinepv->size());
  for (unsigned int j = 0; j<offlinepv->size(); j++){
    hist("PVx","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->x());
    hist("PVy","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->y());
    hist("PVz","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->z());
  }


  // Get online combinations

  Trig::FeatureContainer fc = m_trigDec->features(trigItem);
  const std::vector< Trig::Combination >& bjetCombs = fc.getCombinations();
  ATH_MSG_INFO("RETRIEVED " << bjetCombs.size() << " COMBINATIONS FOR "  << trigItem);

  // Loop on combinations
  std::vector< Trig::Combination >::const_iterator bjetComb;
  for( bjetComb = bjetCombs.begin(); bjetComb != bjetCombs.end(); ++bjetComb ) {

    const Trig::Combination& comb = *bjetComb;
    ATH_MSG_INFO("------------ NEW COMBINATION ------------");

#define HISTO
#ifdef HISTO
    // Get online pv - histo
    const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_histo = comb.get<xAOD::VertexContainer>("EFHistoPrmVtx");
    ATH_MSG_INFO("RETRIEVED PV (H) -   size: " << onlinepvs_histo.size());
    if ( onlinepvs_histo.size() ) {
      const xAOD::VertexContainer* onlinepv_histo = onlinepvs_histo[0].cptr();
      ATH_MSG_INFO("                 -   nVert: " << onlinepv_histo->size());
      hist("nPV_tr","HLT/BjetMon/Shifter")->Fill(onlinepv_histo->size());
      if(onlinepv_histo->size()) {
	hist("PVx_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->x());
	hist("PVy_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->y());
	hist("PVz_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->z());
	hist("diffzPV0offPVon","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->z()-offlinepvz);
	ATH_MSG_INFO("         Online PV - histo   -   z[0]: " << (*(onlinepv_histo))[0]->z());
      }  // if
    } // onlinepvs_histo.size

#else
    // Get online pv - id tracking
    const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_id = comb.get<xAOD::VertexContainer>("xPrimVx");
    ATH_MSG_INFO("RETRIEVED PV (I) -   size: " << onlinepvs_id.size());
    if(onlinepvs_id->size()) {
      const xAOD::VertexContainer* onlinepv_id = onlinepvs_id[0].cptr();
      ATH_MSG_INFO("                 -   nVert: " << onlinepv_id->size());
      hist("nPV_tr","HLT/BjetMon/Shifter")->Fill(onlinepv_id->size());
      if(onlinepv_id->size()) {
	hist("PVz_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_id))[0]->z());
	hist("diffzPV0offPVon","HLT/BjetMon/Shifter")->Fill((*(onlinepv_id))[0]->z()-offlinepvz);
	ATH_MSG_INFO("          Online PV - id tracking   -   z[0]: " << (*(onlinepv_id))[0]->z());
      } // if
    } // onlinepvs_id.size
#endif


    // Get online jet
    const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>();
    ATH_MSG_INFO("RETRIEVED JETS   -   size: " << onlinejets.size());
    if(onlinejets.size()) {
      const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
      ATH_MSG_INFO("                 -   nJet: " << onlinejet->size());
      int nJet = onlinejet->size();
      hist("nJet","HLT/BjetMon/Shifter")->Fill(nJet);
      for(const auto* jet : *onlinejet) {
	ATH_MSG_INFO("                 -   pt/eta/phi: " << jet->pt() << " / " << jet->eta() << " / " << jet->phi());
	hist("jetPt","HLT/BjetMon/Shifter")->Fill((jet->pt())*1.e-3);
	hist2("jetEtaPhi","HLT/BjetMon/Shifter")->Fill(jet->eta(),jet->phi());
      } // for online jet
    }//onlinejets.size


    // Get online track particles
    const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>();
    ATH_MSG_INFO("RETRIEVED TRACKS -   size: " << onlinetracks.size());
    if ( onlinetracks.size()>0 ) { 
      const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
      ATH_MSG_INFO("                 -   nTrack: " << onlinetrack->size());
      int nTrack = onlinetrack->size();
      hist("nTrack","HLT/BjetMon/Shifter")->Fill(nTrack);
      for(const auto* trk : *onlinetrack) {
	ATH_MSG_INFO("     pT: " << trk->pt() << " Eta: " << trk->eta() << " Phi: " << trk->phi() << " d0: " << trk->d0() << " z0 - zPVoffl: " << trk->z0()-offlinepvz );
	hist("d0","HLT/BjetMon/Shifter")->Fill(trk->d0());
	hist("z0","HLT/BjetMon/Shifter")->Fill(trk->z0());
	hist("ed0","HLT/BjetMon/Shifter")->Fill(Amg::error(trk->definingParametersCovMatrix(), 0));
	hist("ez0","HLT/BjetMon/Shifter")->Fill(Amg::error(trk->definingParametersCovMatrix(), 1));
	hist("diffz0PV0","HLT/BjetMon/Shifter")->Fill(trk->z0()-offlinepvz);
	float errz0 = Amg::error(trk->definingParametersCovMatrix(), 1);
	if (errz0 >0.) hist("sigz0PV","HLT/BjetMon/Shifter")->Fill( (trk->z0()-offlinepvz)/errz0 );
	hist("trkPt","HLT/BjetMon/Shifter")->Fill(trk->pt());
	hist2("trkEtaPhi","HLT/BjetMon/Shifter")->Fill(trk->eta(),trk->phi());
      } // for online track particles
    } // onlinetracks.size

   
    // Get online bjet from xAOD BTaggingContainer
    const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
    ATH_MSG_INFO("RETRIEVED BJETS  -   size: " << onlinebjets.size());
    if(onlinebjets.size()) {
      const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
      ATH_MSG_INFO("                 -   nBjet: " << onlinebjet->size());
      for(const auto* bjet : *onlinebjet) {
	double wIP3D, wSV1, wCOMB, wMV1 = 0.;
	bjet->loglikelihoodratio("IP3D", wIP3D);
	bjet->loglikelihoodratio("SV1", wSV1);
	wCOMB = wIP3D+wSV1;
	wMV1 = bjet->MV1_discriminant();
	ATH_MSG_INFO("                 -   IP3Dpu / IP3Dpb / IP3Dpc: " << bjet->IP3D_pu() << " / " << bjet->IP3D_pb() << " / " << bjet->IP3D_pc());
	hist("IP3D_pu_tr","HLT/BjetMon/Shifter")->Fill(bjet->IP3D_pu());
	hist("IP3D_pb_tr","HLT/BjetMon/Shifter")->Fill(bjet->IP3D_pb());
	hist("IP3D_pc_tr","HLT/BjetMon/Shifter")->Fill(bjet->IP3D_pc());
	ATH_MSG_INFO("                 -   IP3D / SV1 / IP3D+SV1 / MV1: " << wIP3D << " / " << wSV1 << " / " << wCOMB << " / " << wMV1);
	hist("wIP3D_Rbu_tr","HLT/BjetMon/Shifter")->Fill(wIP3D);
	hist("wSV1_Rbu_tr","HLT/BjetMon/Shifter")->Fill(wSV1);
	hist("wCOMB_Rbu_tr","HLT/BjetMon/Shifter")->Fill(wCOMB);
	hist("wMV1_tr","HLT/BjetMon/Shifter")->Fill(wMV1);
      } // for online bjet
    } // onlinebjets.size

    // Get online bjet from TrigEFBjetContainer                                                                                                                               
    const std::vector< Trig::Feature<TrigEFBjetContainer> > EFonlinebjets = comb.get<TrigEFBjetContainer>();
    ATH_MSG_INFO("RETRIEVED BJETS  -   size: " << EFonlinebjets.size());
    if(EFonlinebjets.size()) {
      const TrigEFBjetContainer* EFonlinebjet = EFonlinebjets[0].cptr();
      ATH_MSG_INFO("                 -   nBjet: " << EFonlinebjet->size());
      for(const auto* bjet : *EFonlinebjet) {
	ATH_MSG_INFO("                 -   MVTX / EVTX / NVTX: " << bjet->xMVtx() << " / " << bjet->xEVtx() << " / " << bjet->xNVtx());
	hist("xMVtx_tr","HLT/BjetMon/Shifter")->Fill(bjet->xMVtx());
	hist("xNVtx_tr","HLT/BjetMon/Shifter")->Fill(bjet->xNVtx());
      } //EFonlinebjet 
    } // EFonlinebjets.size

  } // for bjetComb

  *m_log << MSG::INFO<< "====> Ended successfully HLTBjetMonTool::fill()" << endreq;

  
  return StatusCode::SUCCESS;

}





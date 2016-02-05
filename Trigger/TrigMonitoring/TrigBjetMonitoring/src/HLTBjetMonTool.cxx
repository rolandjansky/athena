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
  declareProperty ("EtThreshold",           m_etCut   = 10.);
  declareProperty ("PriVtxKey",             m_priVtxKey    = "EFHistoPrmVtx"); // xPrimVx  for PV from ID Tracking
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::~HLTBjetMonTool() {

 
}

//** ----------------------------------------------------------------------------------------------------------------- **//

float HLTBjetMonTool::phiCorr(float phi) {
  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;
  return phi;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::init() {


  StatusCode sc = m_trackJetFinderTool.retrieve();
  if(sc.isFailure()) {
    *m_log << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
    return StatusCode::FAILURE;
  } else
    *m_log << MSG::INFO << "Retrieved tool " << m_trackJetFinderTool << endreq;

  *m_log << MSG::INFO << "in HLTBjetMonTool::init - retrieved tool: " << m_trackJetFinderTool << endreq;

  *m_log << MSG::INFO << " ===> in HLTBjetMonTool::init - Muon-jet parameters: m_etCut = " << m_etCut << " m_priVtxKey = " << m_priVtxKey << endreq;

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::proc(){
#else
  StatusCode HLTBjetMonTool::proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun){  
#endif
  
    *m_log << MSG::INFO << "in HLTBjetMonTool::proc" << endreq;

  return StatusCode::SUCCESS;

  }


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::book(){
#else
  StatusCode HLTBjetMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun){  
#endif

  
    *m_log << MSG::INFO<< "HLTBjetMonTool::book" << endreq;
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
      addHistogram(new TH1F("trkPt","Pt of tracks", 100, 0., 50.));
      addHistogram(new TH2F("trkEtaPhi","Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
      //   Jets online
      addHistogram(new TH1F("nJet","Number of jets", 20, 0., 20.));
      addHistogram(new TH1F("jetPt","Pt of jets", 100, 0., 250.));
      addHistogram(new TH2F("jetEtaPhi","Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
      //   B-jets online
      addHistogram(new TH1F("IP3D_pu_tr","IP3D_pu probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pb_tr","IP3D_pb probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pc_tr","IP3D_pc probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("wIP3D_Rbu_tr","LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wSV1_Rbu_tr","LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wCOMB_Rbu_tr","LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wMV1_tr","MV1 discriminant", 200, 0., 1.));
      addHistogram(new TH1F("xMVtx_tr","SV mass", 50, 0., 10.));
      addHistogram(new TH1F("xNVtx_tr","Number of SV", 10, 0., 10.));
      //  Mu-Jets
      addHistogram(new TH1F("nMuon","Number of muons", 20, 0., 20.));
      addHistogram(new TH1F("muonPt","Pt of muons", 100, 0., 250.));
      addHistogram(new TH2F("muonEtaPhi","Phi vs Eta of muons", 20, -5., 5., 20, -3.1416, 3.1416));
      addHistogram(new TH1F("DeltaRAll","DeltaR between muon and any jet", 100, 0., 6.));
      addHistogram(new TH1F("DeltaZAll","DeltaZ between muon and any jet", 100, 0., 10.));



    } // if newRun



    return StatusCode::SUCCESS;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  StatusCode HLTBjetMonTool::fill() {

    *m_log << MSG::DEBUG<< "====> Entering HLTBjetMonTool::fill()" << endreq;

  
  ////////////////////////////////////
  //setCurrentMonGroup("HLT/BjetMon/Shifter");
  ////////////////////////////////////


  // Trigger (chain) name definition

  int size_TriggerChainBjet =m_TriggerChainBjet.size();
  int size_TriggerChainMujet =m_TriggerChainMujet.size();
  *m_log << MSG::DEBUG << " Size of Bjet Trigger chain " << size_TriggerChainBjet << endreq;
  *m_log << MSG::DEBUG << " Size of Mujet Trigger chain " << size_TriggerChainMujet << endreq;
  if ( (size_TriggerChainBjet <= 0) && (size_TriggerChainMujet <= 0) ) {
    return StatusCode::SUCCESS;
  }
  std::string chainName = "Not fired";
  std::vector<std::string> FiredChainNames;

  for (int i =0; i<size_TriggerChainBjet; i++){
    if (!getTDT()->isPassed(m_TriggerChainBjet.at(i))){
      *m_log << MSG::DEBUG << " Trigger chain " << i << " " << m_TriggerChainBjet.at(i) << " not fired." << endreq;
    } else {
      chainName = m_TriggerChainBjet.at(i);
      *m_log << MSG::DEBUG << " Trigger chain " << i << " " << chainName << " fired." << endreq;
      // now check if configured
      std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
      //      if ( selectChains.size() ) goto Fired;
      if ( selectChains.size() ) FiredChainNames.push_back(chainName);
      //      goto Fired; // w/o checking if configured
    } // else
  } //i

  for (int i =0; i<size_TriggerChainMujet; i++){
    if (!getTDT()->isPassed(m_TriggerChainMujet.at(i))){
      *m_log << MSG::DEBUG << " Trigger chain " << i << " " << m_TriggerChainMujet.at(i) << " not fired." << endreq;
    } else {
      chainName = m_TriggerChainMujet.at(i);
      *m_log << MSG::DEBUG << " Trigger chain " << i << " " << chainName << " fired." << endreq;
      // now check if configured                                                                                                                                                      
      std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
      //      if ( selectChains.size() ) goto Fired;
      if ( selectChains.size() ) FiredChainNames.push_back(chainName);
      //      goto Fired; // w/o checking if configured                                                                                                                                   
    } // else
  } // i

  if ( FiredChainNames.size() ) *m_log << MSG::DEBUG << " " << FiredChainNames.size() << " trigger chains fired: " << endreq;
  for (unsigned int i = 0; i< FiredChainNames.size(); i++) {
    *m_log << MSG::DEBUG << FiredChainNames.at(i) << endreq;
  }

  if ( FiredChainNames.size() ) goto Fired;

  *m_log << MSG::INFO << " ===> No trigger fired neither for TriggerChainBjet of size: " << size_TriggerChainBjet 
	 << " nor for TriggerChainMujet of size: " << size_TriggerChainMujet << " RETURN from HLTBjetMonTool::fill() ! " << endreq;
  return StatusCode::SUCCESS;

 Fired:

  const xAOD::EventInfo* eventInfo = 0;
  //  CHECK( evtStore()->retrieve( eventInfo) );
  CHECK( evtStore()->retrieve( eventInfo, "EventInfo") );
  bool MCflag = true;
  if ( !eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) MCflag = false;
  *m_log << MSG::DEBUG << " MCflag: " << MCflag << endreq;

  /////////////////////////////////////////
  //
  // Carlo's method, see /Trigger/TrigAnalysis/TrigBtagAnalysis/trunk/src/TrigBtagValidation.cxx
  //                 and /TrigBtagTuning.cxx particularly L337 and L1192
  //
  /////////////////////////////////////////


  // Get truth jets
  if (MCflag) {
    const xAOD::JetContainer* truthjets = 0;
    ATH_CHECK( evtStore()->retrieve(truthjets,"AntiKt4TruthJets") );
    ATH_MSG_DEBUG("RETRIEVED TRUTH JETS  - size: " << truthjets->size());
  } // MCflag


  // Get offline PV
  float offlinepvz(0.);
  const xAOD::VertexContainer* offlinepv = 0;
  if ( evtStore()->contains<xAOD::VertexContainer>("PrimaryVertices") ) { 
    ATH_CHECK( evtStore()->retrieve(offlinepv, "PrimaryVertices") );
    ATH_MSG_DEBUG("RETRIEVED OFFLINE PV  - size: " << offlinepv->size());
    if ( offlinepv->size() ) {
      offlinepvz = offlinepv->front()->z();
      ATH_MSG_DEBUG(" 1st zPV a la Carlo: " << offlinepvz);
      hist("nPV","HLT/BjetMon/Shifter")->Fill(offlinepv->size());
      for (unsigned int j = 0; j<offlinepv->size(); j++){
	hist("PVx","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->x());
	hist("PVy","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->y());
	hist("PVz","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->z());
      } // j
    } // if size
  } // evtStore

  // Get online combinations

  // Loop on trigger items
  for (unsigned int ichain = 0; ichain < FiredChainNames.size(); ichain++) {
    //  unsigned int ichain = 0;
    std::string trigItem = FiredChainNames.at(ichain);
    std::string m_jetKey = "";
    std::size_t found = trigItem.find("split");
    if (found!=std::string::npos) m_jetKey = "SplitJet";
    *m_log << MSG::DEBUG << " Trigger chain name: " << trigItem << " m_jetKey:" << m_jetKey << endreq; 
    ATH_MSG_DEBUG("PROCESSING TRIGITEM  -  " << trigItem);

    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    const std::vector< Trig::Combination >& bjetCombs = fc.getCombinations();
    ATH_MSG_DEBUG("RETRIEVED " << bjetCombs.size() << " COMBINATIONS FOR "  << trigItem);

    // Loop on combinations
    std::vector< Trig::Combination >::const_iterator bjetComb;
    for( bjetComb = bjetCombs.begin(); bjetComb != bjetCombs.end(); ++bjetComb ) {
      const Trig::Combination& comb = *bjetComb;
      ATH_MSG_DEBUG("------------ NEW COMBINATION ------------");
      float m_zPrmVtx = 0.; // used for muon-jets

      // Get online pv - histo
      const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_histo = comb.get<xAOD::VertexContainer>(m_priVtxKey);
      ATH_MSG_DEBUG("RETRIEVED PV (H) -   size: " << onlinepvs_histo.size());
      if ( onlinepvs_histo.size() ) {
	const xAOD::VertexContainer* onlinepv_histo = onlinepvs_histo[0].cptr();
	ATH_MSG_DEBUG("                 -   nVert: " << onlinepv_histo->size());
	hist("nPV_tr","HLT/BjetMon/Shifter")->Fill(onlinepv_histo->size());
	if(onlinepv_histo->size()) {
	  hist("PVx_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->x());
	  hist("PVy_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->y());
	  hist("PVz_tr","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->z());
	  m_zPrmVtx = (*(onlinepv_histo))[0]->z();
	  hist("diffzPV0offPVon","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->z()-offlinepvz);  
	  ATH_MSG_DEBUG("         Online PV - histo   -   z[0]: " << (*(onlinepv_histo))[0]->z());  
	}  // if
      } // onlinepvs_histo.size

      // Get online jet
      const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>(m_jetKey);
      ATH_MSG_DEBUG("RETRIEVED JETS   -   size: " << onlinejets.size());
      if(onlinejets.size()) {
	const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
	ATH_MSG_DEBUG("                 -   nJet: " << onlinejet->size());
	int nJet = onlinejet->size();
	hist("nJet","HLT/BjetMon/Shifter")->Fill(nJet);
	for(const auto* jet : *onlinejet) {
	  ATH_MSG_DEBUG("                 -   pt/eta/phi: " << (jet->pt())*1.e-3 << " / " << jet->eta() << " / " << jet->phi());
	  hist("jetPt","HLT/BjetMon/Shifter")->Fill((jet->pt())*1.e-3);
	  hist2("jetEtaPhi","HLT/BjetMon/Shifter")->Fill(jet->eta(),jet->phi());
	} // for online jet
      }//onlinejets.size

      // Get online muon                                                                                                                                                     
      const std::vector< Trig::Feature<xAOD::MuonContainer> > onlinemuons = comb.get<xAOD::MuonContainer>();
      ATH_MSG_DEBUG("RETRIEVED MUONS   -   size: " << onlinemuons.size());
      if(onlinemuons.size()) {
	const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	ATH_MSG_DEBUG("                 -   nMuon: " << onlinemuon->size());
	int nMuon = onlinemuon->size();
	hist("nMuon","HLT/BjetMon/Shifter")->Fill(nMuon);
	for(const auto* muon : *onlinemuon) {
	  ATH_MSG_DEBUG("                 -   pt/eta/phi: " << (muon->pt())*1.e-3 << " / " << muon->eta() << " / " << muon->phi());
	  hist("muonPt","HLT/BjetMon/Shifter")->Fill((muon->pt())*1.e-3);
	  hist2("muonEtaPhi","HLT/BjetMon/Shifter")->Fill(muon->eta(),muon->phi());
	} // for online muon                                                                                                                                                                      
      }//onlinemuons.size                                                                                                                                                                   

      // Loop over muons and jets to monitor muon-jets m_deltaZ and m_dR
      float muonEta=0, muonPhi=0, muonZ=0;
      float jetEta=0,  jetPhi=0, jetZ=0;
      float m_deltaEta=0, m_deltaPhi=0, m_deltaZ=0;
      double m_dR = 0.;
      if(onlinemuons.size()) {
	const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	for(const auto* muon : *onlinemuon) {
	  if(onlinejets.size()) {
	    const xAOD::Muon::MuonType muontype = muon->muonType();
	    if(!(muontype == xAOD::Muon::MuonType::Combined) ) continue;
	    muonEta = muon->eta();
	    muonPhi = muon->phi();
	    muonZ=0;
	    //	    muonZ= (*(muon->combinedTrackParticleLink()))->z0();
	    muonZ = (*(muon->combinedTrackParticleLink()))->z0()  + (*(muon->combinedTrackParticleLink()))->vz(); //LZ 131115
	    const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
	    for(const auto* jet : *onlinejet) {
	      if((jet)->p4().Et() < m_etCut) continue;
	      jetEta = (jet)->eta();
	      jetPhi = (jet)->phi();
	      jetZ=m_zPrmVtx;
	      m_deltaEta = muonEta - jetEta;
	      m_deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
	      m_deltaZ   = fabs(muonZ-jetZ);
	      m_dR = sqrt(m_deltaEta*m_deltaEta + m_deltaPhi*m_deltaPhi);
	      hist("DeltaZAll","HLT/BjetMon/Shifter")->Fill(m_deltaZ);
	      hist("DeltaRAll","HLT/BjetMon/Shifter")->Fill(m_dR);
	    } // for online jet                                                                                                                                                     
	  }//onlinejets.size                                                                                                                                                                   
	} // for online muon                                                                                                                                                                      
      }//onlinemuons.size                                                                                                                                                                   
      
      // Get online track particles
      const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>();
      ATH_MSG_DEBUG("RETRIEVED TRACKS -   size: " << onlinetracks.size());
      if ( onlinetracks.size()>0 ) { 
	const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
	ATH_MSG_DEBUG("                 -   nTrack: " << onlinetrack->size());
	int nTrack = onlinetrack->size();
	hist("nTrack","HLT/BjetMon/Shifter")->Fill(nTrack);
	for(const auto* trk : *onlinetrack) {
	  ATH_MSG_DEBUG("     pT: " << (trk->pt())*1.e-3 << " Eta: " << trk->eta() << " Phi: " << trk->phi() << " d0: " << trk->d0() << " z0 - zPVoffl: " << trk->z0()+trk->vz()-offlinepvz ); // John A
	  hist("d0","HLT/BjetMon/Shifter")->Fill(trk->d0());
	  hist("z0","HLT/BjetMon/Shifter")->Fill(trk->z0());
	  hist("ed0","HLT/BjetMon/Shifter")->Fill(Amg::error(trk->definingParametersCovMatrix(), 0));
	  hist("ez0","HLT/BjetMon/Shifter")->Fill(Amg::error(trk->definingParametersCovMatrix(), 1));
	  hist("diffz0PV0","HLT/BjetMon/Shifter")->Fill(trk->z0()+trk->vz()-offlinepvz); // John A
	  float errz0 = Amg::error(trk->definingParametersCovMatrix(), 1);
	  if (errz0 >0.) hist("sigz0PV","HLT/BjetMon/Shifter")->Fill( (trk->z0()+trk->vz()-offlinepvz)/errz0 ); // John A
	  hist("trkPt","HLT/BjetMon/Shifter")->Fill( (trk->pt())*1.e-3 );
	  hist2("trkEtaPhi","HLT/BjetMon/Shifter")->Fill(trk->eta(),trk->phi());
	} // for online track particles
      } // onlinetracks.size

      // Get online bjet from xAOD BTaggingContainer
      const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
      ATH_MSG_DEBUG("RETRIEVED BJETS  -   size: " << onlinebjets.size());
      if(onlinebjets.size()) {
	const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
	ATH_MSG_DEBUG("                 -   nBjet: " << onlinebjet->size());
	for(const auto* bjet : *onlinebjet) {
	  double wIP3D, wSV1, wCOMB, wMV1 = 0.;
	  bjet->loglikelihoodratio("IP3D", wIP3D);
	  bjet->loglikelihoodratio("SV1", wSV1);
	  wCOMB = wIP3D+wSV1;
	  wMV1 = bjet->MV1_discriminant();
	  ATH_MSG_DEBUG("                 -   IP3Dpu / IP3Dpb / IP3Dpc: " << bjet->IP3D_pu() << " / " << bjet->IP3D_pb() << " / " << bjet->IP3D_pc());
	  hist("IP3D_pu_tr","HLT/BjetMon/Shifter")->Fill(bjet->IP3D_pu());
	  hist("IP3D_pb_tr","HLT/BjetMon/Shifter")->Fill(bjet->IP3D_pb());
	  hist("IP3D_pc_tr","HLT/BjetMon/Shifter")->Fill(bjet->IP3D_pc());
	  ATH_MSG_DEBUG("                 -   IP3D / SV1 / IP3D+SV1 / MV1: " << wIP3D << " / " << wSV1 << " / " << wCOMB << " / " << wMV1);
	  hist("wIP3D_Rbu_tr","HLT/BjetMon/Shifter")->Fill(wIP3D);
	  hist("wSV1_Rbu_tr","HLT/BjetMon/Shifter")->Fill(wSV1);
	  hist("wCOMB_Rbu_tr","HLT/BjetMon/Shifter")->Fill(wCOMB);
	  hist("wMV1_tr","HLT/BjetMon/Shifter")->Fill(wMV1);
	} // for online bjet
      } // onlinebjets.size

      // Get online bjet from TrigEFBjetContainer                                                                                                                               
      const std::vector< Trig::Feature<TrigEFBjetContainer> > EFonlinebjets = comb.get<TrigEFBjetContainer>();
      ATH_MSG_DEBUG("RETRIEVED BJETS  -   size: " << EFonlinebjets.size());
      if(EFonlinebjets.size()) {
	const TrigEFBjetContainer* EFonlinebjet = EFonlinebjets[0].cptr();
	ATH_MSG_DEBUG("                 -   nBjet: " << EFonlinebjet->size());
	for(const auto* bjet : *EFonlinebjet) {
	  ATH_MSG_DEBUG("                 -   MVTX / EVTX / NVTX: " << (bjet->xMVtx())*1.e-3 << " / " << (bjet->xEVtx())*1.e-3 << " / " << bjet->xNVtx());
	  hist("xMVtx_tr","HLT/BjetMon/Shifter")->Fill( (bjet->xMVtx())*1.e-3 );
	  hist("xNVtx_tr","HLT/BjetMon/Shifter")->Fill(bjet->xNVtx());
	} //EFonlinebjet 
      } // EFonlinebjets.size

    } // for bjetComb

  } // ichain

  *m_log << MSG::DEBUG<< "====> Ended successfully HLTBjetMonTool::fill()" << endreq;


  
  return StatusCode::SUCCESS;

}





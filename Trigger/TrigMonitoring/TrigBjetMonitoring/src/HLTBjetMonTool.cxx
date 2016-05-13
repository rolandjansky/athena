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
// REVISED for RUN2: Elemer Nagy
// EMAIL:            Elemer.Nagy@cern.ch
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
/* Shaun Roe says it is alread included elsewhere so no need to be here
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
*/
// #include <math.h> // Shaun Roe says the use of all '.h' headers of the C++ standard library is deprecated
#include <cmath>

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
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_etCut(10.), m_sv1_infosource("SV1")
{
  declareProperty ("monitoring_bjet",       m_TriggerChainBjet);          
  declareProperty ("monitoring_mujet",      m_TriggerChainMujet);          
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

  /*
  StatusCode sc = m_trackJetFinderTool.retrieve();
  if(sc.isFailure()) {
    *m_log << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
    return StatusCode::FAILURE;
  } else
  */
  ATH_CHECK(m_trackJetFinderTool.retrieve());
  //    *m_log << MSG::INFO << "Retrieved tool " << m_trackJetFinderTool << endreq;
  //  ATH_MSG_INFO("Retrieved tool " << m_trackJetFinderTool);

  ATH_MSG_INFO("in HLTBjetMonTool::init - retrieved tool: " << m_trackJetFinderTool);

  m_etCut = 10.;
  ATH_MSG_INFO(" ===> in HLTBjetMonTool::init - Muon-jet parameters: m_etCut = " << m_etCut);

  m_sv1_infosource = "SV1";
  ATH_MSG_INFO(" ===> in HLTBjetMonTool::init - SV1  parameters: inputSV1SourceName = "  <<  m_sv1_infosource);


  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::proc(){
#else
  StatusCode HLTBjetMonTool::proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun){  
#endif
  
    ATH_MSG_INFO("in HLTBjetMonTool::proc");

  return StatusCode::SUCCESS;

  }


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::book(){
#else
  StatusCode HLTBjetMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun){  
#endif

  
    ATH_MSG_INFO("HLTBjetMonTool::book");
    ATH_MSG_INFO(" entered HLTBjetMonTool::book");

    addMonGroup(new MonGroup(this,"HLT/BjetMon", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
 
    ATH_MSG_INFO("in HLTBjetMonTool::book added directory HLT/BjetMon, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED);

    ATH_MSG_INFO("in HLTBjetMonTool::book newRun: " << newRunFlag() );
  

    if(newRunFlag()){
      
      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );

      //    Off-line
      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/Offline", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/Offline, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
      // PV
      addHistogram(new TH1F("nPV","Number of offline PV per event", 100, 0., 100.));
      addHistogram(new TH1F("PVx","offline xPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVy","offline yPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVz","offline zPV", 200, -200., 200.));

      //    On-line SplitChains
      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/Online_SplitChains", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/Online_SplitChains, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );


      // PV
      addHistogram(new TH1F("nPV_tr_Split","Number of online PV per event", 101, -1., 100.));
      addHistogram(new TH1F("PVx_tr_Split","online (1st) xPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVy_tr_Split","online (1st) yPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVz_tr_Split","online (1st) zPV", 200, -200., 200.));
      addHistogram(new TH1F("diffzPV0offPVon_Split","z difference of the 1st offline and (1st) online PV", 200, -1., 1.));
      // Tracks    
      addHistogram(new TH1F("nTrack_Split","Number of tracks", 40, 0., 40.));
      addHistogram(new TH1F("d0_Split","d0 of tracks", 200, -2., 2.));
      addHistogram(new TH1F("z0_Split","z0 of tracks", 200, -200., 200.));
      addHistogram(new TH1F("ed0_Split","err_d0 of tracks", 200, 0., 1.));
      addHistogram(new TH1F("ez0_Split","err_z0 of tracks", 200, 0., 5.));
      addHistogram(new TH1F("diffz0PV0_Split","z0 of tracks wrt 1st offline PV", 200, -10., 10.));
      addHistogram(new TH1F("sigz0PV_Split","z0-PV/errz0", 200, -20., 20.));
      addHistogram(new TH1F("trkPt_Split","Pt of tracks", 100, 0., 50.));
      addHistogram(new TH2F("trkEtaPhi_Split","Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
      //   Jets
      addHistogram(new TH1F("nJet_Split","Number of jets", 40, 0., 40.));
      addHistogram(new TH1F("jetPt_Split","Pt of jets", 100, 0., 250.));
      addHistogram(new TH2F("jetEtaPhi_Split","Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
      //   B-jets 
      addHistogram(new TH1F("IP3D_pu_tr_Split","IP3D_pu probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pb_tr_Split","IP3D_pb probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pc_tr_Split","IP3D_pc probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("wIP3D_Rbu_tr_Split","LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wSV1_Rbu_tr_Split","LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wCOMB_Rbu_tr_Split","LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wMV2c00_tr_Split","MV2c00 discriminant", 200, -1., 1.));
      addHistogram(new TH1F("wMV2c10_tr_Split","MV2c10 discriminant", 200, -1., 1.));
      addHistogram(new TH1F("wMV2c20_tr_Split","MV2c20 discriminant", 200, -1., 1.));
      addHistogram(new TH1F("xMVtx_tr_Split","SV1 mass - all SV1", 50, 0., 10.));
      addHistogram(new TH1F("xEVtx_tr_Split","SV1 E-fraction - all SV1", 50, 0., 1.));
      addHistogram(new TH1F("xNVtx_tr_Split","Number of 2-track SV1 - all SV1", 40, 0., 40.));
      addHistogram(new TH1F("xMVtx_trv_Split","SV1 mass - valid SV1", 50, 0., 10.));
      addHistogram(new TH1F("xEVtx_trv_Split","SV1 E-fraction - valid SV1", 50, 0., 1.));
      addHistogram(new TH1F("xNVtx_trv_Split","Number of 2-track SV1 - valid SV1", 40, 0., 40.));
      //  Mu-Jets
      addHistogram(new TH1F("nMuon_Split","Number of muons", 20, 0., 20.));
      addHistogram(new TH1F("muonPt_Split","Pt of muons", 100, 0., 250.));
      addHistogram(new TH2F("muonEtaPhi_Split","Phi vs Eta of muons", 20, -5., 5., 20, -3.1416, 3.1416));
      addHistogram(new TH1F("DeltaRAll_Split","DeltaR between muon and any jet", 100, 0., 6.));
      addHistogram(new TH1F("DeltaZAll_Split","DeltaZ between muon and any jet", 100, 0., 10.));

      //    On-line UnSplitChains
      addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter/Online_UnSplitChains", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
      ATH_MSG_INFO("  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter/Online_UnSplitChains, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED );
      // PV
      addHistogram(new TH1F("nPV_tr","Number of online PV per event", 101, -1., 100.));
      addHistogram(new TH1F("PVx_tr","online (1st) xPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVy_tr","online (1st) yPV", 200, -1.0, 1.0));
      addHistogram(new TH1F("PVz_tr","online (1st) zPV", 200, -200., 200.));
      addHistogram(new TH1F("diffzPV0offPVon","z difference of the 1st offline and (1st) online PV", 200, -1., 1.));
      // Tracks    
      addHistogram(new TH1F("nTrack","Number of tracks", 40, 0., 40.));
      addHistogram(new TH1F("d0","d0 of tracks", 200, -2., 2.));
      addHistogram(new TH1F("z0","z0 of tracks", 200, -200., 200.));
      addHistogram(new TH1F("ed0","err_d0 of tracks", 200, 0., 1.));
      addHistogram(new TH1F("ez0","err_z0 of tracks", 200, 0., 5.));
      addHistogram(new TH1F("diffz0PV0","z0 of tracks wrt 1st offline PV", 200, -10., 10.));
      addHistogram(new TH1F("sigz0PV","z0-PV/errz0", 200, -20., 20.));
      addHistogram(new TH1F("trkPt","Pt of tracks", 100, 0., 50.));
      addHistogram(new TH2F("trkEtaPhi","Phi vs Eta of tracks", 20, -5., 5., 20, -3.1416, 3.1416));
      //   Jets
      addHistogram(new TH1F("nJet","Number of jets", 40, 0., 40.));
      addHistogram(new TH1F("jetPt","Pt of jets", 100, 0., 250.));
      addHistogram(new TH2F("jetEtaPhi","Phi vs Eta of jets", 20, -5., 5., 20, -3.1416, 3.1416));
      //   B-jets 
      addHistogram(new TH1F("IP3D_pu_tr","IP3D_pu probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pb_tr","IP3D_pb probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("IP3D_pc_tr","IP3D_pc probability distribution", 200, 0., 1.));
      addHistogram(new TH1F("wIP3D_Rbu_tr","LogLH IP3D_pb/IP3D_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wSV1_Rbu_tr","LogLH SV1_pb/SV1_pu probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wCOMB_Rbu_tr","LogLH IP3D+SV1 probability ratio distribution", 200, -4., 6.));
      addHistogram(new TH1F("wMV2c00_tr","MV2c00 discriminant", 200, -1., 1.));
      addHistogram(new TH1F("wMV2c10_tr","MV2c10 discriminant", 200, -1., 1.));
      addHistogram(new TH1F("wMV2c20_tr","MV2c20 discriminant", 200, -1., 1.));
      addHistogram(new TH1F("xMVtx_tr","SV1 mass - all SV1", 50, 0., 10.));
      addHistogram(new TH1F("xEVtx_tr","SV1 E-fraction - all SV1", 50, 0., 1.));
      addHistogram(new TH1F("xNVtx_tr","Number of 2-track SV1 - all SV1", 40, 0., 40.));
      addHistogram(new TH1F("xMVtx_trv","SV1 mass - valid SV1", 50, 0., 10.));
      addHistogram(new TH1F("xEVtx_trv","SV1 E-fraction - valid SV1", 50, 0., 1.));
      addHistogram(new TH1F("xNVtx_trv","Number of 2-track SV1 - valid SV1", 40, 0., 40.));
      //  Mu-Jets
      addHistogram(new TH1F("nMuon","Number of muons", 20, 0., 20.));
      addHistogram(new TH1F("muonPt","Pt of muons", 100, 0., 250.));
      addHistogram(new TH2F("muonEtaPhi","Phi vs Eta of muons", 20, -5., 5., 20, -3.1416, 3.1416));
      addHistogram(new TH1F("DeltaRAll","DeltaR between muon and any jet", 100, 0., 6.));
      addHistogram(new TH1F("DeltaZAll","DeltaZ between muon and any jet", 100, 0., 10.));



    } // if newRunFlag



    return StatusCode::SUCCESS;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  StatusCode HLTBjetMonTool::fill() {

    ATH_MSG_DEBUG("====> Entering HLTBjetMonTool::fill()" );

  
  ////////////////////////////////////
  //setCurrentMonGroup("HLT/BjetMon/Shifter");
  ////////////////////////////////////


  // Trigger (chain) name definition

  int size_TriggerChainBjet =m_TriggerChainBjet.size();
  int size_TriggerChainMujet =m_TriggerChainMujet.size();
  ATH_MSG_DEBUG( " Size of Bjet Trigger chain " << size_TriggerChainBjet );
  ATH_MSG_DEBUG( " Size of Mujet Trigger chain " << size_TriggerChainMujet );
  if ( (size_TriggerChainBjet <= 0) && (size_TriggerChainMujet <= 0) ) {
    return StatusCode::SUCCESS;
  }
  std::string chainName = "Not fired";
  std::vector<std::string> FiredChainNames;

  for (int i =0; i<size_TriggerChainBjet; i++){
    if (!getTDT()->isPassed(m_TriggerChainBjet.at(i))){
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << m_TriggerChainBjet.at(i) << " not fired." );
    } else {
      chainName = m_TriggerChainBjet.at(i);
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << chainName << " fired." );
      std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
      if ( not selectChains.empty() ) FiredChainNames.push_back(chainName); //SR
    } // else
  } //i

  for (int i =0; i<size_TriggerChainMujet; i++){
    if (!getTDT()->isPassed(m_TriggerChainMujet.at(i))){
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << m_TriggerChainMujet.at(i) << " not fired." );
    } else {
      chainName = m_TriggerChainMujet.at(i);
      ATH_MSG_DEBUG( " Trigger chain " << i << " " << chainName << " fired." );
      std::vector<std::string> selectChains  = m_trigDec->getListOfTriggers( chainName );
      if ( not selectChains.empty() ) FiredChainNames.push_back(chainName); //SR 
    } // else
  } // i

  if ( not  FiredChainNames.empty() ) ATH_MSG_DEBUG( " " << FiredChainNames.size() << " trigger chains fired: " );
  for (unsigned int i = 0; i< FiredChainNames.size(); i++) {
    ATH_MSG_DEBUG( FiredChainNames.at(i) );
  }

  if ( FiredChainNames.empty() ) {

    ATH_MSG_INFO(" ===> No trigger fired neither for TriggerChainBjet of size: " << size_TriggerChainBjet 
		 << " nor for TriggerChainMujet of size: " << size_TriggerChainMujet << " RETURN from HLTBjetMonTool::fill() ! " );
    return StatusCode::SUCCESS;
  } else {


    const xAOD::EventInfo* eventInfo = 0;
    CHECK( evtStore()->retrieve( eventInfo, "EventInfo") );
    bool MCflag = true;
    if ( !eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) MCflag = false;
    ATH_MSG_DEBUG( " MCflag: " << MCflag );

    // Get truth jets
    if (MCflag) {
      const xAOD::JetContainer* truthjets = 0;
      ATH_CHECK( evtStore()->retrieve(truthjets,"AntiKt4TruthJets") );
      ATH_MSG_DEBUG("RETRIEVED TRUTH JETS  - size: " << truthjets->size());
    } // MCflag

    std::string HistDir = "/Offline";
    std::string HistExt = "";
    // Get offline PV
    float offlinepvz(-1.e6);
    bool Eofflinepv(false);
    const xAOD::VertexContainer* offlinepv = 0;
    if ( evtStore()->contains<xAOD::VertexContainer>("PrimaryVertices") ) { 
      ATH_CHECK( evtStore()->retrieve(offlinepv, "PrimaryVertices") );
      ATH_MSG_DEBUG("RETRIEVED OFFLINE PV  - size: " << offlinepv->size());
      if ( offlinepv->size() ) {
	Eofflinepv = true;
	offlinepvz = offlinepv->front()->z();
	ATH_MSG_DEBUG(" 1st zPV a la Carlo: " << offlinepvz);
	hist("nPV"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(offlinepv->size());
	for (unsigned int j = 0; j<offlinepv->size(); j++){
	  hist("PVx"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepv))[j]->x());
	  hist("PVy"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepv))[j]->y());
	  hist("PVz"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(offlinepv))[j]->z());
	} // j
      } // if size
    } // evtStore

    // Get online combinations

    // Loop on trigger items
    for (unsigned int ichain = 0; ichain < FiredChainNames.size(); ichain++) {
      //  unsigned int ichain = 0;
      std::string trigItem = FiredChainNames.at(ichain);
      // Set container names (2016/03/03) see TrigBtegEmulation.cxx
      // Non split input chaines
      std::string m_jetKey = "";
      std::string m_priVtxKey = "EFHistoPrmVtx";
      std::string m_trackKey  = "";
      HistDir = "/Online_UnSplitChains";
      HistExt = "";
      // Split input chaines
      std::size_t found = trigItem.find("split");
      if (found!=std::string::npos) {
	m_jetKey = "SplitJet";
	m_priVtxKey = "xPrimVx";
	m_trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
	HistDir = "/Online_SplitChains";
	HistExt = "_Split";
      }
      ATH_MSG_DEBUG( " Trigger chain name: " << trigItem << " m_jetKey: " << m_jetKey << " m_priVtxKey: " << m_priVtxKey << " m_trackKey: " << m_trackKey ); 
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
	
	// Get online PV
	const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs = comb.get<xAOD::VertexContainer>(m_priVtxKey);
	ATH_MSG_DEBUG("RETRIEVED PV (H) -   size: " << onlinepvs.size());
	if ( not onlinepvs.empty() ) {   // SR
	  const xAOD::VertexContainer* onlinepv = onlinepvs[0].cptr();
	  ATH_MSG_DEBUG("                 -   nVert: " << onlinepv->size());
	  if( not onlinepv->empty()) {  // SR
            if ( (*(onlinepv))[0]->vertexType() == xAOD::VxType::VertexType:: PriVtx ) { // test that PriVtx is not dummy (JA)                                                                
	      hist("PVx_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(onlinepv))[0]->x());
	      hist("PVy_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(onlinepv))[0]->y());
	      hist("PVz_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(onlinepv))[0]->z());
	      m_zPrmVtx = (*(onlinepv))[0]->z();
	      if (Eofflinepv) hist("diffzPV0offPVon"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((*(onlinepv))[0]->z()-offlinepvz);  
	      ATH_MSG_DEBUG("         Online PV -   z[0]: " << (*(onlinepv))[0]->z());
	    } // if PV not dummy  
	    else {
	      ATH_MSG_DEBUG(" Online dummy PV - type: " << (*(onlinepv))[0]->vertexType() << " x[0]: " << (*(onlinepv))[0]->x() 
			   << " y[0]: " << (*(onlinepv))[0]->y() <<  " z[0]: " << (*(onlinepv))[0]->z() );
	      int dummyflag = -1;
	      hist("nPV_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(dummyflag);
	      continue; // if vertex is dummy skip reading out the other quntities for this trigger combination (EN)
	    } // else
	    hist("nPV_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(onlinepv->size());
	  }  // if onlinepv not empty
	} // if onlinepvs not empty

	// Get online jet
	const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>(m_jetKey);
	ATH_MSG_DEBUG("RETRIEVED JETS   -   size: " << onlinejets.size());
	if( not onlinejets.empty()) {  // SR
	  const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
	  ATH_MSG_DEBUG("                 -   nJet: " << onlinejet->size());
	  int nJet = onlinejet->size();
	  hist("nJet"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(nJet);
	  for(const auto* jet : *onlinejet) {
	    ATH_MSG_DEBUG("                 -   pt/eta/phi: " << (jet->pt())*1.e-3 << " / " << jet->eta() << " / " << jet->phi());
	    hist("jetPt"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((jet->pt())*1.e-3);
	    hist2("jetEtaPhi"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(jet->eta(),jet->phi());
	  } // for online jet
	}//onlinejets.size

	// Get online muon                                                                                                                                                     
	const std::vector< Trig::Feature<xAOD::MuonContainer> > onlinemuons = comb.get<xAOD::MuonContainer>();
	ATH_MSG_DEBUG("RETRIEVED MUONS   -   size: " << onlinemuons.size());
	if( not onlinemuons.empty()) {  // SR
	  const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	  ATH_MSG_DEBUG("                 -   nMuon: " << onlinemuon->size());
	  int nMuon = onlinemuon->size();
	  hist("nMuon"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(nMuon);
	  for(const auto* muon : *onlinemuon) {
	    ATH_MSG_DEBUG("                 -   pt/eta/phi: " << (muon->pt())*1.e-3 << " / " << muon->eta() << " / " << muon->phi());
	    hist("muonPt"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill((muon->pt())*1.e-3);
	    hist2("muonEtaPhi"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(muon->eta(),muon->phi());
	  } // for online muon                                                                                                                                                                    
	}//onlinemuons.size                                                                                                                                                                   
	
	// Loop over muons and jets to monitor muon-jets m_deltaZ and m_dR
	float muonEta=0, muonPhi=0, muonZ=0;
	float jetEta=0,  jetPhi=0, jetZ=0;
	float m_deltaEta=0, m_deltaPhi=0, m_deltaZ=0;
	double m_dR = 0.;
	if( not onlinemuons.empty()) { // SR
	  const xAOD::MuonContainer* onlinemuon = onlinemuons[0].cptr();
	  for(const auto* muon : *onlinemuon) {
	    //	  if(onlinejets.size()) {
	    if( not onlinejets.empty()) { // SR
	      const xAOD::Muon::MuonType muontype = muon->muonType();
	      if( muontype != xAOD::Muon::MuonType::Combined ) continue; // to correct coverity issue - see next commented line 
	      //	    if(!(muontype == xAOD::Muon::MuonType::Combined) ) continue;
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
		hist("DeltaZAll"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(m_deltaZ);
		hist("DeltaRAll"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(m_dR);
	      } // for online jet                                                                                                                                                     
	    }//onlinejets.size                                                                                                                                                                   
	  } // for online muon                                                                                                                                                                    
	}//onlinemuons.size                                                                                                                                                                   
      
	// Get online track particles
	const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>(m_trackKey);
	ATH_MSG_DEBUG("RETRIEVED TRACKS -   size: " << onlinetracks.size());
	if ( not onlinetracks.empty() ) { // SR
	  const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
	  ATH_MSG_DEBUG("                 -   nTrack: " << onlinetrack->size());
	  int nTrack = onlinetrack->size();
	  hist("nTrack"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(nTrack);
	  for(const auto* trk : *onlinetrack) {
	    ATH_MSG_DEBUG("     pT: " << (trk->pt())*1.e-3 << " Eta: " << trk->eta() << " Phi: " << trk->phi() << " d0: " << trk->d0() );
	    if (Eofflinepv) ATH_MSG_DEBUG(" z0 - zPVoffl: " << trk->z0()+trk->vz()-offlinepvz ); // John A
	    hist("d0"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(trk->d0());
	    hist("z0"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(trk->z0());
	    hist("ed0"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(Amg::error(trk->definingParametersCovMatrix(), 0));
	    hist("ez0"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(Amg::error(trk->definingParametersCovMatrix(), 1));
	    if (Eofflinepv) hist("diffz0PV0"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(trk->z0()+trk->vz()-offlinepvz); // John Alison
	    float errz0 = Amg::error(trk->definingParametersCovMatrix(), 1);
	    if ( errz0 >0. && Eofflinepv ) hist("sigz0PV"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill( (trk->z0()+trk->vz()-offlinepvz)/errz0 ); // John Alison
	    hist("trkPt"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill( (trk->pt())*1.e-3 );
	    hist2("trkEtaPhi"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(trk->eta(),trk->phi());
	  } // for online track particles
	} // onlinetracks.size

	// Get online bjet from xAOD BTaggingContainer
	const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
	ATH_MSG_DEBUG("RETRIEVED BJETS  from xAOD BTaggingContainer -   size: " << onlinebjets.size());
	if(not onlinebjets.empty()) { // SR
	  const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
	  ATH_MSG_DEBUG("                 -   nBjet: " << onlinebjet->size());
	  for(const auto* bjet : *onlinebjet) {
	    double wIP3D, wSV1, wCOMB, wMV2c00, wMV2c10, wMV2c20  = 0.; // discriminant variables
	    //	  double wMV1  = 0.;
	    float svp_efrc, svp_mass = -1.; int svp_n2t = -1; // SV1 variables
	    bjet->loglikelihoodratio("IP3D", wIP3D);
	    bjet->loglikelihoodratio("SV1", wSV1);
	    double SV1_loglikelihoodratioLZ = bjet->SV1_loglikelihoodratio();
	    wCOMB = wIP3D+wSV1;
	    wMV2c00 = bjet->auxdata<double>("MV2c00_discriminant");
	    wMV2c10 = bjet->auxdata<double>("MV2c10_discriminant");
	    wMV2c20 = bjet->auxdata<double>("MV2c20_discriminant");
	    //	  wMV1 = bjet->MV1_discriminant();
	    // Suggestion of LZ
	    bjet->variable<float>("SV1", "masssvx", svp_mass);
	    bjet->variable<float>("SV1", "efracsvx", svp_efrc);
	    bjet->variable<int>("SV1", "N2Tpair", svp_n2t);
	    ATH_MSG_DEBUG("                 -   Before SV1 check - MVTX / EVTX / NVTX: " << svp_mass << " / " << svp_efrc << " / " << svp_n2t ) ; 
	    hist("xNVtx_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(svp_n2t);
	    if ( svp_n2t > 0 ) {
	      hist("xMVtx_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill( svp_mass * 1.e-3 ); 
	      hist("xEVtx_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill( svp_efrc );
	    } // if svp_n2t
	    // end of suggestion of LZ 
	    ATH_MSG_DEBUG("                 -   IP3Dpu / IP3Dpb / IP3Dpc: " << bjet->IP3D_pu() << " / " << bjet->IP3D_pb() << " / " << bjet->IP3D_pc() );
	    hist("IP3D_pu_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(bjet->IP3D_pu());
	    hist("IP3D_pb_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(bjet->IP3D_pb());
	    hist("IP3D_pc_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(bjet->IP3D_pc());
	    ATH_MSG_DEBUG("                 -   IP3D / SV1 / IP3D+SV1: " << wIP3D << " / " << wSV1 << " / " << wCOMB );
	    ATH_MSG_DEBUG("                 -   SV1 LZ: " << SV1_loglikelihoodratioLZ );
	    ATH_MSG_DEBUG("                 -   MV2c00 / MV2c10 / MV2c20: " << wMV2c00 << " / " << wMV2c10 << " / " << wMV2c20);
	    hist("wIP3D_Rbu_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(wIP3D);
	    hist("wSV1_Rbu_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(wSV1);
	    hist("wCOMB_Rbu_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(wCOMB);
	    hist("wMV2c00_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(wMV2c00);
	    hist("wMV2c10_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(wMV2c10);
	    hist("wMV2c20_tr"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(wMV2c20);

	    // Get SV1 secondary vtx information, see:
	    // /PhysicsAnalysis/JetTagging/JetTagTools/src/MV2Tag.cxx#0486 and 
	    // /PhysicsAnalysis/JetTagging/JetTagTools/src/GaiaNNTool.cxx#0349
	    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
	    ATH_MSG_DEBUG("    SV1 info source name before calling VertexContainer: " << m_sv1_infosource ) ;
	    bjet->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices);
	    ATH_MSG_DEBUG("    SV1 info source name after calling VertexContainer: " << m_sv1_infosource ) ;
	    if ( myVertices.size() > 0 && myVertices[0].isValid() ) {
	      ATH_MSG_DEBUG("    SV1 vertex size: " << myVertices.size() << " is it valid? " << myVertices[0].isValid() ) ;
	      bjet->variable<float>(m_sv1_infosource, "masssvx", svp_mass);
	      bjet->variable<float>(m_sv1_infosource, "efracsvx", svp_efrc);
	      bjet->variable<int>(m_sv1_infosource, "N2Tpair", svp_n2t);
	      ATH_MSG_DEBUG("                 -   MVTX / EVTX / NVTX: " << svp_mass << " / " << svp_efrc << " / " << svp_n2t ) ; 	    
	      hist("xNVtx_trv"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill(svp_n2t);
	      if ( svp_n2t > 0 ) {
		hist("xMVtx_trv"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill( svp_mass ); 
		hist("xEVtx_trv"+HistExt,"HLT/BjetMon/Shifter"+HistDir)->Fill( svp_efrc );
	      } // if svp_n2t 
	    } else {
	      ATH_MSG_DEBUG("  No valid SV1 vertex found --  SV1 vertex size: " << myVertices.size() );
	      if ( myVertices.size() > 0 ) ATH_MSG_DEBUG("  No valid SV1 vertex found -- myVertices[0].isValid(): " << myVertices[0].isValid() ) ;
	    } // if vertex valid
	  } // for online bjet
	} // onlinebjets.size
      } // for bjetComb
    } // ichain

  } // FiredChainNames.empty()

  ATH_MSG_DEBUG("====> Ended successfully HLTBjetMonTool::fill()" );


  
  return StatusCode::SUCCESS;

}





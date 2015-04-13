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
  m_likelihood_EF(0),
  m_probability_EF(0),
  m_tuningLikelihoodIP1D_EF(0),
  m_tuningLikelihoodIP2D_EF(0),
  m_tuningLikelihoodIP3D_EF(0),
  m_constLikelihood_EF(0),
  m_xBeamSpot(0),     
  m_yBeamSpot(0),
  m_prmVtxEFBj(0)
{
  declareProperty ("Taggers",            m_taggers);
  //  declareProperty ("TriggerChainBjet",       m_TriggerChainBjet);
  declareProperty ("monitoring_bjet",       m_TriggerChainBjet);           // change requested by Trigger Management 15/02/04
  //  declareProperty ("TriggerChainMujet",      m_TriggerChainMujet);
  declareProperty ("monitoring_mujet",      m_TriggerChainMujet);          // change requested by Trigger Management 15/02/04 
  declareProperty ("TriggerChainMujet_phys",      m_TriggerChainMujet_phys);  // this will be probably eliminated later

  declareProperty ("Menu_ppV0",           m_Menu_ppV0);

  declareProperty ("par_EF0",            m_par_EF0);
  declareProperty ("par_EF1",            m_par_EF1);

  declareProperty ("SizeIP1D_EF",        m_sizeIP1D_EF);
  declareProperty ("bIP1D_EF",           m_bIP1D_EF);
  declareProperty ("uIP1D_EF",           m_uIP1D_EF);
  declareProperty ("SizeIP2D_EF",        m_sizeIP2D_EF);
  declareProperty ("bIP2D_EF",           m_bIP2D_EF);
  declareProperty ("uIP2D_EF",           m_uIP2D_EF);
  declareProperty ("SizeIP3D_EF",        m_sizeIP3D_EF);
  declareProperty ("bIP3D_EF",           m_bIP3D_EF);
  declareProperty ("uIP3D_EF",           m_uIP3D_EF);

  declareProperty ("UseErrIPParam",      m_useErrIPParam      = false);
  declareProperty ("JetDirection",       m_jetDirection       = true);
  declareProperty ("UseEtaPhiTrackSel",  m_useEtaPhiTrackSel  = false);

  declareProperty ("SetBeamSpotWidth",   m_setBeamSpotWidth   = 0.05);

  declareProperty ("EFTrkSel_Chi2",      m_efTrkSelChi2       = 0.0);
  declareProperty ("EFTrkSel_BLayer",    m_efTrkSelBLayer     = 1);
  declareProperty ("EFTrkSel_PixHits",   m_efTrkSelPixHits    = 2);
  declareProperty ("EFTrkSel_SiHits",    m_efTrkSelSiHits     = 7);
  declareProperty ("EFTrkSel_D0",        m_efTrkSelD0         = 1*CLHEP::mm);
  declareProperty ("EFTrkSel_Z0",        m_efTrkSelZ0         = 2*CLHEP::mm);
  declareProperty ("EFTrkSel_Pt",        m_efTrkSelPt         = 1*CLHEP::GeV);


}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::~HLTBjetMonTool() {

  if (m_likelihood_EF)     delete m_likelihood_EF;
  if (m_probability_EF)    delete m_probability_EF;
  if (m_tuningLikelihoodIP1D_EF) delete m_tuningLikelihoodIP1D_EF;
  if (m_tuningLikelihoodIP2D_EF) delete m_tuningLikelihoodIP2D_EF;
  if (m_tuningLikelihoodIP3D_EF) delete m_tuningLikelihoodIP3D_EF;
 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::init() {

  m_likelihood_EF = new BjetLikelihoodMon(this, m_log);

  m_probability_EF = new BjetProbabilityMon(this, m_log);


  m_tuningLikelihoodIP1D_EF = new TuningLikelihood(&m_sizeIP1D_EF[0], &m_bIP1D_EF[0], &m_uIP1D_EF[0], m_sizeIP1D_EF.size());
  m_likelihood_EF->fillLikelihoodMap("EF_IP1D", m_tuningLikelihoodIP1D_EF);

  m_tuningLikelihoodIP2D_EF = new TuningLikelihood(&m_sizeIP2D_EF[0], &m_bIP2D_EF[0], &m_uIP2D_EF[0], m_sizeIP2D_EF.size());   
  m_likelihood_EF->fillLikelihoodMap("EF_IP2D", m_tuningLikelihoodIP2D_EF);
    
  m_tuningLikelihoodIP3D_EF = new TuningLikelihood(&m_sizeIP3D_EF[0], &m_bIP3D_EF[0], &m_uIP3D_EF[0], m_sizeIP3D_EF.size()); 
  m_likelihood_EF->fillLikelihoodMap("EF_IP3D", m_tuningLikelihoodIP3D_EF);

  //  m_tuningLikelihoodCOMB_EF = new TuningLikelihood(&m_sizeCOMB_EF[0], &m_bCOMB_EF[0], &m_uCOMB_EF[0], m_sizeCOMB_EF.size()); 
  // m_likelihood_EF->fillLikelihoodMap("EF_COMB", m_tuningLikelihoodCOMB_EF);

  m_constLikelihood_EF  = const_cast<const BjetLikelihoodMon*>(m_likelihood_EF);
  m_constProbability_EF = const_cast<const BjetProbabilityMon*>(m_probability_EF);


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


// Added by EN

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::proc(){
#else
  StatusCode HLTBjetMonTool::proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun){  
#endif
  
    *m_log << MSG::VERBOSE << "in HLTBjetMonTool::proc" << endreq;

  return StatusCode::SUCCESS;

  }

  //EN

//StatusCode HLTBjetMonTool::book( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun) { //EN

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::book(){
#else
  StatusCode HLTBjetMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun){  
#endif

  
    *m_log << MSG::VERBOSE<< "HLTBjetMonTool::book" << endreq;
    *m_log << MSG::INFO<< " entered HLTBjetMonTool::book" << endreq;

    addMonGroup(new MonGroup(this,"HLT/BjetMon", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
 
    *m_log << MSG::INFO<< "in HLTBjetMonTool::book added directory HLT/BjetMon, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED << endreq;

  int size_ChainBjet = m_TriggerChainBjet.size();
  if (size_ChainBjet <= 0) {
    *m_log << MSG::INFO << "  ===> in HLTBjetMonTool: TriggerChainBjet has no size! Return " << endreq;
    return StatusCode::FAILURE;
  }
  std::string chainName;
  chainName = m_TriggerChainBjet.at(0);

  *m_log << MSG::INFO<< "in HLTBjetMonTool::book chainName used: " << chainName << endreq;

  *m_log << MSG::INFO<< "in HLTBjetMonTool::book newRun: " << newRun << endreq;
  

  if(newRun){

    addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN

    *m_log << MSG::INFO<< "  in HLTBjetMonTool::book added directory HLT/BjetMon/Shifter, run: " << run << " " << ManagedMonitorToolBase::ATTRIB_MANAGED << endreq;

    addHistogram(new TH1F("xAOD_Counters","xAOD Event and trigger counters", 10, 0., 10.));
    addHistogram(new TH1F("xAOD_XIP2D_pu","IP2D_pu probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP2D_pu_tr","Triggered IP2D_pu probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP2D_pb","IP2D_pb probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP2D_pb_tr","Triggered IP2D_pb probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP2D_pc","IP2D_pc probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP2D_pc_tr","Triggered IP2D_pc probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP2D_Rbu","IP2D_pb/IP2D_pu probability ratio distribution from xAOD", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XIP2D_Rbu_tr","Triggered IP2D_pb/IP2D_pu probability ratio distribution from xAOD", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XIP2D_Rbu_tr_C","Triggered IP2D_pb/IP2D_pu probability ratio distribution from xAOD a la Carlo", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XIP3D_pu","IP3D_pu probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP3D_pu_tr","Triggered IP3D_pu probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP3D_pb","IP3D_pb probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP3D_pb_tr","Triggered IP3D_pb probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP3D_pc","IP3D_pc probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP3D_pc_tr","Triggered IP3D_pc probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XIP3D_Rbu","IP3D_pb/IP3D_pu probability ratio distribution from xAOD", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XIP3D_Rbu_tr","Triggered IP3D_pb/IP3D_pu probability ratio distribution from xAOD", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XIP3D_Rbu_tr_C","Triggered IP3D_pb/IP3D_pu probability ratio distribution from xAOD a la Carlo", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XSV1_pu","SV1_pu probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XSV1_pu_tr","Triggered SV1_pu probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XSV1_pb","SV1_pb probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XSV1_pb_tr","Triggered SV1_pb probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XSV1_pc","SV1_pc probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XSV1_pc_tr","Triggered SV1_pc probability distribution from xAOD", 200, -0.5, 1.5));
    addHistogram(new TH1F("xAOD_XSV1_Rbu","SV1_pb/SV1_pu probability ratio distribution from xAOD", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XSV1_Rbu_tr","Triggered SV1_pb/SV1_pu probability ratio distribution from xAOD", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_XSV1_Rbu_tr_C","Triggered SV1_pb/SV1_pu probability ratio distribution from xAOD a la Carlo", 200, 0., 10.));
    addHistogram(new TH1F("xAOD_nTrack","Number of tracks from xAOD", 20, 0., 20.));
    addHistogram(new TH1F("xAOD_nTrack_C","Number of tracks from xAOD a la Carlo", 20, 0., 20.));
    addHistogram(new TH1F("xAOD_d0","d0 of tracks from xAOD", 200, -2., 2.));
    addHistogram(new TH1F("xAOD_d0_C","d0 of tracks from xAOD a la Carlo", 200, -2., 2.));
    addHistogram(new TH1F("xAOD_z0","z0 of tracks from xAOD", 200, -100., 100.));
    addHistogram(new TH1F("xAOD_z0_C","z0 of tracks from xAOD a la Carlo", 200, -100., 100.));
    addHistogram(new TH1F("xAOD_ed0","err_d0 of tracks from xAOD", 200, 0., 1.));
    addHistogram(new TH1F("xAOD_ed0_C","err_d0 of tracks from xAOD a la Carlo", 200, 0., 1.));
    addHistogram(new TH1F("xAOD_ez0","err_z0 of tracks from xAOD", 200, 0., 5.));
    addHistogram(new TH1F("xAOD_ez0_C","err_z0 of tracks from xAOD a la Carlo", 200, 0., 5.));
    addHistogram(new TH1F("xAOD_diffz0PV","z0 of tracks wrt PV closest in z from xAOD", 200, -10., 10.));
    addHistogram(new TH1F("xAOD_diffz0PV0","z0 of tracks wrt 1st offline PV from xAOD", 200, -10., 10.));
    addHistogram(new TH1F("xAOD_diffz0PV0_C","z0 of tracks wrt 1st offline PV from xAOD a la Carlo", 200, -10., 10.));
    addHistogram(new TH1F("xAOD_diffzPV0offPVon","z difference of the 1st offline and triggered online PV", 200, -1., 1.));
    addHistogram(new TH1F("xAOD_diffzPV0offPVon_C","z difference of the 1st offline and triggered online PV a la Carlo", 200, -1., 1.));
    addHistogram(new TH1F("xAOD_sigz0PV","z0-PV/errz0 from xAOD", 200, -20., 20.));
    addHistogram(new TH1F("xAOD_sigz0PV_C","z0-PV/errz0 from xAOD a la Carlo", 200, -20., 20.));
    addHistogram(new TH1F("xAOD_nPV","Number of offline PV per event from xAOD", 20, 0., 20.));
    addHistogram(new TH1F("xAOD_nPV_C","Number of offline PV per event from xAOD a la Carlo", 20, 0., 20.));
    addHistogram(new TH1F("Closest_zPV","PV closest in z to triggered track", 20, -2., 18.));
    addHistogram(new TH1F("xAOD_PVz","offline PV_z from xAOD", 200, -100., 100.));
    addHistogram(new TH1F("xAOD_PVz_C","offline PV_z from xAOD a la Carlo", 200, -100., 100.));
    addHistogram(new TH1F("xAOD_PVz_tr","online PV_z passing the trigger from xAOD", 200, -100., 100.));
    addHistogram(new TH1F("xAOD_PVz_tr_C","online PV_z passing the trigger from xAOD a la Carlo", 200, -100., 100.));
    addHistogram(new TH1F("xAOD_PVx","offline PV_x from xAOD", 200, -1.0, 1.0));
    addHistogram(new TH1F("xAOD_PVx_C","offline PV_x from xAOD a la Carlo", 200, -1.0, 1.0));
    addHistogram(new TH1F("xAOD_PVy","offline PV_y from xAOD", 200, -1.0, 1.0));
    addHistogram(new TH1F("xAOD_PVy_C","offline PV_y from xAOD a la Carlo", 200, -1.0, 1.0));

  } // if newRun



  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::fill() {

  *m_log << MSG::INFO<< "====> Entering HLTBjetMonTool::fill()" << endreq;

  //* Clear RoI-based monitoring *//
  m_listCutAppliedLikelihood.clear();
  m_listCutAppliedLikelihood_EF.clear(); 
  m_listCutAppliedProbability.clear();
  m_listCutAppliedProbability_EF.clear();

  //* Clear track-based monitoring *//

  m_mon_ef_trk_a0.clear();
  m_mon_ef_trk_a0_sel.clear();
  m_mon_ef_trk_Sa0_sel.clear();
  m_mon_ef_trk_z0.clear();
  m_mon_ef_trk_z0_sel.clear();
  m_mon_ef_trk_z0_sel_PV.clear();
  m_mon_ef_trk_Sz0_sel.clear();
  m_mon_ef_trk_prob.clear();

  
  ////////////////////////////////////
  //setCurrentMonGroup("HLT/BjetMon/Shifter");
  ////////////////////////////////////


  // Trigger (chain) name definition

  int size_TriggerChainBjet =m_TriggerChainBjet.size();
  
  std::string chainName;
  if (size_TriggerChainBjet > 0) chainName = m_TriggerChainBjet.at(0);

  for (int i =0; i<size_TriggerChainBjet; i++){
    if (!getTDT()->isPassed(m_TriggerChainBjet.at(i))){
      *m_log << MSG::INFO << " Trigger chain " << i << " " << m_TriggerChainBjet.at(i) << " not fired." << endreq;
      //      return StatusCode::SUCCESS;
    }else {
      chainName = m_TriggerChainBjet.at(i);
      *m_log << MSG::INFO << " Trigger chain " << i << " " << chainName << " fired." << endreq;
    }
  }



#define OLDSTYLE

#ifdef OLDSTYLE

  //////////////////////////////////////////                                                                                                                                                       
  //* Retrieve xAOD b-jet object *//                                                                                                                                                               
  //////////////////////////////////////////                                                                                                   




  int nCounter = 0;

  hist("xAOD_Counters","HLT/BjetMon/Shifter")->Fill(nCounter);

  bool BtagCount(true), BtagTrCount(true), PVCount(true), PVTrCount(true), TrackTrCount(true);

  StatusCode sc = StatusCode::FAILURE;
  //  StatusCode sc;



  const DataHandle<xAOD::BTaggingContainer> fBTag, lastBTag;

  sc = m_storeGate->retrieve(fBTag, lastBTag);


  if( sc.isFailure() ){
    *m_log << MSG::INFO << "Failed to retrieve BTaggingContainer" << endreq;
    //    return sc;
  } else {
    *m_log << MSG::INFO << "Retrieve BTaggingContainer" << endreq;

    for(int j = 1; fBTag != lastBTag; ++fBTag, ++j) {
      *m_log << MSG::INFO << "Looking at BTaggingContainer " << j << endreq;
      xAOD::BTaggingContainer::const_iterator pBTagItr    = fBTag->begin();
      xAOD::BTaggingContainer::const_iterator lastBTagItr = fBTag->end();
      if (pBTagItr == lastBTagItr) *m_log << MSG::INFO << " BTaggingContainer is empty - no HLT/BjetMon/Shifter/xAOD histos filled" << endreq;
      
      *m_log << MSG::VERBOSE << "Retrieve ==> All B-jets" << endreq;
      for (int k=1; pBTagItr != lastBTagItr; ++pBTagItr, ++k) {
        *m_log << MSG::VERBOSE << "Looking at Btag Container " << k << "/" << fBTag->size() << endreq;
        if ((*pBTagItr)) {
	  if (BtagCount) {
	    nCounter = 1;
	    hist("xAOD_Counters","HLT/BjetMon/Shifter")->Fill(nCounter);
	    BtagCount = false;
	  } // if
	  *m_log << MSG::VERBOSE << "Now fill xAOD histos " << endreq;
	  *m_log << MSG::VERBOSE << " IP2D_pu " << (*pBTagItr)->IP2D_pu() << " IP2D_pb " << (*pBTagItr)->IP2D_pb() << " IP2D_pc " << (*pBTagItr)->IP2D_pc() << endreq;
	  *m_log << MSG::VERBOSE << " IP3D_pu " << (*pBTagItr)->IP3D_pu() << " IP3D_pb " << (*pBTagItr)->IP3D_pb() << " IP3D_pc " << (*pBTagItr)->IP3D_pc() << endreq;
	  *m_log << MSG::VERBOSE << " SV1_pu " << (*pBTagItr)->SV1_pu() << " SV1_pb " << (*pBTagItr)->SV1_pb() << " SV1_pc " << (*pBTagItr)->SV1_pc() << endreq;
	  hist("xAOD_XIP2D_pu","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->IP2D_pu());
	  hist("xAOD_XIP2D_pb","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->IP2D_pb());
	  hist("xAOD_XIP2D_pc","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->IP2D_pc());
          hist("xAOD_XIP3D_pu","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->IP3D_pu());
          hist("xAOD_XIP3D_pb","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->IP3D_pb());
          hist("xAOD_XIP3D_pc","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->IP3D_pc());
          hist("xAOD_XSV1_pu","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->SV1_pu());
          hist("xAOD_XSV1_pb","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->SV1_pb());
          hist("xAOD_XSV1_pc","HLT/BjetMon/Shifter")->Fill((*pBTagItr)->SV1_pc());
	  if ( (*pBTagItr)->IP2D_pu() > 0.) hist("xAOD_XIP2D_Rbu","HLT/BjetMon/Shifter")->Fill( (*pBTagItr)->IP2D_pb()/(*pBTagItr)->IP2D_pu() );
	  if ( (*pBTagItr)->IP3D_pu() > 0.) hist("xAOD_XIP3D_Rbu","HLT/BjetMon/Shifter")->Fill( (*pBTagItr)->IP3D_pb()/(*pBTagItr)->IP3D_pu() );
	  if ( (*pBTagItr)->SV1_pu() > 0.)  hist("xAOD_XSV1_Rbu","HLT/BjetMon/Shifter")->Fill( (*pBTagItr)->SV1_pb()/(*pBTagItr)->SV1_pu() );
	  // Printing B-jet quantities
	  *m_log << MSG::VERBOSE << "  " << k << "th jet in BTagging contener - IP2D_pu:  " << (*pBTagItr)->IP2D_pu() << endreq;
	} // if
      } // k
    } // j
           
  } // else sc. 




  *m_log << MSG::INFO << "Retrieve ==> Triggered B-jets" << endreq;
  std::vector<Trig::Feature<xAOD::BTagging> > trigHLTBjetVector =
    (getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<xAOD::BTagging>();
  std::vector<Trig::Feature<xAOD::BTagging> >::const_iterator pHLTBjetItr = trigHLTBjetVector.begin();
  std::vector<Trig::Feature<xAOD::BTagging> >::const_iterator lastHLTBjetItr = trigHLTBjetVector.end();
  for(int k = 1; pHLTBjetItr != lastHLTBjetItr; ++pHLTBjetItr, ++k) {
    const xAOD::BTagging* TrBjet = (*pHLTBjetItr).cptr();
    if ((TrBjet)) {
      if (BtagTrCount) {
	nCounter = 2;
	hist("xAOD_Counters","HLT/BjetMon/Shifter")->Fill(nCounter);
	BtagTrCount = false;
      } // if                                                                                                                                                                                  
      
      hist("xAOD_XIP2D_pu_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->IP2D_pu());
      hist("xAOD_XIP2D_pb_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->IP2D_pb());
      hist("xAOD_XIP2D_pc_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->IP2D_pc());
      
      hist("xAOD_XIP3D_pu_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->IP3D_pu());
      hist("xAOD_XIP3D_pb_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->IP3D_pb());
      hist("xAOD_XIP3D_pc_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->IP3D_pc());
      
      hist("xAOD_XSV1_pu_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->SV1_pu());
      hist("xAOD_XSV1_pb_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->SV1_pb());
      hist("xAOD_XSV1_pc_tr","HLT/BjetMon/Shifter")->Fill((TrBjet)->SV1_pc());
      
      if ( (TrBjet)->IP2D_pu() > 0.) hist("xAOD_XIP2D_Rbu_tr","HLT/BjetMon/Shifter")->Fill( (TrBjet)->IP2D_pb()/(TrBjet)->IP2D_pu() );
      if ( (TrBjet)->IP3D_pu() > 0.) hist("xAOD_XIP3D_Rbu_tr","HLT/BjetMon/Shifter")->Fill( (TrBjet)->IP3D_pb()/(TrBjet)->IP3D_pu() );
      if ( (TrBjet)->SV1_pu() > 0.) hist("xAOD_XSV1_Rbu_tr","HLT/BjetMon/Shifter")->Fill( (TrBjet)->SV1_pb()/(TrBjet)->SV1_pu() );
      
      // Printing triggered B-jet quantities
      *m_log << MSG::INFO << "  " << k << "th jet triggered -  IP2D_pu:  " << (TrBjet)->IP2D_pu() << endreq;
    }  // if
  } // k
  
      

    // } // else sc.



  ////////////*******************//////////////                                                                                                                                              

  //********** Retrieve xAOD vertices ***********//                                                                                                                                          

  ////////////*******************//////////////                                                                                                                                     
  std::vector<float> zPV;         
  std::vector<int> kPV;

  *m_log << MSG::INFO << " Retrieve xAOD vertices " << endreq;

  std::string vertex_container_name = "PrimaryVertices";
  const xAOD::VertexContainer* vertex = 0;
  sc = m_storeGate->retrieve( vertex, vertex_container_name);
  //  evtStore()->retrieve( vertex, vertex_container_name);
  if( sc.isFailure() ){
    *m_log << MSG::INFO << " Failed to retrieve xAOD vertices " << endreq;
    //    return sc;
  } else {
    xAOD::VertexContainer::const_iterator vtx_itr = vertex->begin();
    xAOD::VertexContainer::const_iterator vtx_end = vertex->end();

    unsigned int nPV = vertex->size();
    hist("xAOD_nPV","HLT/BjetMon/Shifter")->Fill(nPV);

    if (PVCount) {
      nCounter = 3;
      hist("xAOD_Counters","HLT/BjetMon/Shifter")->Fill(nCounter);
      PVCount = false;
    } // if                                                                                                                                                                                  

    for(int j = 1; vtx_itr != vtx_end; ++vtx_itr, ++j) {
      float zv = (*vtx_itr)->z();
      zPV.push_back(zv);
      kPV.push_back(j);
      float xv = (*vtx_itr)->x();
      float yv = (*vtx_itr)->y();
      *m_log << MSG::INFO << " Primary vtx retrieved " << j << " z: " << zv << endreq;
      hist("xAOD_PVz","HLT/BjetMon/Shifter")->Fill(zv);
      hist("xAOD_PVx","HLT/BjetMon/Shifter")->Fill(xv);
      hist("xAOD_PVy","HLT/BjetMon/Shifter")->Fill(yv);
    } // j



    *m_log << MSG::INFO << " Retrieved xAOD vertices " << endreq;

    *m_log << MSG::INFO << " Size of zPV " << zPV.size() << endreq;
    for (unsigned int j = 0; j<zPV.size(); j++){
      *m_log << MSG::INFO << " Primary vtx stored " << j+1 << " z: " << zPV[j] << endreq;
    } // j
  } // else sc.

  
  *m_log << MSG::INFO << " Retrieve online triggered xAOD PV's a la Run 1" << endreq;

  std::vector<Trig::Feature<xAOD::VertexContainer> > OLHLTPVVector = 
    (getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<xAOD::VertexContainer>();
  unsigned int nOLPV  = OLHLTPVVector.size();
  *m_log << MSG::INFO << " Number of OL PV's: " << nOLPV << endreq;
  for (unsigned int j = 0; j < nOLPV; j++) {
    *m_log << MSG::INFO << " pointerToVertexCollection " << OLHLTPVVector[j].cptr() << endreq;
    if (OLHLTPVVector[j].cptr()!=0){
      unsigned int nContPV = OLHLTPVVector[j].cptr()->size();
      *m_log << MSG::INFO << " Container size " << nContPV << " for vertex "<< j << endreq;
      for (unsigned int k = 0; k < nContPV; k++) {
      float zOLPV = (*(OLHLTPVVector[j].cptr()))[k]->z();  
      *m_log << MSG::INFO << " Vertex: " << j << " container: " << k << " zPV: " << zOLPV << endreq;
      if (zOLPV != 0) {
	if (PVTrCount) {
	  nCounter = 4;
	  hist("xAOD_Counters","HLT/BjetMon/Shifter")->Fill(nCounter);
	  PVTrCount = false;
	} // if                                                                                                                                                                                       
	hist("xAOD_PVz_tr","HLT/BjetMon/Shifter")->Fill(zOLPV);
        if (zPV.size()) hist("xAOD_diffzPV0offPVon","HLT/BjetMon/Shifter")->Fill(zPV[0]-zOLPV);
      } // if
      } // k    
    } // if
  } // j

  ////////////*******************//////////////                                                                                                                                                         
  //********** Retrieve xAOD tracks ***********//                                                                                                                                                         
  ////////////*******************//////////////                                                                                                                                                         

  *m_log << MSG::INFO << " Retrieve xAOD tracks" << endreq;
  const xAOD::TrackParticleContainer* pointerToHLTTrackCollection = 0;
  std::vector<Trig::Feature<xAOD::TrackParticleContainer> > trigHLTTrackVector =
    (getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<xAOD::TrackParticleContainer>();
  std::vector<Trig::Feature<xAOD::TrackParticleContainer> >::const_iterator pHLTTrackItr = trigHLTTrackVector.begin();
  std::vector<Trig::Feature<xAOD::TrackParticleContainer> >::const_iterator lastHLTTrackItr = trigHLTTrackVector.end();
  for(int j = 1; pHLTTrackItr != lastHLTTrackItr; ++pHLTTrackItr, ++j) {
    const xAOD::TrackParticleContainer* pHLTTrack = (*pHLTTrackItr).cptr();
    if (pHLTTrack->size() != 0) {
      *m_log << MSG::INFO << " Retrieved HLT tracks " <<endreq;
      pointerToHLTTrackCollection = pHLTTrack;
    } //if    
    if (pointerToHLTTrackCollection) {
      *m_log << MSG::INFO << " pointerToHLTTrackCollection " << pointerToHLTTrackCollection << endreq;
      unsigned int nTracks = pointerToHLTTrackCollection->size();
      hist("xAOD_nTrack","HLT/BjetMon/Shifter")->Fill(nTracks);
      for (unsigned int jj = 0; jj < nTracks; jj++) {
	const xAOD::TrackParticle* track = (*pointerToHLTTrackCollection)[jj];
	float d0t, z0t, errd0t, errz0t, diffz0PV, sigz0PV = -77777.;
	d0t = track->d0();
	z0t = track->z0();
	if (TrackTrCount) {
	  nCounter = 5;
	  hist("xAOD_Counters","HLT/BjetMon/Shifter")->Fill(nCounter);
	  TrackTrCount = false;
	} // if                                                        
	errz0t = Amg::error(track->definingParametersCovMatrix(), 1 );
	errd0t = Amg::error(track->definingParametersCovMatrix(), 0 ); 
	*m_log << MSG::INFO << "HLT track " << jj << " d0t " << d0t <<  " z0t " << z0t << " errd0t " << errd0t << " errz0t " << errz0t << endreq;
	hist("xAOD_d0","HLT/BjetMon/Shifter")->Fill(d0t);
	hist("xAOD_z0","HLT/BjetMon/Shifter")->Fill(z0t);
	hist("xAOD_ed0","HLT/BjetMon/Shifter")->Fill(errd0t);
	hist("xAOD_ez0","HLT/BjetMon/Shifter")->Fill(errz0t);

	if (zPV.size()>0) {
	  // Find the primary vertex closest in zPV
	  float distzMin = 1.e9;
	  unsigned int kmin = 0;
	  for (unsigned int k = 0; k<zPV.size(); k++){
	    float distz = fabs(zPV[k]-z0t);
	    if (distz < distzMin-0.001) {
	      distzMin = distz;
	      kmin = k;
	    } // if
	  } // k                                           

	  diffz0PV = z0t - zPV[kmin];
	  sigz0PV = 0; 
	  if (errz0t > 0.) {
	    sigz0PV = (z0t-zPV[0])/errz0t;
	    hist("xAOD_sigz0PV","HLT/BjetMon/Shifter")->Fill(sigz0PV);
	  } //if                                                                                                                           

	  hist("xAOD_diffz0PV","HLT/BjetMon/Shifter")->Fill(diffz0PV);
	  hist("xAOD_diffz0PV0","HLT/BjetMon/Shifter")->Fill(z0t-zPV[0]);

	  *m_log << MSG::INFO << "Closest vertex number " << " kmin " << kmin <<  " kPV[kmin] " << kPV[kmin] << endreq;
	  hist("Closest_zPV","HLT/BjetMon/Shifter")->Fill(kmin);
	  if ( (fabs(zPV[0]-zPV[zPV.size()-1]) < 0.001) && (zPV.size()>1) ) hist("Closest_zPV","HLT/BjetMon/Shifter")->Fill(-2.);
	} // zPV.size
      } //jj
    } else { 
      *m_log << MSG::INFO << " pointerToHLTTrackCollection not set " << endreq;
    } // if
  } // j


#else

  /////////////////////////////////////////
  //
  // Carlo's method
  //
  /////////////////////////////////////////

  std::string trigItem = chainName;
  ATH_MSG_INFO("PROCESSING TRIGITEM  -  " << trigItem);

  // Get truth jets
  const xAOD::JetContainer* truthjets = 0;
  ATH_CHECK( evtStore()->retrieve(truthjets,"AntiKt4TruthJets") );
  ATH_MSG_INFO("RETRIEVED TRUTH JETS  - size: " << truthjets->size());

  // Get offline PV
  const xAOD::VertexContainer* offlinepv = 0;
  ATH_CHECK( evtStore()->retrieve(offlinepv, "PrimaryVertices") );
  ATH_MSG_INFO("RETRIEVED OFFLINE PV  - size: " << offlinepv->size());
  float offlinepvz = offlinepv->front()->z();
  ATH_MSG_INFO(" 1st zPV a la Carlo: " << offlinepvz);
  for (unsigned int j = 0; j<offlinepv->size(); j++){
    hist("xAOD_PVx_C","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->x());
    hist("xAOD_PVy_C","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->y());
    hist("xAOD_PVz_C","HLT/BjetMon/Shifter")->Fill((*(offlinepv))[j]->z());
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

    // Define jet quantities
    //    int jet_label = 1001;
    //    float jet_eta=0, jet_phi=0;

#define HISTO
#ifdef HISTO
    // Get online pv - histo
    const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_histo = comb.get<xAOD::VertexContainer>("EFHistoPrmVtx");
    ATH_MSG_INFO("RETRIEVED PV (H) -   size: " << onlinepvs_histo.size());
    const xAOD::VertexContainer* onlinepv_histo = onlinepvs_histo[0].cptr();
    ATH_MSG_INFO("                 -   nVert: " << onlinepv_histo->size());
    hist("xAOD_nPV_C","HLT/BjetMon/Shifter")->Fill(onlinepv_histo->size());
    if(onlinepv_histo->size()) {
      hist("xAOD_PVz_tr_C","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->z());
      hist("xAOD_diffzPV0offPVon_C","HLT/BjetMon/Shifter")->Fill((*(onlinepv_histo))[0]->z()-offlinepvz);
      ATH_MSG_INFO("         Online PV - histo   -   z[0]: " << (*(onlinepv_histo))[0]->z());
    }  // if

#else
    // Get online pv - id tracking
    const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_id = comb.get<xAOD::VertexContainer>("xPrimVx");
    ATH_MSG_INFO("RETRIEVED PV (I) -   size: " << onlinepvs_id.size());
    const xAOD::VertexContainer* onlinepv_id = onlinepvs_id[0].cptr();
    ATH_MSG_INFO("                 -   nVert: " << onlinepv_id->size());
    hist("xAOD_nPV_C","HLT/BjetMon/Shifter")->Fill(onlinepv_id->size());
    if(onlinepv_id->size()) {
      hist("xAOD_PVz_tr_C","HLT/BjetMon/Shifter")->Fill((*(onlinepv_id))[0]->z());
      hist("xAOD_diffzPV0offPVon_C","HLT/BjetMon/Shifter")->Fill((*(onlinepv_id))[0]->z()-offlinepvz);
      ATH_MSG_INFO("          Online PV - id tracking   -   z[0]: " << (*(onlinepv_id))[0]->z());
    } // if
#endif


    // Get online jet
    const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets = comb.get<xAOD::JetContainer>();
    ATH_MSG_INFO("RETRIEVED JETS   -   size: " << onlinejets.size());
    const xAOD::JetContainer* onlinejet = onlinejets[0].cptr();
    ATH_MSG_INFO("                 -   nJet: " << onlinejet->size());
    for(const auto* jet : *onlinejet) {
      ATH_MSG_INFO("                 -   eta/phi: " << jet->eta() << " / " << jet->phi());
    } // for online jet



    // Get online track particles
    const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>();
    ATH_MSG_INFO("RETRIEVED TRACKS -   size: " << onlinetracks.size());
    const xAOD::TrackParticleContainer* onlinetrack = onlinetracks[0].cptr();
    ATH_MSG_INFO("                 -   nTrack: " << onlinetrack->size());
    int nTrack = onlinetrack->size();
    hist("xAOD_nTrack_C","HLT/BjetMon/Shifter")->Fill(nTrack);
    for(const auto* trk : *onlinetrack) {
      ATH_MSG_INFO("     pT: " << trk->pt() << " d0: " << trk->d0() << " z0 - zPVoffl: " << trk->z0()-offlinepvz );
      hist("xAOD_d0_C","HLT/BjetMon/Shifter")->Fill(trk->d0());
      hist("xAOD_z0_C","HLT/BjetMon/Shifter")->Fill(trk->z0());
      hist("xAOD_diffz0PV0_C","HLT/BjetMon/Shifter")->Fill(trk->z0()-offlinepvz);
      hist("xAOD_ed0_C","HLT/BjetMon/Shifter")->Fill(Amg::error(trk->definingParametersCovMatrix(), 0));
      hist("xAOD_ez0_C","HLT/BjetMon/Shifter")->Fill(Amg::error(trk->definingParametersCovMatrix(), 1));
      float errz0 = Amg::error(trk->definingParametersCovMatrix(), 1);
      if (errz0 >0.) hist("xAOD_sigz0PV_C","HLT/BjetMon/Shifter")->Fill( (trk->z0()-offlinepvz)/errz0 );
    } // for online track particles

   
    // Get online bjet
    const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
    ATH_MSG_INFO("RETRIEVED BJETS  -   size: " << onlinebjets.size());
    const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
    ATH_MSG_INFO("                 -   nBjet: " << onlinebjet->size());
    for(const auto* bjet : *onlinebjet) {
      double wIP2D, wIP3D, wSV1, wCOMB, wMV1 = 0.;
      double RIP2D, RIP3D, RSV1;
      bjet->loglikelihoodratio("IP2D", wIP2D);
      bjet->loglikelihoodratio("IP3D", wIP3D);
      bjet->loglikelihoodratio("SV1", wSV1);
      wCOMB = wIP3D+wSV1;
      wMV1 = bjet->MV1_discriminant();
      ATH_MSG_INFO("                 -   IP2D/IP3D/SV1/IP3D+SV1/MV1: " << wIP2D << " / " << wIP3D << " / " << wSV1 << " / " << wCOMB << " / " << wMV1);
      if (wIP2D > 10.) wIP2D = 10.;
      RIP2D = exp(wIP2D);
      hist("xAOD_XIP2D_Rbu_tr_C","HLT/BjetMon/Shifter")->Fill(RIP2D);
      if (wIP3D > 10.) wIP3D = 10.;
      RIP3D = exp(wIP3D);
      hist("xAOD_XIP3D_Rbu_tr_C","HLT/BjetMon/Shifter")->Fill(RIP3D);
      if (wSV1 > 10.) wSV1 = 10.;
      RSV1 = exp(wSV1);
      hist("xAOD_XSV1_Rbu_tr_C","HLT/BjetMon/Shifter")->Fill(RSV1);
    } // for online bjet
   

  } // for bjetComb

#endif

  *m_log << MSG::INFO<< "====> Ended successfully HLTBjetMonTool::fill()" << endreq;

  
  return StatusCode::SUCCESS;
  //  return sc;
}





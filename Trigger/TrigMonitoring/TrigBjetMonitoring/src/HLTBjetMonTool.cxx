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

#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticle/TrigL2BjetContainer.h"
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



#include "TrigBjetMonitoring/HLTBjetMonTool.h"

#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "GaudiKernel/ListItem.h"

namespace Analysis { class MuonContainer; }



//** ----------------------------------------------------------------------------------------------------------------- **//


HLTBjetMonTool::HLTBjetMonTool(const std::string & type, const std::string & name, const IInterface* parent) :
  IHLTMonTool(type, name, parent),
  m_trackJetFinderTool("TrigTrackJetFinderTool"),
  m_likelihood_L2(0),
  m_likelihood_EF(0),
  m_probability_L2(0),
  m_probability_EF(0),
  m_tuningLikelihoodIP1D_L2(0),
  m_tuningLikelihoodIP2D_L2(0),
  m_tuningLikelihoodIP3D_L2(0),
  m_tuningLikelihoodIP1D_EF(0),
  m_tuningLikelihoodIP2D_EF(0),
  m_tuningLikelihoodIP3D_EF(0),
  m_constLikelihood_L2(0),
  m_constLikelihood_EF(0),
  m_xBeamSpot(0),     
  m_yBeamSpot(0),
  m_prmVtxL2Bj(0),
  m_prmVtxEFBj(0)
{
  declareProperty ("Taggers",            m_taggers);
  declareProperty ("TriggerChainBjet",       m_TriggerChainBjet);
  declareProperty ("TriggerChainL2Bjet",       m_TriggerChainL2Bjet);
  declareProperty ("TriggerChainMujet",      m_TriggerChainMujet);
  declareProperty ("TriggerChainMujet_phys",      m_TriggerChainMujet_phys);

  declareProperty ("Menu_ppV0",           m_Menu_ppV0);

  declareProperty ("par_L2Si0",          m_par_L2Si0);
  declareProperty ("par_L2Si1",          m_par_L2Si1);
  declareProperty ("par_L2Id0",          m_par_L2Id0);
  declareProperty ("par_L2Id1",          m_par_L2Id1);
  declareProperty ("par_EF0",            m_par_EF0);
  declareProperty ("par_EF1",            m_par_EF1);

  declareProperty ("SizeIP1D_L2",        m_sizeIP1D_L2);
  declareProperty ("bIP1D_L2",           m_bIP1D_L2);
  declareProperty ("uIP1D_L2",           m_uIP1D_L2);
  declareProperty ("SizeIP2D_L2",        m_sizeIP2D_L2);
  declareProperty ("bIP2D_L2",           m_bIP2D_L2);
  declareProperty ("uIP2D_L2",           m_uIP2D_L2);
  declareProperty ("SizeIP3D_L2",        m_sizeIP3D_L2);
  declareProperty ("bIP3D_L2",           m_bIP3D_L2);
  declareProperty ("uIP3D_L2",           m_uIP3D_L2);
  
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
  declareProperty ("L2PrmVtxAtEF",       m_l2PrmVtxAtEF       = true);
  declareProperty ("UseEtaPhiTrackSel",  m_useEtaPhiTrackSel  = false);

  declareProperty ("SetBeamSpotWidth",   m_setBeamSpotWidth   = 0.05);

  declareProperty ("L2TrkSel_Chi2",      m_l2TrkSelChi2       = 0.001);
  declareProperty ("L2TrkSel_BLayer",    m_l2TrkSelBLayer     = 1);
  declareProperty ("L2TrkSel_SiHits",    m_l2TrkSelSiHits     = 4);
  declareProperty ("L2TrkSel_D0",        m_l2TrkSelD0         = 1*CLHEP::mm);
  declareProperty ("L2TrkSel_Z0",        m_l2TrkSelZ0         = 2*CLHEP::mm);
  declareProperty ("L2TrkSel_Pt",        m_l2TrkSelPt         = 2*CLHEP::GeV);
	
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

  if (m_likelihood_L2)     delete m_likelihood_L2;
  if (m_likelihood_EF)     delete m_likelihood_EF;
  if (m_probability_L2)    delete m_probability_L2;
  if (m_probability_EF)    delete m_probability_EF;
  if (m_tuningLikelihoodIP1D_L2) delete m_tuningLikelihoodIP1D_L2;
  if (m_tuningLikelihoodIP2D_L2) delete m_tuningLikelihoodIP2D_L2;
  if (m_tuningLikelihoodIP3D_L2) delete m_tuningLikelihoodIP3D_L2;
  if (m_tuningLikelihoodIP1D_EF) delete m_tuningLikelihoodIP1D_EF;
  if (m_tuningLikelihoodIP2D_EF) delete m_tuningLikelihoodIP2D_EF;
  if (m_tuningLikelihoodIP3D_EF) delete m_tuningLikelihoodIP3D_EF;
 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::init() {

  m_likelihood_L2 = new BjetLikelihoodMon(this, m_log);
  m_likelihood_EF = new BjetLikelihoodMon(this, m_log);

  m_probability_L2 = new BjetProbabilityMon(this, m_log);
  m_probability_EF = new BjetProbabilityMon(this, m_log);

  m_tuningLikelihoodIP1D_L2 = new TuningLikelihood(&m_sizeIP1D_L2[0], &m_bIP1D_L2[0], &m_uIP1D_L2[0], m_sizeIP1D_L2.size());
  m_likelihood_L2->fillLikelihoodMap("L2_IP1D", m_tuningLikelihoodIP1D_L2);

  m_tuningLikelihoodIP2D_L2 = new TuningLikelihood(&m_sizeIP2D_L2[0], &m_bIP2D_L2[0], &m_uIP2D_L2[0], m_sizeIP2D_L2.size());   
  m_likelihood_L2->fillLikelihoodMap("L2_IP2D", m_tuningLikelihoodIP2D_L2);

  m_tuningLikelihoodIP3D_L2 = new TuningLikelihood(&m_sizeIP3D_L2[0], &m_bIP3D_L2[0], &m_uIP3D_L2[0], m_sizeIP3D_L2.size()); 
  m_likelihood_L2->fillLikelihoodMap("L2_IP3D", m_tuningLikelihoodIP3D_L2);

  m_tuningLikelihoodIP1D_EF = new TuningLikelihood(&m_sizeIP1D_EF[0], &m_bIP1D_EF[0], &m_uIP1D_EF[0], m_sizeIP1D_EF.size());
  m_likelihood_EF->fillLikelihoodMap("EF_IP1D", m_tuningLikelihoodIP1D_EF);

  m_tuningLikelihoodIP2D_EF = new TuningLikelihood(&m_sizeIP2D_EF[0], &m_bIP2D_EF[0], &m_uIP2D_EF[0], m_sizeIP2D_EF.size());   
  m_likelihood_EF->fillLikelihoodMap("EF_IP2D", m_tuningLikelihoodIP2D_EF);
    
  m_tuningLikelihoodIP3D_EF = new TuningLikelihood(&m_sizeIP3D_EF[0], &m_bIP3D_EF[0], &m_uIP3D_EF[0], m_sizeIP3D_EF.size()); 
  m_likelihood_EF->fillLikelihoodMap("EF_IP3D", m_tuningLikelihoodIP3D_EF);

  //  m_tuningLikelihoodCOMB_EF = new TuningLikelihood(&m_sizeCOMB_EF[0], &m_bCOMB_EF[0], &m_uCOMB_EF[0], m_sizeCOMB_EF.size()); 
  // m_likelihood_EF->fillLikelihoodMap("EF_COMB", m_tuningLikelihoodCOMB_EF);

  m_constLikelihood_L2  = const_cast<const BjetLikelihoodMon*>(m_likelihood_L2);
  m_constLikelihood_EF  = const_cast<const BjetLikelihoodMon*>(m_likelihood_EF);
  m_constProbability_L2 = const_cast<const BjetProbabilityMon*>(m_probability_L2);
  m_constProbability_EF = const_cast<const BjetProbabilityMon*>(m_probability_EF);


  StatusCode sc = m_trackJetFinderTool.retrieve();
  if(sc.isFailure()) {
    *m_log << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
    return StatusCode::FAILURE;
  } else
    *m_log << MSG::VERBOSE << "Retrieved tool " << m_trackJetFinderTool << endreq;

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


// Added by EN

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTBjetMonTool::proc(){
#else
  StatusCode HLTBjetMonTool::proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun){  
#endif
  
    *m_log << MSG::VERBOSE<< "HLTBjetMonTool::proc" << endreq;

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

  addMonGroup(new MonGroup(this,"HLT/BjetMon", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
 
  // int size_ChainBjet =m_TriggerChainBjet.size();
  
  std::string chainName;
  std::string chainNameL2;
    
  if (m_Menu_ppV0==3){
    chainName = "EF_b10_IDTrkNoCut";//EF_b10_j30_a4tc_EFFS_IDTrkNoCut";
    chainNameL2 = "EF_b10_IDTrkNoCut";//EF_b10_j30_a4tc_EFFS_IDTrkNoCut";
  }   else  {
      chainName = "EF_b55_NoCut_j55_a4tchad";
      chainNameL2 = "L2_b50_NoCut_j50_c4cchad";}
  *m_log << MSG::DEBUG<< "BOOK IDTrkNoCut chain fired." << chainName<<endreq;
  
  if(newRun){

    addMonGroup(new MonGroup(this,"HLT/BjetMon/Shifter", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
    
    addHistogram(new TH1F("L2Bjet_XIP1D","IP1D tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F("L2Bjet_XIP2D","IP2D tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F("L2Bjet_XIP3D","IP3D tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F("L2Bjet_XComb","COMB tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F("L2Bjet_XCHI2","JetProb tagger distribution at LVL2", 100, -1, 1));
    addHistogram(new TH1F("L2Bjet_CounterXComb","Track Counter at LVL2 : XCOMB patological cases", 3, -1, 2));
    //addHistogram(new TH1F("L2Bjet_XSV","SV tagger distribution at LVL2", 2000, -50, 50));

    addHistogram(new TH1F("EFBjet_XIP1D","IP1D tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F("EFBjet_XIP2D","IP2D tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F("EFBjet_XIP3D","IP3D tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F("EFBjet_XComb","COMB tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F("EFBjet_XCHI2","JetProb tagger distribution at EF", 100, -1, 1));
    addHistogram(new TH1F("EFBjet_CounterXComb","Track Counter at EF : XCOMB patological cases", 3, -1, 2));

    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XCHI2").c_str()),"JetProb tagger distribution at LVL2 using IDTrkNoCut chain", 100, -1, 1));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XCHI2").c_str()),"JetProb tagger distribution at EF using IDTrkNoCut chain", 100, -1, 1));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XComb").c_str()),"IP3D+SV1 tagger distribution at LVL2 using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XComb").c_str()),"IP3D+SV1 tagger distribution at EF using IDTrkNoCut chain", 200, -10, 15));
   
    addMonGroup(new MonGroup(this,"HLT/BjetMon/mu-jet", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN

      addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(1)+" versus offline jet pT").c_str()),300,0,300000));
    //  addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(2)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(2)+" versus offline jet pT").c_str()),300,0,300000));
      // addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(3)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(3)+" versus offline jet pT").c_str()),300,0,300000));
      //  addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(4)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(4)+" versus offline jet pT").c_str()),300,0,300000));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(0)+" versus offline jet pT").c_str()),300,0,300000));
    //  addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(5)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(5)+" versus offline jet pT").c_str()),300,0,300000));
    //  addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(6)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(6)+" versus offline jet pT").c_str()),300,0,300000));
    //  addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(7)).c_str()),const_cast<char*>((m_TriggerChainMujet.at(7)+" versus offline jet pT").c_str()),300,0,300000));
    //  addHistogram(new TH1F("MuJet_ALL","All mu-jet trigger versus offline jet pT",300,0,300000));

    addHistogram(new TH1F("NumOffMuJet","Number of online mu-jets matched offline",10,0,10));
    addHistogram(new TH1F("MuJet_ALL","All mu-jet trigger versus offline jet pT",300,0,300000));

    ///************ COMB **********//
  
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XComb").c_str()),"Comb tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XIP3D").c_str()),"IP3D tagger distribution at L2", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XSV0").c_str()),"SV0 tagger distribution at L2", 200, -1, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XMVtx").c_str()),"MVtx distribution at L2", 200, -1, 5000));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XNVtx").c_str()),"NVtx distribution at L2", 21, -1, 20));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XEVtx").c_str()),"EVtx distribution at L2", 100, 0, 1.2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXComb").c_str()),"Track Counter at LVL2 : XComb patological cases", 3, -1, 2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXSV").c_str()),"Track Counter at LVL2 : XSV patological cases", 3, -1, 2));

    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XComb").c_str()),"Comb tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XIP3D").c_str()),"IP3D tagger distribution at L2", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XSV0").c_str()),"SV0 tagger distribution at L2", 200, -1, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XMVtx").c_str()),"MVtx distribution at L2", 200, -1, 5000));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XNVtx").c_str()),"NVtx distribution at L2", 21, -1, 20));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XEVtx").c_str()),"EVtx distribution at L2", 100, 0, 1.2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXComb").c_str()),"Track Counter  at LVL2 : XCOMB patological cases", 3, -1, 2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXSV").c_str()),"Track Counter at LVL2 : XSV patological cases", 3, -1, 2));
    
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XComb_pTrel2").c_str()),"Comb tagger distribution at LVL2", 100, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XComb_pTrel2").c_str()),"Comb tagger distribution at LVL2", 100, -10, 15));
    
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XComb").c_str()),"Comb tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XIP3D").c_str()),"IP3D tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XSV0").c_str()),"SV0 tagger distribution at EF", 200, -1, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XMVtx").c_str()),"MVtx distribution at EF", 200, -1, 5000));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XNVtx").c_str()),"NVtx distribution at EF", 21, -1, 20));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XEVtx").c_str()),"EVtx distribution at EF", 100, 0, 1.2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXComb").c_str()),"Track Counter at EF : XComb patological cases", 3, -1, 2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXSV").c_str()),"Track Counter at EF : XSV patological cases", 3, -1, 2));
    
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XComb").c_str()),"Comb tagger distribution at EF", 200, -10, 15));   
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XIP3D").c_str()),"IP3D tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XSV0").c_str()),"SV0 tagger distribution at EF", 200, -1, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XMVtx").c_str()),"MVtx distribution at EF", 200, -1, 5000));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XNVtx").c_str()),"NVtx distribution at EF", 21, -1, 20));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XEVtx").c_str()),"EVtx distribution at EF", 100, 0, 1.2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXComb").c_str()),"Track Counter at EF : XComb patological cases", 3, -1, 2));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXSV").c_str()),"Track Counter at EF : XSV patological cases", 3, -1, 2));

    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XComb_pTrel2").c_str()),"Comb tagger distribution at EF", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XComb_pTrel2").c_str()),"Comb tagger distribution at EF", 200, -10, 15));
    ///************ COMB **********//

    addHistogram(new TH1F("pTrel_muChains","pTrel",100,0,10));
    addHistogram(new TH1F("pTrel","pTrel",100,0,10));

    addMonGroup(new MonGroup(this,"HLT/BjetMon/b-jet", run, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN
    
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_a0").c_str()),"transverse IP for all tracks at LVL2", 200, -10, 10));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_a0_sel").c_str()),   "transverse IP for selected tracks at LVL2", 200, -10, 10));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_Sa0_sel").c_str()),  "transverse IP significance for selected tracks at LVL2", 200, -15, 15));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_z0").c_str()),       "longitudinal IP for all tracks at LVL2", 200, -400, 400));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_z0_sel").c_str()),   "longitudinal IP for selected tracks at LVL2", 200, -400, 400));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_z0_sel_PV").c_str()),"longitudinal IP wrt PV for selected tracks at LVL2", 200, -20, 20));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_Sz0_sel").c_str()),  "longitudinal IP significance for selected tracks at LVL2", 200, -20, 20));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_prob").c_str()),     "reconstructed track probability estimated by JetProb at LVL2", 40, 0, 1));

    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_a0").c_str()),       "transverse IP for all tracks at EF", 200, -10, 10));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_a0_sel").c_str()),   "transverse IP for selected tracks at EF", 200, -10, 10));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_Sa0_sel").c_str()),  "transverse IP significance for selected tracks at EF", 200, -15, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_z0").c_str()),       "longitudinal IP for all tracks at EF", 200, -400, 400));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_z0_sel").c_str()),   "longitudinal IP for selected tracks at EF", 200, -400, 400));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_z0_sel_PV").c_str()),"longitudinal IP wrt PV for selected tracks at EF", 200, -20, 20));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_Sz0_sel").c_str()),  "longitudinal IP significance for selected tracks at EF", 200, -20, 20));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_prob").c_str()),     "reconstructed track probability estimated by JetProb at EF", 40, 0, 1));

    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_stepsToSelect").c_str()), "Steps to select tracks for JetProb tagger", 11, 0, 11));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_stepsToSelect").c_str()), "Steps to select tracks for JetProb tagger", 11, 0, 11));

    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XIP1D").c_str()),"IP1D tagger distribution at LVL2 using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XIP2D").c_str()),"IP2D tagger distribution at LVL2 using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XIP3D").c_str()),"IP3D tagger distribution at LVL2 using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XSV0").c_str()),"SV0 tagger distribution at LVL2 using IDTrkNoCut chain", 200, -1, 15));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XMVtx").c_str()),"Mass distribution at LVL2 using IDTrkNoCut chain", 200, -1, 5000));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XNVtx").c_str()),"Number distribution at LVL2 using IDTrkNoCut chain", 21, -1, 20));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XEVtx").c_str()),"Energy  distribution at LVL2 using IDTrkNoCut chain", 100, 0, 1.2));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_CounterXComb").c_str()),"Track Counter at LVL2 using IDTrkNoCut chain : XComb patological cases", 3, -1, 2));
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_CounterXSV").c_str()),"Track Counter at LVL2 using IDTrkNoCut chain : XSV patological cases", 3, -1, 2));

    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XIP1D").c_str()),"IP1D tagger distribution at EF using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XIP2D").c_str()),"IP2D tagger distribution at EF using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XIP3D").c_str()),"IP3D tagger distribution at EF using IDTrkNoCut chain", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XSV0").c_str()),"SV0 tagger distribution at EF using IDTrkNoCut chain", 200, -1, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XMVtx").c_str()),"Mass distribution at EF using IDTrkNoCut chain", 200, -1, 5000));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XNVtx").c_str()),"Number distribution at EF using IDTrkNoCut chain", 21, -1, 20));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XEVtx").c_str()),"Energy  distribution at EF using IDTrkNoCut chain", 100, 0, 1.2));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_CounterXComb").c_str()),"Track Counter at EF using IDTrkNoCut chain : XComb patological cases", 3, -1, 2));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_CounterXSV").c_str()),"Track Counter at EF using IDTrkNoCut chain : XSV patological cases", 3, -1, 2));

    addHistogram(new TH1F("L2Bjet_XIP3D_Recomputed","IP3D tagger distribution at LVL2 recomputed starting from reconstructed tracks", 200, -50, 50));
    addHistogram(new TH1F("L2Bjet_XCHI2_Recomputed","JetProb tagger distribution at LVL2 recomputed starting from reconstructed tracks", 100, -1, 1));
    
    addHistogram(new TH1F("EFBjet_XIP3D_Recomputed","IP3D tagger distribution at EF recomputed starting from reconstructed tracks", 200, -50, 50));
    addHistogram(new TH1F("EFBjet_XCHI2_Recomputed","JetProb tagger distribution at EF recomputed starting from reconstructed tracks", 100, -1, 1));

  }

  if (newLowStatInterval) {

    addMonGroup(new MonGroup(this,"HLT/BjetMon/LowStat", ManagedMonitorToolBase::lowStat, ManagedMonitorToolBase::ATTRIB_MANAGED)); //EN

    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_trk_prob_LS").c_str()), "track probability estimated by JetProb at LVL2", 40, 0, 1));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_trk_prob_LS").c_str()), "track probability estimated by JetProb at EF", 40, 0, 1));

    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XCHI2_LS").c_str()),"JetProb tagger distribution at LVL2", 100, -1, 1));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XCHI2_LS").c_str()),"JetProb tagger distribution at EF", 100, -1, 1));
    
    addHistogram(new TH1F(const_cast<char*>(("L2Bjet_"+chainNameL2+"_XComb_LS").c_str()),"Comb tagger distribution at LVL2", 200, -10, 15));
    addHistogram(new TH1F(const_cast<char*>(("EFBjet_"+chainName+"_XComb_LS").c_str()),"Comb tagger distribution at EF", 200, -10, 15));


  }

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode HLTBjetMonTool::fill() {

  StatusCode sc = StatusCode::FAILURE;

  //* Clear RoI-based monitoring *//
  m_listCutAppliedLikelihood.clear();
  m_listCutAppliedLikelihood_L2.clear();
  m_listCutAppliedLikelihood_EF.clear(); 
  m_listCutAppliedProbability.clear();
  m_listCutAppliedProbability_L2.clear();
  m_listCutAppliedProbability_EF.clear();

  //* Clear track-based monitoring *//
  m_mon_l2_trk_a0.clear();
  m_mon_l2_trk_a0_sel.clear();
  m_mon_l2_trk_Sa0_sel.clear();
  m_mon_l2_trk_z0.clear();
  m_mon_l2_trk_z0_sel.clear();
  m_mon_l2_trk_z0_sel_PV.clear();
  m_mon_l2_trk_Sz0_sel.clear();
  m_mon_l2_trk_prob.clear();
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

  //////////////////////////////
  //* LVL2 b-tagging from SG *//
  //////////////////////////////

  const DataHandle<TrigL2BjetContainer> trigL2Bjet, lastTrigL2Bjet;
	
  sc = m_storeGate->retrieve(trigL2Bjet,lastTrigL2Bjet);
	
  if(sc.isFailure())
    *m_log << MSG::DEBUG<< "Failed to retrieve L2 Bjet container" << endreq;
  else {
    *m_log << MSG::DEBUG << "Retrieved L2 Bjet container" << endreq;

    for(int j = 1; trigL2Bjet != lastTrigL2Bjet; ++trigL2Bjet, ++j) {
      
      *m_log << MSG::VERBOSE << "Looking at TrigL2BjetContainer " << j << endreq;

      TrigL2BjetContainer::const_iterator pL2BjetItr    = trigL2Bjet->begin();
      TrigL2BjetContainer::const_iterator lastL2BjetItr = trigL2Bjet->end();

      for (int k=1; pL2BjetItr != lastL2BjetItr; ++pL2BjetItr, ++k) {

	*m_log << MSG::DEBUG << "Looking at TrigL2Bjet " << k << "/" << trigL2Bjet->size() << endreq;

	if ((*pL2BjetItr)) {

	 //  *m_log << MSG::DEBUG<< "TrigL2Bjet->xIP1D() = " << (*pL2BjetItr)->xIP1D() << "; TrigL2Bjet->xIP2D() = " << (*pL2BjetItr)->xIP2D()
// 		 << "; TrigL2Bjet->xIP3D() = " << (*pL2BjetItr)->xIP3D() << "; TrigL2Bjet->xCHI2() = " << (*pL2BjetItr)->xCHI2() 
// 		 << "; TrigL2Bjet->xComb() = " << (*pL2BjetItr)->xComb() 
// 		 << "; TrigL2Bjet->xMVtx() = " << (*pL2BjetItr)->xSV() 
// 		 << "; TrigL2Bjet->xMVtx() = " << (*pL2BjetItr)->xMVtx() 
// 		 << endreq;
	
	  if((*pL2BjetItr)->xComb()>-42){
	    hist("L2Bjet_XIP1D","HLT/BjetMon/Shifter")->Fill((*pL2BjetItr)->xIP1D());
	    hist("L2Bjet_XIP2D","HLT/BjetMon/Shifter")->Fill((*pL2BjetItr)->xIP2D());
	    hist("L2Bjet_XIP3D","HLT/BjetMon/Shifter")->Fill((*pL2BjetItr)->xIP3D());
	    hist("L2Bjet_XComb","HLT/BjetMon/Shifter")->Fill((*pL2BjetItr)->xComb());
	    hist("L2Bjet_CounterXComb","HLT/BjetMon/Shifter")->Fill(1);
	  }
	  else if ((*pL2BjetItr)->xComb()==-46)
	    hist("L2Bjet_CounterXComb","HLT/BjetMon/Shifter")->Fill(0);
	  else  hist("L2Bjet_CounterXComb","HLT/BjetMon/Shifter")->Fill(-1);
	  
	  hist("L2Bjet_XCHI2","HLT/BjetMon/Shifter")->Fill((*pL2BjetItr)->xCHI2());
	  // hist("L2Bjet_XSV","HLT/BjetMon/Shifter")->Fill((*pL2BjetItr)->xSV1());


	} // else {

// 	  *m_log << MSG::VERBOSE << "pL2BjetItr is NULL" << endreq;

// 	  hist("L2Bjet_XIP1D","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  hist("L2Bjet_XIP2D","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  hist("L2Bjet_XIP3D","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  hist("L2Bjet_XCHI2","HLT/BjetMon/Shifter")->Fill(-1.0);
// 	  hist("L2Bjet_XComb","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  //hist("L2Bjet_XSV","HLT/BjetMon/Shifter")->Fill(-50.0);

// 	}
      }
    } 
  }
  
  
  ////////////////////////////
  //* EF b-tagging from SG *//
  ////////////////////////////
  
  const DataHandle<TrigEFBjetContainer> trigEFBjet, lastTrigEFBjet;
  
  sc = m_storeGate->retrieve(trigEFBjet,lastTrigEFBjet);
  
  if( sc.isFailure() ){
    *m_log << MSG::VERBOSE << "Failed to retrieve EF Bjet container" << endreq;
  } else {
    *m_log << MSG::DEBUG << "Retrieved EF Bjet container" << endreq;
    
    for(int j = 1; trigEFBjet != lastTrigEFBjet; ++trigEFBjet, ++j) { 
      
      *m_log << MSG::VERBOSE << "Looking at TrigEFBjetContainer " << j << endreq;
      
      TrigEFBjetContainer::const_iterator pEFBjetItr    = trigEFBjet->begin();
      TrigEFBjetContainer::const_iterator lastEFBjetItr = trigEFBjet->end();
      
      for (int k=1; pEFBjetItr != lastEFBjetItr; ++pEFBjetItr, ++k) {
	
	*m_log << MSG::VERBOSE << "Looking at TrigEFBjet " << k << "/" << trigEFBjet->size() << endreq;
	
	if ((*pEFBjetItr)) {
	  
	 //  *m_log << MSG::DEBUG
// 		 << "TrigEFBjet->xIP1D() = "   << (*pEFBjetItr)->xIP1D() << "; TrigEFBjet->xIP2D() = " << (*pEFBjetItr)->xIP2D()
// 		 << "; TrigEFBjet->xIP3D() = " << (*pEFBjetItr)->xIP3D() << "; TrigEFBjet->xCHI2() = " << (*pEFBjetItr)->xCHI2() 
// 		 << "; TrigEFBjet->xMVtx() = " << (*pEFBjetItr)->xSV() 
// 		 << "; TrigEFBjet->xMVtx() = " << (*pEFBjetItr)->xMVtx() 
// 		 << endreq;
	  if((*pEFBjetItr)->xComb()>-42){
	    hist("EFBjet_XIP1D","HLT/BjetMon/Shifter")->Fill((*pEFBjetItr)->xIP1D());
	    hist("EFBjet_XIP2D","HLT/BjetMon/Shifter")->Fill((*pEFBjetItr)->xIP2D());
	    hist("EFBjet_XIP3D","HLT/BjetMon/Shifter")->Fill((*pEFBjetItr)->xIP3D());
	    hist("EFBjet_XComb","HLT/BjetMon/Shifter")->Fill((*pEFBjetItr)->xComb());
	    hist("EFBjet_CounterXComb","HLT/BjetMon/Shifter")->Fill(1);
	  }
	  else if ((*pEFBjetItr)->xComb()==-46)
	    hist("EFBjet_CounterXComb","HLT/BjetMon/Shifter")->Fill(0);
	  else hist("EFBjet_CounterXComb","HLT/BjetMon/Shifter")->Fill(-1);

	  hist("EFBjet_XCHI2","HLT/BjetMon/Shifter")->Fill((*pEFBjetItr)->xCHI2());


	} // else {

// 	  *m_log << MSG::VERBOSE << "pEFBjetItr is NULL" << endreq;

// 	  hist("EFBjet_XIP1D","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  hist("EFBjet_XIP2D","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  hist("EFBjet_XIP3D","HLT/BjetMon/Shifter")->Fill(-50.0);
// 	  hist("EFBjet_XCHI2","HLT/BjetMon/Shifter")->Fill(-1.0);
// 	  hist("EFBjet_XComb","HLT/BjetMon/Shifter")->Fill(-50.0);

// 	}
      }
    } 
  }

  
  /////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  ///*************setCurrentMonGroup("HLT/BjetMon/mu-jet")*************///
  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  
  
  ///////////****************///////////
  //******** mu-jet matched **********//
  ///////////****************///////////

 
  int  size_TriggerChainMujet =m_TriggerChainMujet_phys.size();

  *m_log << MSG::DEBUG << "HLT/BjetMon/mu-jet" << endreq;
  
  // get muons
  const Analysis::MuonContainer* muonColl = 0;
  StatusCode sc_muon = m_storeGate->retrieve(muonColl,"StacoMuonCollection");
  if(sc_muon.isFailure() && !muonColl)
    (*m_log) << MSG::DEBUG << "No StacoMuonCollection container found" << endreq;

  // get jets
  const JetCollection* jetColl;
  StatusCode sc_jet = m_storeGate->retrieve(jetColl,"AntiKt4TopoEMJets");
  if(sc_jet.isFailure() && !jetColl)
    *m_log << MSG::DEBUG << "No AntiKt4TopoEMJets container found" << endreq;
  
  if (sc_muon.isSuccess() && sc_jet.isSuccess()) {
    
    std::vector<float> ppjetVector;
    
    
    JetCollection::const_iterator jetItr = (*jetColl).begin();
    JetCollection::const_iterator jetEnd = (*jetColl).end();

    for (; jetItr != jetEnd; ++jetItr) {

      CLHEP::HepLorentzVector pjet((*jetItr)->px(),(*jetItr)->py(),(*jetItr)->pz(),(*jetItr)->e());
      TLorentzVector pjet_mu;

      //if (!jet::JetCaloQualityUtils::isGood(*jetItr)) continue; // REQUIRES xAOD migration
      if (false) continue; // REQUIRES xAOD migration
      
      //      if (fabs((*jetItr)->pt(P4SignalState::JETEMSCALE))>15000 && fabs((*jetItr)->eta())<2.5) {
      if (fabs(pjet.perp())>15000 && fabs((*jetItr)->eta())<2.5) {
	
	Analysis::MuonContainer::const_iterator muItr = (*muonColl).begin();
	Analysis::MuonContainer::const_iterator muEnd = (*muonColl).end();
	
	//	*m_log << MSG::DEBUG<<" jet pt "<< fabs((*jetItr)->pt(P4SignalState::JETEMSCALE)) <<
	//  " pjet.perp() "<< pjet.perp()<< endreq;
	TLorentzVector pmu1;
	for (; muItr != muEnd; ++muItr) {
	 	 
	  CLHEP::HepLorentzVector pmu((*muItr)->px(),(*muItr)->py(),(*muItr)->pz(),(*muItr)->e());
	  if (pmu.deltaR(pjet)<0.4 && pmu.perp()>4000){
	    ppjetVector.push_back(pjet.perp());
	    pjet_mu.SetPtEtaPhiE((*jetItr)->pt(),(*jetItr)->eta(),(*jetItr)->phi(),(*jetItr)->e());
	    
	    const Trk::Track* msatIP= (*muItr)->muonExtrapolatedTrkTrack();
	    double muID_z0=0;
	    double muID_theta=0;
	    long long int pattInfoInt = 0;
	    int patt=0;
	    if(msatIP){
	      muID_z0=msatIP->perigeeParameters()->parameters()[Trk::z0];
	      muID_theta=msatIP->perigeeParameters()->parameters()[Trk::theta];
	      *m_log << MSG::VERBOSE << "muTrk "<<msatIP->perigeeParameters()->parameters()[Trk::d0]<<endreq;
	      
	      
	      
	      const Trk::TrackInfo& trkInfo = msatIP->info(); 
	      for(unsigned int i=0; i<Trk::TrackInfo::NumberOfTrackRecoInfo;i++){
		const Trk::TrackInfo::TrackPatternRecoInfo preco = (Trk::TrackInfo::TrackPatternRecoInfo)i;
		long long int one = 1;
		if ((trkInfo.patternRecoInfo(preco) & 1)!=1){
		  pattInfoInt = (pattInfoInt | (one << i));
		  patt=1;
		  *m_log << MSG::VERBOSE << "muTrk pattInfoInt "<<pattInfoInt<<" "<<one<<"  "<<patt<<" preco "<<preco<<endreq;
		}
	      }
	    }
	    
	    double chi2overDof=1;
	    if(msatIP){
	      const Trk::FitQuality * idFitQual = msatIP->fitQuality();
	      
	      if(idFitQual){
		if(idFitQual->chiSquared() > 0. && idFitQual->numberDoF() > 0)
		  { 
		    if(idFitQual->numberDoF() > 0){
		      chi2overDof = idFitQual->chiSquared()/(double)idFitQual->numberDoF();
		      *m_log << MSG::DEBUG <<"idFitQual->chiSquared() "<<idFitQual->chiSquared()<<"  idFitQual->numberDoF() "<<idFitQual->numberDoF()<<endreq;
		    }
		  }
	      }
	      else chi2overDof=-1e12;
	    }
	    
	    if((*muItr)->author()!=6) continue;
	    if((*muItr)->eta()>2.5) continue;
	    if((*muItr)->pt()<4000.) continue;
	    if(muID_z0*sin(muID_theta)>2) continue;
	    if((*muItr)->numberOfPixelHits()<2) continue;
	    if((*muItr)->numberOfSCTHits()<4) continue;
	    if((*muItr)->numberOfTRTHits()<7) continue;
	    if(chi2overDof>3) continue;
	    	      	      
	    double ptrel=pmu.perp(pjet+pmu);
	    //	    (*m_log) << MSG::DEBUG <<"  pTrel : "<<ptrel<<endreq; 
	    
	    ptrel= ptrel/1000.;
	    hist("pTrel","HLT/BjetMon/mu-jet")->Fill(ptrel);
	    
	    for(int i=0;i< size_TriggerChainMujet;i++){
	      if (getTDT()->isPassed(m_TriggerChainMujet_phys.at(i)))
		hist("pTrel_muChains","HLT/BjetMon/mu-jet")->Fill(ptrel);
	    }
	    ///////////*************************///////////
	    /////////////********  L2 ********/////////////
	    ///////////*************************///////////
	  
	    const DataHandle<TrigL2BjetContainer> trigL2Bjet, lastTrigL2Bjet;
	    sc = m_storeGate->retrieve(trigL2Bjet,lastTrigL2Bjet);
	    if(sc.isFailure())
	      *m_log << MSG::VERBOSE << "Failed to retrieve L2 Bjet container" << endreq;
	    else {	  *m_log << MSG::VERBOSE << "Retrieved L2 Bjet container" << endreq;
	      for(int j = 1; trigL2Bjet != lastTrigL2Bjet; ++trigL2Bjet, ++j) {
		*m_log << MSG::VERBOSE << "Looking at TrigL2BjetContainer " << j <<endreq;
		TrigL2BjetContainer::const_iterator pL2BjetItr1    = trigL2Bjet->begin();
		TrigL2BjetContainer::const_iterator lastL2BjetItr1 = trigL2Bjet->end();
		
		for (int k=1; pL2BjetItr1 != lastL2BjetItr1; ++pL2BjetItr1, ++k) {
		    *m_log << MSG::VERBOSE << "Looking at TrigL2Bjet " << k << "/" << trigL2Bjet->size() << endreq;
		    
		    //		      *m_log << MSG::DEBUG << " p_L2 "<<(*pL2BjetItr1)->px()<<" "<<(*pL2BjetItr1)->py()<<" "<<(*pL2BjetItr1)->pz()<<" "<<(*pL2BjetItr1)->e()<<endreq;
		     
		      TLorentzVector pL2;
		      pL2.SetPtEtaPhiM(1, (*pL2BjetItr1)->eta(), (*pL2BjetItr1)->phi(), 5000);
		      
		      if(pL2.DeltaR(pjet_mu)<0.4){
			//  *m_log << MSG::DEBUG << "L2 Bjet DELAR OFF" << pL2.DeltaR(pjet_mu)<<endreq;

			if (getTDT()->isPassed(m_TriggerChainMujet.at(1))){
			  if((*pL2BjetItr1)->xComb()>-42){
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XComb","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xComb());
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XIP3D","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xIP3D());
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXComb","HLT/BjetMon/mu-jet")->Fill(1);}
			  else if ((*pL2BjetItr1)->xComb()==-46) 
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXComb","HLT/BjetMon/mu-jet")->Fill(0);
			  else  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXComb","HLT/BjetMon/mu-jet")->Fill(-1);

			  if((*pL2BjetItr1)->xSV()!=0){
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XSV0","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xSV());
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XMVtx","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xMVtx());
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XNVtx","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xNVtx());
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XEVtx","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xEVtx());
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXSV","HLT/BjetMon/mu-jet")->Fill(1);}
			  else hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_CounterXSV","HLT/BjetMon/mu-jet")->Fill(0);
			}
			
			if (getTDT()->isPassed(m_TriggerChainMujet.at(0))){
			  if((*pL2BjetItr1)->xComb()>-42){
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XComb","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xComb());
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XIP3D","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xIP3D());
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXComb","HLT/BjetMon/mu-jet")->Fill(1);
			  }
			  else if((*pL2BjetItr1)->xComb()==-46)
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXComb","HLT/BjetMon/mu-jet")->Fill(0);
			  else hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXComb","HLT/BjetMon/mu-jet")->Fill(-1);

			  if((*pL2BjetItr1)->xSV()!=0){
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XSV0","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xSV());
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XMVtx","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xMVtx());
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XNVtx","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xNVtx());
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XEVtx","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xEVtx());
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXSV","HLT/BjetMon/mu-jet")->Fill(1);
			  } 
			  else hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_CounterXSV","HLT/BjetMon/mu-jet")->Fill(0);


			}
			
					
			if(ptrel>2){
			  if((*pL2BjetItr1)->xComb()>-42){
			    if (getTDT()->isPassed(m_TriggerChainMujet.at(1)))
			      hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_L2_XComb_pTrel2","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xComb());
			    
			    if (getTDT()->isPassed(m_TriggerChainMujet.at(0)))
			      hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_L2_XComb_pTrel2","HLT/BjetMon/mu-jet")->Fill((*pL2BjetItr1)->xComb());
			  }
			}
		      }
		}
	      }
	    }
	  
	    
	    
	    ///////////*************************///////////
	    /////////////********  EF ********/////////////
	    ///////////*************************///////////

	    const DataHandle<TrigEFBjetContainer> trigEFBjet, lastTrigEFBjet;
	    sc = m_storeGate->retrieve(trigEFBjet,lastTrigEFBjet);
	    if(sc.isFailure())
	      *m_log << MSG::VERBOSE << "Failed to retrieve EF Bjet container" << endreq;
	    else {	  *m_log << MSG::VERBOSE << " Retrieved EF Bjet container" << endreq;
	      for(int j = 1; trigEFBjet != lastTrigEFBjet; ++trigEFBjet, ++j) {
		*m_log << MSG::VERBOSE << "Looking at TrigEFBjetContainer " << j << endreq;
		TrigEFBjetContainer::const_iterator pEFBjetItr1    = trigEFBjet->begin();
		TrigEFBjetContainer::const_iterator lastEFBjetItr1 = trigEFBjet->end();
		
		for (int k=1; pEFBjetItr1 != lastEFBjetItr1; ++pEFBjetItr1, ++k) {
		  *m_log << MSG::VERBOSE << "Looking at TrigEFBjet " << k << "/" << trigEFBjet->size() << endreq;
		  if ((*pEFBjetItr1)) {
		    //		    *m_log << MSG::DEBUG << " p_EF "<<(*pEFBjetItr1)->px()<<" "<<(*pEFBjetItr1)->py()<<" "<<(*pEFBjetItr1)->pz()<<" "<<(*pEFBjetItr1)->e()<<endreq;

		    TLorentzVector pEF;
		    pEF.SetPtEtaPhiM(1, (*pEFBjetItr1)->eta(), (*pEFBjetItr1)->phi(), 5000);
		    
		    if(pEF.DeltaR(pjet_mu)<0.4){
		      // *m_log << MSG::DEBUG << "EF Bjet DELAR OFF" << pEF.DeltaR(pjet_mu)<<endreq;
		      if (getTDT()->isPassed(m_TriggerChainMujet.at(1))){
			if((*pEFBjetItr1)->xComb()>-42){
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XComb","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xComb());
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XIP3D","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xIP3D());
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXComb","HLT/BjetMon/mu-jet")->Fill(1);
			}
			else if((*pEFBjetItr1)->xComb()==-46)
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXComb","HLT/BjetMon/mu-jet")->Fill(0);
			else  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXComb","HLT/BjetMon/mu-jet")->Fill(-1);

			if((*pEFBjetItr1)->xSV()!=0){
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XSV0","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xSV());
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XMVtx","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xMVtx());
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XNVtx","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xNVtx());
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XEVtx","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xEVtx());
			  hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXSV","HLT/BjetMon/mu-jet")->Fill(1);
			} else hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_CounterXSV","HLT/BjetMon/mu-jet")->Fill(0);
		      }

		      if (getTDT()->isPassed(m_TriggerChainMujet.at(0))){
			if((*pEFBjetItr1)->xComb()>-42){
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XComb","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xComb());
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XIP3D","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xIP3D());
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXComb","HLT/BjetMon/mu-jet")->Fill(1);
			}
			else if((*pEFBjetItr1)->xComb()==-46)  
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXComb","HLT/BjetMon/mu-jet")->Fill(0);
			else  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXComb","HLT/BjetMon/mu-jet")->Fill(-1);
			
			if((*pEFBjetItr1)->xSV()!=0){
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XSV0","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xSV());
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XMVtx","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xMVtx());
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XNVtx","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xNVtx());
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XEVtx","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xEVtx());
			  hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXSV","HLT/BjetMon/mu-jet")->Fill(1);
			} else hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_CounterXSV","HLT/BjetMon/mu-jet")->Fill(0);
		      }

		      if(ptrel>2){
			if((*pEFBjetItr1)->xComb()>-42){
			  if (getTDT()->isPassed(m_TriggerChainMujet.at(1)))
			    hist("MuJet_"+m_TriggerChainMujet.at(1)+"_MatchOffmu_EF_XComb_pTrel2","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xComb());
			  if (getTDT()->isPassed(m_TriggerChainMujet.at(0)))
			    hist("MuJet_"+m_TriggerChainMujet.at(0)+"_MatchOffmu_EF_XComb_pTrel2","HLT/BjetMon/mu-jet")->Fill((*pEFBjetItr1)->xComb());
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  
  
    if (ppjetVector.size()==0)
      ppjetVector.push_back(-1);
    
    hist("NumOffMuJet","HLT/BjetMon/mu-jet")->Fill(ppjetVector.size());
    
    for (unsigned int i=0;i<ppjetVector.size();i++) {
     
      
      if (getTDT()->isPassed(m_TriggerChainMujet.at(0)))
	hist("MuJet_"+m_TriggerChainMujet.at(0),"HLT/BjetMon/mu-jet")->Fill(ppjetVector[i]);

      if (getTDT()->isPassed(m_TriggerChainMujet.at(1)))
	hist("MuJet_"+m_TriggerChainMujet.at(1),"HLT/BjetMon/mu-jet")->Fill(ppjetVector[i]);
         
      for(int i=0;i< size_TriggerChainMujet;i++){
	if (getTDT()->isPassed(m_TriggerChainMujet_phys.at(i)))
	  hist("MuJet_ALL","HLT/BjetMon/mu-jet")->Fill(ppjetVector[i]);
      }
    }
  }

  ///////////****************///////////
  //******** mu-jet matched **********//
  ///////////****************///////////



  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  ///*************setCurrentMonGroup("HLT/BjetMon/b-jet");*************///
  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  
  *m_log << MSG::VERBOSE << "HLT/BjetMon/b-jet Trigger passing" << endreq;
  int size_TriggerChainBjet =m_TriggerChainBjet.size();
  
  std::string chainName;
  std::string chainNameL2;

  for (int i =0; i<size_TriggerChainBjet; i++){
    if (!getTDT()->isPassed(m_TriggerChainBjet.at(i))){
      *m_log << MSG::DEBUG<< "IDTrkNoCut chain not fired." << endreq;
      // return StatusCode::SUCCESS;
    }else {
      *m_log << MSG::DEBUG << "IDTrkNoCut chain fired." << endreq;
      chainName = m_TriggerChainBjet.at(i);
      chainNameL2 = m_TriggerChainL2Bjet.at(i);
    }
  }
  
  if (m_Menu_ppV0==3){
    chainName = "EF_b10_IDTrkNoCut";
    chainNameL2 = "EF_b10_IDTrkNoCut";
  }
  ////////////******************//////////////
  //********** Retrieve beam spot **********//
  ////////////******************//////////////

  IBeamCondSvc* m_iBeamCondSvc; 
  sc = service("BeamCondSvc", m_iBeamCondSvc);
 
  if (sc.isFailure() || m_iBeamCondSvc == 0) {
    m_iBeamCondSvc = 0;
 
    *m_log << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
    *m_log << MSG::WARNING << "Using origin at ( " << m_xBeamSpot << " , " << m_yBeamSpot << " ) " << endreq;
 
  } else {
 
    Amg::Vector3D m_beamSpot = m_iBeamCondSvc->beamPos();
      
    m_xBeamSpot = m_beamSpot.x();
    m_yBeamSpot = m_beamSpot.y();
    m_zBeamSpot = m_beamSpot.z();
 
    m_sigmaX = m_iBeamCondSvc->beamSigma(0);
    m_sigmaY = m_iBeamCondSvc->beamSigma(1);
    m_sigmaZ = m_iBeamCondSvc->beamSigma(2);

    *m_log << MSG::DEBUG << " beam spot X " << m_sigmaX << " beam spot Y " << m_sigmaY << " beam spot Z " << m_sigmaZ<< endreq;
  }

  ////////////******************//////////////
  //********* Retrieve LVL2 tracks *********//
  ////////////******************//////////////
  
  *m_log << MSG::DEBUG << " Retrieve LVL2 tracks" << endreq;

  const TrigInDetTrackCollection* pointerToL2TrackCollection = 0;

  //  std::vector<Trig::Feature<TrigInDetTrackCollection> > trigL2TrackVector = 
  //(getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<TrigInDetTrackCollection>();
  std::vector<Trig::Feature<TrigInDetTrackCollection> > trigL2TrackVector = 
    (getTDT()->features(chainNameL2,TrigDefs::alsoDeactivateTEs)).get<TrigInDetTrackCollection>();
  //L2_b50_NoCut_j50_c4cchad
  std::vector<Trig::Feature<TrigInDetTrackCollection> >::const_iterator pL2TrackItr = trigL2TrackVector.begin();
  std::vector<Trig::Feature<TrigInDetTrackCollection> >::const_iterator lastL2TrackItr = trigL2TrackVector.end();

  for(int j = 1; pL2TrackItr != lastL2TrackItr; ++pL2TrackItr, ++j) {

    const TrigInDetTrackCollection* pL2Track = (*pL2TrackItr).cptr();

    if (pL2Track->size()) {

      *m_log << MSG::DEBUG << " Retrieved LVL2 tracks with algo " << pL2Track->front()->algorithmId() << endreq;

      if (pL2Track->front()->algorithmId() == 6)
	pointerToL2TrackCollection = pL2Track;

    }
  }

  ////////////*******************//////////////
  //********* Retrieve LVL2 prm vtx *********//
  ////////////*******************//////////////

  //std::vector<Trig::Feature<TrigL2Bjet> > trigL2BjetVector = 
  //  (getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<TrigL2Bjet>();
  
  std::vector<Trig::Feature<TrigL2Bjet> > trigL2BjetVector = 
    (getTDT()->features(chainNameL2,TrigDefs::alsoDeactivateTEs)).get<TrigL2Bjet>();
  std::vector<Trig::Feature<TrigL2Bjet> >::const_iterator pL2BjetItr = trigL2BjetVector.begin();
  std::vector<Trig::Feature<TrigL2Bjet> >::const_iterator lastL2BjetItr = trigL2BjetVector.end();

  for(int j = 1; pL2BjetItr != lastL2BjetItr; ++pL2BjetItr, ++j) {

    const TrigL2Bjet* L2Bjet = (*pL2BjetItr).cptr();

    //    *m_log << MSG::DEBUG << "TrigL2Bjet->prmVtx() = " << L2Bjet->prmVtx() << endreq;

    m_prmVtxL2Bj = L2Bjet->prmVtx();

  }
  
  ////////////*******************//////////////
  //***** Compute LVL2 b-tagging weights ****//
  ////////////*******************//////////////
  
  if (m_sigmaX > m_setBeamSpotWidth || m_sigmaY > m_setBeamSpotWidth) {
    
    m_listCutAppliedLikelihood.push_back(1);
    m_listCutAppliedProbability.push_back(1);
    
    m_likelihood_L2->getLikelihoodTag();
    m_probability_L2->getProbabilityTag();
  } else {
    if(pointerToL2TrackCollection) {  
      
      m_likelihood_L2->getLikelihoodTag(pointerToL2TrackCollection, m_prmVtxL2Bj, m_trackJetFinderTool);
      m_probability_L2->getProbabilityTag(pointerToL2TrackCollection, m_prmVtxL2Bj, m_trackJetFinderTool);
      
      hist("L2Bjet_XIP3D_Recomputed","HLT/BjetMon/b-jet")->Fill(m_likelihood_L2->getXIP3D());
      hist("L2Bjet_XCHI2_Recomputed","HLT/BjetMon/b-jet")->Fill(m_probability_L2->getXCHI2());
      
      m_listCutAppliedLikelihood_L2 = m_listCutAppliedLikelihood;
      m_listCutAppliedProbability_L2 = m_listCutAppliedProbability;
      
      for(unsigned int i=0; i<m_mon_l2_trk_a0.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_a0","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_a0[i]);
     
      for(unsigned int i=0; i<m_mon_l2_trk_a0_sel.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_a0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_a0_sel[i]);
     
      for(unsigned int i=0; i<m_mon_l2_trk_Sa0_sel.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_Sa0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_Sa0_sel[i]);
    
      for(unsigned int i=0; i<m_mon_l2_trk_z0.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_z0","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_z0[i]);
    
      for(unsigned int i=0; i<m_mon_l2_trk_z0_sel.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_z0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_z0_sel[i]);
    
      for(unsigned int i=0; i<m_mon_l2_trk_z0_sel_PV.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_z0_sel_PV","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_z0_sel_PV[i]);
          
      for(unsigned int i=0; i<m_mon_l2_trk_Sz0_sel.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_trk_Sz0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_Sz0_sel[i]);
    
      for(unsigned int i=0; i<m_mon_l2_trk_prob.size(); i++) {
	hist("L2Bjet_"+chainNameL2+"_trk_prob","HLT/BjetMon/b-jet")->Fill(m_mon_l2_trk_prob[i]);
	hist("L2Bjet_"+chainNameL2+"_trk_prob_LS","HLT/BjetMon/LowStat")->Fill(m_mon_l2_trk_prob[i]);
      }
          
      for(unsigned int i=0; i<m_listCutAppliedLikelihood_L2.size(); i++)
	hist("L2Bjet_"+chainNameL2+"_stepsToSelect","HLT/BjetMon/b-jet")->Fill(m_listCutAppliedLikelihood_L2[i]);
   
    } else {

      *m_log << MSG::DEBUG << "No L2 track collection found" <<endreq;
  
    }
  }


  ////////////*******************//////////////
  //********** Retrieve EF tracks ***********//
  ////////////*******************//////////////
  
  const Rec::TrackParticleContainer* pointerToEFTrackCollection = 0;
  
  std::vector<Trig::Feature<Rec::TrackParticleContainer> > trigEFTrackVector = 
    (getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<Rec::TrackParticleContainer>();
  std::vector<Trig::Feature<Rec::TrackParticleContainer> >::const_iterator pEFTrackItr = trigEFTrackVector.begin();
  std::vector<Trig::Feature<Rec::TrackParticleContainer> >::const_iterator lastEFTrackItr = trigEFTrackVector.end();
  
  for(int j = 1; pEFTrackItr != lastEFTrackItr; ++pEFTrackItr, ++j) {
    
    const Rec::TrackParticleContainer* pEFTrack = (*pEFTrackItr).cptr();
    
    if (pEFTrack->size() != 0) {
      *m_log << MSG::DEBUG << " Retrieved EF tracks " <<endreq;
      pointerToEFTrackCollection = pEFTrack;
    }
  }
  
  
  ////////////*******************//////////////
  //********** Retrieve EF prm vtx **********//
  ////////////*******************//////////////

  std::vector<Trig::Feature<TrigEFBjet> > trigEFBjetVector = 
    (getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs)).get<TrigEFBjet>();
  std::vector<Trig::Feature<TrigEFBjet> >::const_iterator pEFBjetItr = trigEFBjetVector.begin();
  std::vector<Trig::Feature<TrigEFBjet> >::const_iterator lastEFBjetItr = trigEFBjetVector.end();

  for(int j = 1; pEFBjetItr != lastEFBjetItr; ++pEFBjetItr, ++j) {

    const TrigEFBjet* EFBjet = (*pEFBjetItr).cptr();

    //*m_log << MSG::DEBUG << "TrigEFBjet->prmVtx() = " << EFBjet->prmVtx() << endreq;

    m_prmVtxEFBj = EFBjet->prmVtx();    

  }

  ////////////********************//////////////  
  //****** Compute EF b-tagging weights ******//
  ////////////********************//////////////  
  
  if (m_sigmaX > m_setBeamSpotWidth || m_sigmaY > m_setBeamSpotWidth) {
    //   *m_log << MSG::DEBUG << "EF b-tagging weights Bjet BeamSpot "<< endreq;
    m_listCutAppliedLikelihood.push_back(1);
    m_listCutAppliedProbability.push_back(1);
    
    m_likelihood_EF->getLikelihoodTag();
    m_probability_EF->getProbabilityTag();

  } else {
  
    if(pointerToEFTrackCollection) {
  
      m_likelihood_EF->getLikelihoodTag(pointerToEFTrackCollection, m_prmVtxEFBj, m_trackJetFinderTool);
      m_probability_EF->getProbabilityTag(pointerToEFTrackCollection, m_prmVtxEFBj,  m_trackJetFinderTool);
      
      hist("EFBjet_XIP3D_Recomputed","HLT/BjetMon/b-jet")->Fill(m_likelihood_EF->getXIP3D());
      hist("EFBjet_XCHI2_Recomputed","HLT/BjetMon/b-jet")->Fill(m_probability_EF->getXCHI2());
      	      
      m_listCutAppliedLikelihood_EF = m_listCutAppliedLikelihood;
      m_listCutAppliedProbability_EF = m_listCutAppliedProbability;

      for(unsigned int i=0; i<m_mon_ef_trk_a0.size(); i++)
	hist("EFBjet_"+chainName+"_trk_a0","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_a0[i]);
      
      for(unsigned int i=0; i<m_mon_ef_trk_a0_sel.size(); i++)
       hist("EFBjet_"+chainName+"_trk_a0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_a0_sel[i]);
      	
      for(unsigned int i=0; i<m_mon_ef_trk_Sa0_sel.size(); i++)
	hist("EFBjet_"+chainName+"_trk_Sa0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_Sa0_sel[i]);
      	
      for(unsigned int i=0; i<m_mon_ef_trk_z0.size(); i++)
	hist("EFBjet_"+chainName+"_trk_z0","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_z0[i]);
      	
      for(unsigned int i=0; i<m_mon_ef_trk_z0_sel.size(); i++)
	hist("EFBjet_"+chainName+"_trk_z0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_z0_sel[i]);
      	
      for(unsigned int i=0; i<m_mon_ef_trk_z0_sel_PV.size(); i++)
	hist("EFBjet_"+chainName+"_trk_z0_sel_PV","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_z0_sel_PV[i]);
      	
      for(unsigned int i=0; i<m_mon_ef_trk_Sz0_sel.size(); i++)
	hist("EFBjet_"+chainName+"_trk_Sz0_sel","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_Sz0_sel[i]);
      	
      for(unsigned int i=0; i<m_mon_ef_trk_prob.size(); i++) {
	hist("EFBjet_"+chainName+"_trk_prob","HLT/BjetMon/b-jet")->Fill(m_mon_ef_trk_prob[i]);
	hist("EFBjet_"+chainName+"_trk_prob_LS","HLT/BjetMon/LowStat")->Fill(m_mon_ef_trk_prob[i]);
      }
      
      for(unsigned int i=0; i<m_listCutAppliedLikelihood_EF.size(); i++)
	hist("EFBjet_"+chainName+"_stepsToSelect","HLT/BjetMon/b-jet")->Fill(m_listCutAppliedLikelihood_EF[i]);
      
    } else {
      
      *m_log << MSG::DEBUG << "No Ef track collection found" <<endreq;

    }
  }

  ////////////********************////////////// 
  //***** LVL2 b-tagging with chainName ******//
  ////////////********************////////////// 
  //  *m_log << MSG::DEBUG <<" chainName "<<chainName<< endreq;
  //* feature already retrieved above *//
  pL2BjetItr = trigL2BjetVector.begin();
  
  for(int j = 1; pL2BjetItr != lastL2BjetItr; ++pL2BjetItr, ++j) {
    
    *m_log << MSG::VERBOSE << "Looking at TrigL2Bjet " << j << "/" << trigL2BjetVector.size() << " in trigger "<< chainName << endreq;
    
    const TrigL2Bjet* L2Bjet = (*pL2BjetItr).cptr();
    
    *m_log << MSG::DEBUG << "TrigL2Bjet->xIP1D() = "   << L2Bjet->xIP1D() << "; TrigL2Bjet->xIP2D() = " << L2Bjet->xIP2D()
	   << "; TrigL2Bjet->xIP3D() = " << L2Bjet->xIP3D() << "; TrigL2Bjet->xCHI2() = " << L2Bjet->xCHI2() 
    	   <<"; TrigL2Bjet->xSV() = " << L2Bjet->xSV()<< "; TrigL2Bjet->xMVtx() = " << L2Bjet->xMVtx()
    	   << "; TrigL2Bjet->xNVtx() = " << L2Bjet->xNVtx()<< "; TrigL2Bjet->xEVtx() = " << L2Bjet->xEVtx()<<endreq;    
    
    if(L2Bjet->xComb()>-42){
      hist("L2Bjet_"+chainNameL2+"_XIP1D","HLT/BjetMon/b-jet")->Fill(L2Bjet->xIP1D());
      hist("L2Bjet_"+chainNameL2+"_XIP2D","HLT/BjetMon/b-jet")->Fill(L2Bjet->xIP2D());
      hist("L2Bjet_"+chainNameL2+"_XIP3D","HLT/BjetMon/b-jet")->Fill(L2Bjet->xIP3D());
      hist("L2Bjet_"+chainNameL2+"_XComb","HLT/BjetMon/Shifter")->Fill(L2Bjet->xComb());
      hist("L2Bjet_"+chainNameL2+"_XComb_LS","HLT/BjetMon/LowStat")->Fill(L2Bjet->xComb());
      hist("L2Bjet_"+chainNameL2+"_CounterXComb","HLT/BjetMon/b-jet")->Fill(1);
    }
    else if (L2Bjet->xComb()==-46)
      hist("L2Bjet_"+chainNameL2+"_CounterXComb","HLT/BjetMon/b-jet")->Fill(0);
    else  hist("L2Bjet_"+chainNameL2+"_CounterXComb","HLT/BjetMon/b-jet")->Fill(-1);
    
    //    *m_log << MSG::DEBUG <<"  L2Bjet->xSV() : "<<L2Bjet->xSV()<<"  L2Bjet->xMVtx():  "<<L2Bjet->xMVtx()<<
    //      "  L2Bjet->xEVtx() : "<<L2Bjet->xMVtx()<<"  L2Bjet->xNVtx() : "<<L2Bjet->xNVtx()<<endreq;
    
    if(L2Bjet->xMVtx()>0){
      hist("L2Bjet_"+chainNameL2+"_XSV0","HLT/BjetMon/b-jet")->Fill(L2Bjet->xSV());
      hist("L2Bjet_"+chainNameL2+"_XMVtx","HLT/BjetMon/b-jet")->Fill(L2Bjet->xMVtx());
      hist("L2Bjet_"+chainNameL2+"_XNVtx","HLT/BjetMon/b-jet")->Fill(L2Bjet->xNVtx());
      hist("L2Bjet_"+chainNameL2+"_XEVtx","HLT/BjetMon/b-jet")->Fill(L2Bjet->xEVtx());
      hist("L2Bjet_"+chainNameL2+"_CounterXSV","HLT/BjetMon/b-jet")->Fill(1);
    }
    else hist("L2Bjet_"+chainNameL2+"_CounterXSV","HLT/BjetMon/b-jet")->Fill(0);
    
    hist("L2Bjet_"+chainNameL2+"_XCHI2","HLT/BjetMon/Shifter")->Fill(L2Bjet->xCHI2());
    hist("L2Bjet_"+chainNameL2+"_XCHI2_LS","HLT/BjetMon/LowStat")->Fill(L2Bjet->xCHI2());
  }


  ////////////********************////////////// 
  //***** EF b-tagging with "+chainName+"*****//
  ////////////********************////////////// 


  //* feature already retrieved above *//
  pEFBjetItr = trigEFBjetVector.begin();

  for(int j = 1; pEFBjetItr != lastEFBjetItr; ++pEFBjetItr, ++j) {

    *m_log << MSG::VERBOSE << "Looking at TrigEFBjet " << j << "/" << trigEFBjetVector.size() << " in trigger "<<chainName << endreq;

    const TrigEFBjet* EFBjet = (*pEFBjetItr).cptr();

    *m_log << MSG::DEBUG   << "TrigEFBjet->xIP1D() = "   << EFBjet->xIP1D() << "; TrigEFBjet->xIP2D() = " << EFBjet->xIP2D()
    	   << "; TrigEFBjet->xIP3D() = " << EFBjet->xIP3D() << "; TrigEFBjet->xCHI2() = " << EFBjet->xCHI2() 
	   <<"; TrigEFBjet->xSV() = " << EFBjet->xSV()<< "; TrigEFBjet->xMVtx() = " << EFBjet->xMVtx()
	   << "; TrigEFBjet->xNVtx() = " << EFBjet->xNVtx()<< "; TrigEFBjet->xEVtx() = " << EFBjet->xEVtx()<<endreq; 
     
    if(EFBjet->xComb()>-42){
      hist("EFBjet_"+chainName+"_XIP1D","HLT/BjetMon/b-jet")->Fill(EFBjet->xIP1D());
      hist("EFBjet_"+chainName+"_XIP2D","HLT/BjetMon/b-jet")->Fill(EFBjet->xIP2D());
      hist("EFBjet_"+chainName+"_XIP3D","HLT/BjetMon/b-jet")->Fill(EFBjet->xIP3D());
      hist("EFBjet_"+chainName+"_XComb","HLT/BjetMon/Shifter")->Fill(EFBjet->xComb());
      hist("EFBjet_"+chainName+"_CounterXComb","HLT/BjetMon/b-jet")->Fill(1);
    }
    else if (EFBjet->xComb()==-46)
      hist("EFBjet_"+chainName+"_CounterXComb","HLT/BjetMon/b-jet")->Fill(0);
    else  hist("EFBjet_"+chainName+"_CounterXComb","HLT/BjetMon/b-jet")->Fill(-1);

    if(EFBjet->xMVtx()>0){
      hist("EFBjet_"+chainName+"_XSV0","HLT/BjetMon/b-jet")->Fill(EFBjet->xSV());
      hist("EFBjet_"+chainName+"_XMVtx","HLT/BjetMon/b-jet")->Fill(EFBjet->xMVtx());
      hist("EFBjet_"+chainName+"_XNVtx","HLT/BjetMon/b-jet")->Fill(EFBjet->xNVtx());
      hist("EFBjet_"+chainName+"_XEVtx","HLT/BjetMon/b-jet")->Fill(EFBjet->xEVtx());
      hist("EFBjet_"+chainName+"_CounterXSV","HLT/BjetMon/b-jet")->Fill(1);
    }
    else hist("EFBjet_"+chainName+"_CounterXSV","HLT/BjetMon/b-jet")->Fill(0);

    hist("EFBjet_"+chainName+"_XCHI2","HLT/BjetMon/Shifter")->Fill(EFBjet->xCHI2());
    hist("EFBjet_"+chainName+"_XCHI2_LS","HLT/BjetMon/LowStat")->Fill(EFBjet->xCHI2());
   
  }

 
  return StatusCode::SUCCESS;
}



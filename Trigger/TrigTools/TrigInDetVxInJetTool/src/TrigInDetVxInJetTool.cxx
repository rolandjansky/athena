/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VxVertex/RecVertex.h"
#include "TrigInDetVxInJetTool/TrigInDetVxInJetTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TMath.h"

#include "GaudiKernel/ToolFactory.h"

#include<iostream>

#include "TrigInterfaces/Algo.h"

//
//Constructor-------------------------------------------------------------- 
TrigInDetVxInJetTool::TrigInDetVxInJetTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_CutSctHits(0),   // was 4
  m_CutPixelHits(2), // was 1
  m_CutSiHits(4),    // was 7
  m_CutBLayHits(1),  // was 0
  m_CutSharedHits(1),
  m_CutPt(1000.),
  m_CutZVrt(25.),
  m_CutA0(5.),
  m_CutProbChi2(0.001),
  m_CutChi2Ndof(0.2),
  m_SecTrkChi2Cut(10.),
  m_ConeForTag(0.4),
  m_Sel2VrtChi2Cut(4.5),
  m_Sel2VrtSigCut(3.0),
  m_TrkSigCut(2.0), 
  m_TrkSigSumCut(2.), 
  m_A0TrkErrorCut(1.0),
  m_ZTrkErrorCut(5.0),
  m_AntiPileupSigRCut(2.0),
  m_AntiPileupSigZCut(6.0),
  m_AntiFake2trVrtCut(0.5),
  m_RobustFit(5),
  m_Xbeampipe (0.),
  m_Ybeampipe (0.),
  m_XlayerB (0.),
  m_YlayerB (0.),
  m_Xlayer1 (0.),
  m_Ylayer1 (0.),
  m_Xlayer2 (0.),
  m_Ylayer2 (0.),
  m_Rbeampipe (29.4),
  m_RlayerB (50.5),
  m_Rlayer1 (88.0),
  m_Rlayer2 (123.0),
  m_getNegativeTail(true),
  m_getNegativeTag(false),
  m_doTrackSelection(true),
  m_fitSvc("TrigVKalFitter/VertexFitterTool",this)
{

  declareInterface< ITrigInDetVxInJetTool >(this);

  declareProperty("CutSctHits",        m_CutSctHits ,       "Remove track is it has less SCT hits" );
  declareProperty("CutPixelHits",      m_CutPixelHits,      "Remove track is it has less Pixel hits");
  declareProperty("CutSiHits",         m_CutSiHits,         "Remove track is it has less Pixel+SCT hits"  );
  declareProperty("CutBLayHits",       m_CutBLayHits,       "Remove track is it has less B-layer hits"   );
  declareProperty("CutSharedHits",     m_CutSharedHits,     "Reject final 2tr vertices if tracks have shared hits" );

  declareProperty("CutPt",             m_CutPt,             "Track Pt selection cut"  );
  declareProperty("CutA0",             m_CutA0,             "Track A0 selection cut"  );
  declareProperty("CutZVrt",           m_CutZVrt,           "Track Z impact selection cut"   );
  declareProperty("ConeForTag",        m_ConeForTag,        "Cone around jet direction for track selection"  );
  declareProperty("CutProbChi2",       m_CutProbChi2,       "Track Chi2 probability selection cut" );
  declareProperty("CutChi2Ndof",       m_CutChi2Ndof,       "Track Chi2/Ndof selection cut" );
  declareProperty("TrkSigCut",         m_TrkSigCut,         "Track 3D impact significance w/r primary vertex" );
  declareProperty("TrkSigSumCut",      m_TrkSigSumCut,      "Sum of 3D track significances cut for 2tr vertex search" );
  declareProperty("SecTrkChi2Cut",     m_SecTrkChi2Cut,     "Track - common secondary vertex association cut. Single Vertex Finder only" );

  declareProperty("A0TrkErrorCut",     m_A0TrkErrorCut,     "Track A0 error cut" );
  declareProperty("ZTrkErrorCut",      m_ZTrkErrorCut,      "Track Z impact error cut" );

  declareProperty("Sel2VrtChi2Cut",    m_Sel2VrtChi2Cut,    "Cut on Chi2 of 2-track vertex for initial selection"  );
  declareProperty("Sel2VrtSigCut",     m_Sel2VrtSigCut,     "Cut on significance of 3D distance between initial 2-track vertex and PV"  );
  declareProperty("AntiPileupSigRCut", m_AntiPileupSigRCut, "Remove tracks with low Rphi and big Z impacts presumably coming from pileup"  );
  declareProperty("AntiPileupSigZCut", m_AntiPileupSigZCut, "Remove tracks with low Rphi and big Z impacts presumably coming from pileup"  );
  declareProperty("AntiFake2trVrtCut", m_AntiFake2trVrtCut, "Cut to reduce fake 2-track vertices contribution.Single Vertex Finder only"  );

  declareProperty("RobustFit",  m_RobustFit, "Use vertex fit with RobustFit functional(VKalVrt) for common secondary vertex fit" );

  declareProperty("Xbeampipe", m_Xbeampipe);
  declareProperty("Ybeampipe", m_Ybeampipe);
  declareProperty("XlayerB",   m_XlayerB  );
  declareProperty("YlayerB",   m_YlayerB  );
  declareProperty("Xlayer1",   m_Xlayer1  );
  declareProperty("Ylayer1",   m_Ylayer1  );
  declareProperty("Xlayer2",   m_Xlayer2  );
  declareProperty("Ylayer2",   m_Ylayer2  );
  declareProperty("Rbeampipe", m_Rbeampipe);
  declareProperty("RlayerB",   m_RlayerB  );
  declareProperty("Rlayer1",   m_Rlayer1  );
  declareProperty("Rlayer2",   m_Rlayer2  );

  declareProperty("getNegativeTail", m_getNegativeTail, "Allow secondary vertex behind the primary one (negative) w/r jet direction (not for multivertex!)" );
  declareProperty("getNegativeTag",  m_getNegativeTag,  "Return ONLY negative secondary vertices (not for multivertex!)"   );

  declareProperty("VertexFitterTool", m_fitSvc);

  m_massPi  = 139.5702 ;
  m_massP   = 938.272  ;
  m_massE   =   0.511  ;
  m_massK0  = 497.648  ;
  m_massLam =1115.683  ;
}

//Destructor---------------------------------------------------------------
TrigInDetVxInJetTool::~TrigInDetVxInJetTool(){
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "TrigInDetVxInJetTool destructor called" << endreq;
}

//Initialize---------------------------------------------------------------
StatusCode TrigInDetVxInJetTool::initialize(){
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "TrigInDetVxInJetTool initialize() called" << endreq;

  StatusCode sc; sc.isSuccess();

 
  if (m_fitSvc.retrieve().isFailure()) {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Could not find Trk::TrkVKalVrtFitter" << endreq;
    return StatusCode::SUCCESS;
  } else {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "TrigInDetVxInJetTool TrkVKalVrtFitter found" << endreq;
  }

  if(m_getNegativeTag){
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << " Negative TAG is requested! " << endreq;
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Not compatible with negativeTAIL option, so getNegativeTail is set to FALSE." << endreq;
    m_getNegativeTail=false;
  }

  for(int ntv=2; ntv<=10; ntv++) m_chiScale[ntv]=TMath::ChisquareQuantile(0.9,2.*ntv-3.)/ntv; m_chiScale[0]=m_chiScale[2];
  for(int ntv=2; ntv<=10; ntv++) m_chiScale[ntv]/=m_chiScale[0];

  if(m_RobustFit>7)m_RobustFit=7;
  if(m_RobustFit<0)m_RobustFit=0;
  
  return StatusCode::SUCCESS;

}


StatusCode TrigInDetVxInJetTool::finalize() {
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"TrigInDetVxInJetTool finalize()" << endreq;
  return StatusCode::SUCCESS; 
}


const TrigVertex* TrigInDetVxInJetTool::findSecVertex(const Trk::RecVertex & PrimVrt,
                                                      const CLHEP::HepLorentzVector & JetDir,
                                                      const std::vector<const TrigInDetTrack*> & InpTrk) {

  std::vector<double> Results;
  std::vector<const TrigInDetTrack*> SelSecTrk;
  std::vector< std::vector<const TrigInDetTrack*> > SelSecTrkPerVrt;
  std::vector<const TrigInDetTrack*> TrkFromV0;

  TrigVertex* secVrt = GetVrtSec(InpTrk, PrimVrt, JetDir, Results, SelSecTrk, TrkFromV0);
  //m_fitSvc->clearMemory();
  return secVrt;

}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMenuHists.cxx
*/
#include "TriggerMenuAnalysis/TrigMenuHists.h"
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"

using namespace std;

static const InterfaceID IID_TrigMenuHists = InterfaceID("TrigMenuHists", 1, 0);

const InterfaceID& TrigMenuHists::interfaceID() {
  return IID_TrigMenuHists;
}


TrigMenuHists::TrigMenuHists(const std::string& type, const std::string& name, 
			     const IInterface* p) :
  AlgTool(type, name, p), 
  mTHistSvc("THistSvc", this->name()), 
  mHistMuFastPt(0), mHistMuInDetPt(0), mHistMuCombPt(0) {
  declareInterface<TrigMenuHists>(this);
}

TrigMenuHists::~TrigMenuHists() {
}

StatusCode TrigMenuHists::initialize() {
  MsgStream log(msgSvc(), name());

  if (mTHistSvc.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve service THistSvc" << endreq;
  }
  
  mHistMuFastPt = new TH1F("MuFastPt", "MuFast p_{T}", 100, 0, 100);
  mHistMuInDetPt = new TH1F("MuInDetPt", "MuInDet p_{T}", 100, 0, 100);
  mHistMuCombPt = new TH1F("MuCombPt", "MuComb p_{T}", 100, 0, 100);

  mTHistSvc->regHist("/MenuHist/TrigMenu/MuFastPt", mHistMuFastPt);
  mTHistSvc->regHist("/MenuHist/TrigMenu/MuInDetPt", mHistMuInDetPt);
  mTHistSvc->regHist("/MenuHist/TrigMenu/MuCombPt", mHistMuCombPt);

  return StatusCode::SUCCESS;
}

StatusCode TrigMenuHists::finalize() {
  return StatusCode::SUCCESS;
}

bool TrigMenuHists::fillMuonObjects(const std::vector<HLTObjectsInRoI*>& objs) {
  MsgStream log(msgSvc(), name());

  std::vector<HLTObjectsInRoI*>::const_iterator p;

  for(p=objs.begin(); p!=objs.end(); ++p) {
    MuonObjectsInRoI* mu_obj = dynamic_cast<MuonObjectsInRoI*>( (*p));
    if (mu_obj->getMuonFeature()) {
      log << MSG::INFO << "mufast pt: " << mu_obj->getMuonFeature()->pt()*0.001
	  << endreq;
      mHistMuFastPt->Fill(mu_obj->getMuonFeature()->pt()*0.001);
    }
    if (mu_obj->getCombinedMuonFeature()) {
      log << MSG::INFO << "mufast pt: " 
	  << mu_obj->getCombinedMuonFeature()->pt()*0.001 << endreq;
      mHistMuCombPt->Fill(mu_obj->getCombinedMuonFeature()->pt());
    }
  }
  return true;
}


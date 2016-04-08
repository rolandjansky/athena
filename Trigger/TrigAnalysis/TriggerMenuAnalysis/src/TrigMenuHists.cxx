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
  AthAlgTool(type, name, p), 
  m_THistSvc("THistSvc", this->name()), 
  m_histMuFastPt(0), m_histMuInDetPt(0), m_histMuCombPt(0) {
  declareInterface<TrigMenuHists>(this);
}

TrigMenuHists::~TrigMenuHists() {
}

StatusCode TrigMenuHists::initialize() {
  MsgStream log(msgSvc(), name());

  if (m_THistSvc.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve service THistSvc" << endreq;
  }
  
  m_histMuFastPt = new TH1F("MuFastPt", "MuFast p_{T}", 100, 0, 100);
  m_histMuInDetPt = new TH1F("MuInDetPt", "MuInDet p_{T}", 100, 0, 100);
  m_histMuCombPt = new TH1F("MuCombPt", "MuComb p_{T}", 100, 0, 100);

  m_THistSvc->regHist("/MenuHist/TrigMenu/MuFastPt", m_histMuFastPt);
  m_THistSvc->regHist("/MenuHist/TrigMenu/MuInDetPt", m_histMuInDetPt);
  m_THistSvc->regHist("/MenuHist/TrigMenu/MuCombPt", m_histMuCombPt);

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
    if (mu_obj) {
      if (mu_obj->getMuonFeature()) {
	log << MSG::INFO << "mufast pt: " 
	    << mu_obj->getMuonFeature()->pt()*0.001 << endreq;
	m_histMuFastPt->Fill(mu_obj->getMuonFeature()->pt()*0.001);
      }
      if (mu_obj->getCombinedMuonFeature()) {
	log << MSG::INFO << "mufast pt: " 
	    << mu_obj->getCombinedMuonFeature()->pt()*0.001 << endreq;
	m_histMuCombPt->Fill(mu_obj->getCombinedMuonFeature()->pt());
      }
    }
  }
  return true;
}


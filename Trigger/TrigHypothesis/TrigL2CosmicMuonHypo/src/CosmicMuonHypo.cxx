/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CosmicMuonHypo.cxx
*/
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigL2CosmicMuonHypo/CosmicMuonHypo.h"
#include "TrigCosmicEvent/CosmicMuon.h"
#include "TrigCosmicEvent/MdtTrackSegment.h"
#include <iostream>

using namespace std;


CosmicMuonHypo::CosmicMuonHypo(const std::string& name, ISvcLocator* svcloc) 
  : HLT::HypoAlgo(name, svcloc) {
  declareProperty("NMuons", mNMuons=1);

  declareProperty("MuonEtaMin", mMuonEtaMin=-10);
  declareProperty("MuonEtaMax", mMuonEtaMax=10);
  declareProperty("MuonDCA_Z0", mMuonDCA_Z0=0.0);
  declareProperty("MuonDCA_XY", mMuonDCA_XY=10.0E+3);
  declareProperty("MuonDCA_Z", mMuonDCA_Z=20.0E+3);
  declareProperty("MuonDCA_3D", mMuonDCA_3D=10.0E+3);

  declareProperty("NRpcHits", mNRpcHits=2);
  declareProperty("NMdtStations", mNMdtStations=1);

  declareProperty("UpDownDeltaEta", mUpDownDeltaEta=-1.0);
  declareProperty("UpDownDeltaPhi", mUpDownDeltaPhi=-1.0);
  declareProperty("UpDownTiming", mUpDownTiming=0.0);

  // Monitoring
  declareMonitoredStdContainer("AcceptedMuonEta", mAcceptedMuonEta);
  declareMonitoredStdContainer("AcceptedMuonPhi", mAcceptedMuonPhi);
  declareMonitoredStdContainer("AcceptedMuonDCA_XY", mAcceptedMuonDCA_XY);
  declareMonitoredStdContainer("AcceptedMuonDCA_Z", mAcceptedMuonDCA_Z);
  declareMonitoredStdContainer("AcceptedMuonDCA_3D", mAcceptedMuonDCA_3D);
}

CosmicMuonHypo::~CosmicMuonHypo() {
}

HLT::ErrorCode CosmicMuonHypo::hltInitialize() {
  msg() << MSG::DEBUG << "hltInitialize()" << endreq;

  msg() << MSG::INFO << "Parameters for CosmicMuonHypo:" << name() << endreq;
  msg() << MSG::INFO << "NMuons            : " << mNMuons << endreq;

  msg() << MSG::INFO << "MuonEtaMin        : " << mMuonEtaMin << endreq;
  msg() << MSG::INFO << "MuonEtaMax        : " << mMuonEtaMax << endreq;
  msg() << MSG::INFO << "MuonDCA_Z0        : " << mMuonDCA_Z0 << endreq;
  msg() << MSG::INFO << "MuonDCA_XY        : " << mMuonDCA_XY << endreq;
  msg() << MSG::INFO << "MuonDCA_Z         : " << mMuonDCA_Z << endreq;
  msg() << MSG::INFO << "MuonDCA_3D        : " << mMuonDCA_3D << endreq;

  msg() << MSG::INFO << "NRpcHits          : " << mNRpcHits << endreq;
  msg() << MSG::INFO << "NMdtStations      : " << mNMdtStations << endreq;

  msg() << MSG::INFO << "UpDownDeltaEta    : " << mUpDownDeltaEta << endreq;
  msg() << MSG::INFO << "UpDownDeltaPhi    : " << mUpDownDeltaPhi << endreq;
  msg() << MSG::INFO << "UpDownTiming      : " << mUpDownTiming << endreq;

  msg() << MSG::INFO << "CosmicMuonHypo initialized successfully" << endreq;

  return HLT::OK;
}

HLT::ErrorCode CosmicMuonHypo::hltFinalize() {
  msg() << MSG::INFO << "hltFinalize" << endreq;
  return HLT::OK;
}

HLT::ErrorCode CosmicMuonHypo::hltBeginRun() {
  msg() << MSG::INFO << "hltBeginRun" << endreq;
  return HLT::OK;
}

HLT::ErrorCode CosmicMuonHypo::hltEndRun() {
  msg() << MSG::INFO << "hltEndRun" << endreq;
  return HLT::OK;
}

HLT::ErrorCode CosmicMuonHypo::hltExecute(const HLT::TriggerElement* outputTE, 
					  bool& pass) {
  HLT::ErrorCode hec;
  pass = false;
  // If the program didn't reach the end it's likely to be rejected

  clearEvent();

  std::vector<const CosmicMuonCollection*> muon_colls;
  if ( (hec = getFeatures(outputTE, muon_colls)) != HLT::OK) {
    msg() << MSG::ERROR << "Couldn't get the object CosmicMuons" 
	  << endreq;
    return hec;
  }

  std::vector<const CosmicMuonCollection*>::const_iterator muons;
  int nmuons=0;
  for (muons=muon_colls.begin(); muons!=muon_colls.end(); ++muons) {
    CosmicMuonCollection::const_iterator p_muon;
    for (p_muon=(*muons)->begin(); p_muon!=(*muons)->end(); ++p_muon) {
//       mAcceptedMuonEta.push_back( (*p_muon)->eta());
//       mAcceptedMuonPhi.push_back( (*p_muon)->phi());
//       mAcceptedMuonDCA_XY.push_back( (*p_muon)->dcaXY());
//       mAcceptedMuonDCA_Z.push_back( (*p_muon)->dca2D_Z());
      if (acceptThisMuon(**p_muon)) {
	mAcceptedMuonEta.push_back( (*p_muon)->eta());
	mAcceptedMuonPhi.push_back( (*p_muon)->phi());
	mAcceptedMuonDCA_XY.push_back( (*p_muon)->dcaXY());
	mAcceptedMuonDCA_Z.push_back( (*p_muon)->dca2D_Z());
	mAcceptedMuonDCA_3D.push_back( (*p_muon)->dca3D(0,0,mMuonDCA_Z0));
	nmuons ++;
	msg() << MSG::DEBUG << "Accepted muon candidate" << endreq;
      } else {
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Muon candidate rejected" << endreq;
	  msg() << MSG::DEBUG << "  eta            : " 
		<< (*p_muon)->eta() << endreq;
	  msg() << MSG::DEBUG << "  DCA(xy)        : " 
		<< (*p_muon)->dcaXY() << " (cm)" << endreq;
	  msg() << MSG::DEBUG << "  DCA(2D,z)      : " 
		<< (*p_muon)->dca2D_Z() << " (cm)" << endreq;
	  msg() << MSG::DEBUG << "  DCA(3D)        : " 
		<< (*p_muon)->dca3D() << " (cm)" << endreq;
	  msg() << MSG::DEBUG << "  N(RPC pairs)   : " 
		<< (*p_muon)->NRpcPairs() << endreq;
	  msg() << MSG::DEBUG << "  N(MDT segments): " 
		<< (*p_muon)->NMdtSegs() << endreq;
	}
      }
    }
  }

  if (nmuons >= mNMuons) {
    pass = true;
  } else {
    pass = false;
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Passed or not: " << pass << endreq;
  }

  return HLT::OK;
}

bool CosmicMuonHypo::acceptThisMuon(const CosmicMuon& muon) const {
  bool status=false;

  // CosmicMuon returns DCA in (cm) while cuts are defined in (mm)
  if (mMuonEtaMin <= muon.eta() && muon.eta() <= mMuonEtaMax &&
      (mMuonDCA_XY <= 0 || muon.dcaXY()*10 <= mMuonDCA_XY) &&
      (mMuonDCA_Z <= 0 || fabs(muon.dca2D_Z()*10-mMuonDCA_Z0) <= mMuonDCA_Z) &&
      (mMuonDCA_3D <= 0 || muon.dca3D(0,0,mMuonDCA_Z0*0.1)*10<=mMuonDCA_3D) &&
      muon.NRpcPairs() >= mNRpcHits && 
      muon.NMdtSegs() >= mNMdtStations) {
    status = true;
  } else {
    status = false;
  }

  return status;
}

void CosmicMuonHypo::clearEvent() {
  mAcceptedMuonEta.clear();
  mAcceptedMuonPhi.clear();
  mAcceptedMuonDCA_XY.clear();
  mAcceptedMuonDCA_Z.clear();
  mAcceptedMuonDCA_3D.clear();
}


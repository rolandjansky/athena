/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CombinedIDMuonHypo.cxx
*/
#include "GaudiKernel/ITHistSvc.h"
#include "TrigL2CosmicMuonHypo/CombinedIDMuonHypo.h"
#include "TrigCosmicEvent/CosmicMuon.h"
#include "TrigCosmicEvent/CombinedIDMuonObject.h"
#include "TrigCosmicEvent/CombinedIDMuonObjectCollection.h"
#include "TrigCosmicEvent/CombinedIDHalfTracksObject.h"
#include "TrigCosmicEvent/CombinedIDHalfTracksObjectCollection.h"

using namespace std;


CombinedIDMuonHypo::CombinedIDMuonHypo(const std::string& name, ISvcLocator* svcloc) 
  : HLT::HypoAlgo(name, svcloc) {
  declareProperty("CombIDMuNtracks", mCombIDMuNtracks=1);
  declareProperty("CombIDMu_DeltaPhi0Cut", mCombIDMu_DeltaPhi0Cut=0.1);
  declareProperty("CombIDMu_DeltaEtaCut", mCombIDMu_DeltaEtaCut=0.1);
  declareProperty("CombIDMu_DeltaThetaCut", mCombIDMu_DeltaThetaCut=0.1);

  // Monitoring
  declareMonitoredStdContainer("HistIDMuondPhi_Accepted",  mHistdPhi_Cut);
  declareMonitoredStdContainer("HistIDMuondEta_Accepted",  mHistdEta_Cut);
  declareMonitoredStdContainer("HistIDMuondTheta_Accepted",  mHistdTheta_Cut);

}

CombinedIDMuonHypo::~CombinedIDMuonHypo() {
}

HLT::ErrorCode CombinedIDMuonHypo::hltInitialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "initialize()" << endreq;

//   StatusCode sc2 = service("StoreGateSvc", mStoreGate);
//   if (sc2.isFailure()) {
//     log << MSG::FATAL << "cannot get StoreGateSvc" << endreq;
//     return HLT::ERROR;
//   }

  log << MSG::INFO << "Parameters for CombinedIDMuonHypo:" << name() << endreq;
  log << MSG::INFO << "CombIDMuNtracks   : " << mCombIDMuNtracks << endreq;
  log << MSG::INFO << "CombIDMu_DeltaPhi0Cut    : " << mCombIDMu_DeltaPhi0Cut << endreq;
  log << MSG::INFO << "CombIDMu_DeltaEtaCut    : " << mCombIDMu_DeltaEtaCut << endreq;
  log << MSG::INFO << "CombIDMu_DeltaThetaCut    : " << mCombIDMu_DeltaThetaCut << endreq;
  
  log << MSG::INFO << "CombinedIDMuonHypo initialized successfully" << endreq;

  return HLT::OK;
}

HLT::ErrorCode CombinedIDMuonHypo::hltFinalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize()" << endreq;
  return HLT::OK;
}


//bool CombinedIDMuonHypo::execHLTAlgorithm(TriggerElement* outputTE) {
HLT::ErrorCode CombinedIDMuonHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
  MsgStream log(msgSvc(), name());
  int output_level = msgSvc()->outputLevel();
  bool passed = false;


  if (output_level <= MSG::DEBUG) {
    log << MSG::DEBUG << " in  hltExecute "<< endreq;
  }

  // clear event ?
  mHistdPhi_Cut.clear();
  mHistdEta_Cut.clear();
  mHistdTheta_Cut.clear();


  // Retrieve vector of CombinedIDMuon objects

 //  const DataVector<CombinedIDMuonObject>* combIDMuon =0; 
  std::vector<const CombinedIDMuonObjectCollection*> combIDMuons; 

//   StatusCode st = mStoreGate->retrieve(combIDMuon);
//   if (st.isFailure()) {
//     log << MSG::FATAL << "Cannot get DataVector<CombinedIDMuonObject>: " <<combIDMuon  << endreq;
//     return HLT::ERROR;
//     //    return false;
//   }


  HLT::ErrorCode status2 = getFeatures(inputTE, combIDMuons);
   if(status2 != HLT::OK) 
     {
       msg() << MSG::DEBUG << " Cannot get CombinedIDMuonObjects" << endreq;
       return status2;
     } 
   else 
     {

       msg() << MSG::VERBOSE << "execHLTAlgorithm: got vector Of CombinedIDMuonObjects of size "<<
	 combIDMuons.size()<< endreq;
       if (combIDMuons.size()==0) return HLT::OK;

       const CombinedIDMuonObjectCollection* combidmuon = combIDMuons[0];
      
       
       
       // DataVector<CombinedIDMuonObject>::const_iterator idmuons;
       CombinedIDMuonObjectCollection::const_iterator idmuons; // =combID.begin();
       
       int n_idmuons=0;
       for (idmuons=combidmuon->begin(); idmuons!=combidmuon->end(); ++idmuons) {
	 
	 if ( fabs((*idmuons)->delta_phi0()-M_PI) < mCombIDMu_DeltaPhi0Cut &&
	      fabs((*idmuons)->delta_eta()) < mCombIDMu_DeltaEtaCut &&
	      fabs((*idmuons)->delta_theta()) < mCombIDMu_DeltaThetaCut) {
	   
	   mHistdPhi_Cut.push_back((*idmuons)->delta_phi0());
	   mHistdEta_Cut.push_back((*idmuons)->delta_eta());
	   mHistdTheta_Cut.push_back((*idmuons)->delta_theta());
	   
	   n_idmuons ++; 
	 }
       }
       
       
       if (n_idmuons>=mCombIDMuNtracks) {
	 passed = true;
       } else {
	 passed = false;
       }

     }

   if (output_level <= MSG::DEBUG) {
     log << MSG::DEBUG << "Passed or not: " << passed << endreq;
   }
   
   if (passed)
     pass = true;
   else
     pass = false;
   return HLT::OK;
}

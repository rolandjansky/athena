/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CombinedIDHalfTracksHypo.cxx
*/
#include "GaudiKernel/ITHistSvc.h"
#include "TrigL2CosmicMuonHypo/CombinedIDHalfTracksHypo.h"
#include "TrigCosmicEvent/CombinedIDHalfTracksObject.h"
#include "TrigCosmicEvent/CombinedIDHalfTracksObjectCollection.h"
#include <cmath>

using namespace std;


CombinedIDHalfTracksHypo::CombinedIDHalfTracksHypo(const std::string& name, ISvcLocator* svcloc) 
  : HLT::HypoAlgo(name, svcloc) {
  declareProperty("CombIDNtracks",mCombIDNtracks=1);
  declareProperty("CombID_DeltaPhi0Cut",  mCombID_DeltaPhi0Cut=0.1);
  declareProperty("CombID_DeltaEtaCut", mCombID_DeltaEtaCut=0.1);
  // Monitoring
  declareMonitoredStdContainer("HistIDdPhi_Accepted",  mHistIDdPhi_Cut);
  declareMonitoredStdContainer("HistIDdEta_Accepted",    mHistIDdEta_Cut);

}

CombinedIDHalfTracksHypo::~CombinedIDHalfTracksHypo() {
}

HLT::ErrorCode CombinedIDHalfTracksHypo::hltInitialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "initialize()" << endreq;

 //  StatusCode sc2 = service("StoreGateSvc", mStoreGate);
//   if (sc2.isFailure()) {
//     log << MSG::FATAL << "cannot get StoreGateSvc" << endreq;
//     return HLT::ERROR;
//   }


  log << MSG::INFO << "Parameters for CombinedIDHalfTracksHypo:" << name() << endreq;
  log << MSG::INFO << "CombIDNtracks   : " <<mCombIDNtracks << endreq;
  log << MSG::INFO << "CombID_DeltaPhi0Cut    : " <<  mCombID_DeltaPhi0Cut << endreq;
  log << MSG::INFO << "CombID_DeltaEtaCut    : " << mCombID_DeltaEtaCut << endreq;
  
  log << MSG::INFO << "CombinedIDHalfTracksHypo initialized successfully" << endreq;

  return HLT::OK;
}


HLT::ErrorCode CombinedIDHalfTracksHypo::hltFinalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize()" << endreq;
  return HLT::OK;
}


HLT::ErrorCode CombinedIDHalfTracksHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
  MsgStream log(msgSvc(), name());
  // bool status;
  int output_level = msgSvc()->outputLevel();
  bool passed = false;


  if (output_level <= MSG::DEBUG) {
    log << MSG::DEBUG << " in  hltExecute "<< endreq;
  }


  //Clear Event
  mHistIDdPhi_Cut.clear();
  mHistIDdEta_Cut.clear();

  //Retrieve vector of combined objects
  // const DataVector<CombinedIDHalfTracksObject>* combID =0;   
  std::vector<const CombinedIDHalfTracksObjectCollection*> combIDs; 

   HLT::ErrorCode status2 = getFeatures(inputTE, combIDs);
   if(status2 != HLT::OK) 
     {
       msg() << MSG::DEBUG << " Cannot get CombinedIDHalfTracksObjects" << endreq;
       return status2;
     } 
   else 
     {

       msg() << MSG::VERBOSE << "execHLTAlgorithm: got vector Of CombinedIDHalfTracksObjects of size "<<
	 combIDs.size()<< endreq;
       if (combIDs.size()==0) return HLT::OK;

       const CombinedIDHalfTracksObjectCollection* combID = combIDs[0];
      

       //DataVector<CombinedIDHalfTracksObject>::const_iterator idtracks;
       CombinedIDHalfTracksObjectCollection::const_iterator idtracks; // =combID.begin();
       


       int n_idtracks=0;
       for (idtracks=combID->begin(); idtracks!=combID->end(); ++idtracks) {
       
	 log << MSG::DEBUG << " track = "<< n_idtracks <<" delta_phi0 "<<(*idtracks)->delta_phi0() << " deltaEta "<<(*idtracks)->delta_eta()<< endreq; 
	 	 
	 if ( fabs((*idtracks)->delta_phi0()-M_PI) <  mCombID_DeltaPhi0Cut &&
	      fabs((*idtracks)->delta_eta()) < mCombID_DeltaEtaCut){
	   
	   mHistIDdPhi_Cut.push_back((*idtracks)->delta_phi0());
	   mHistIDdEta_Cut.push_back((*idtracks)->delta_eta());
	   
	   n_idtracks ++; 
	 }

        }
       
      
       
       if (n_idtracks>=mCombIDNtracks) {
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

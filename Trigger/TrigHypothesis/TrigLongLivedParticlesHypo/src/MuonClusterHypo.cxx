/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// MuonClusterHypo  
// (see header for history/etc..)
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "TrigLongLivedParticlesHypo/MuonClusterHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MuonClusterHypo::MuonClusterHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){

  //default monitored values are unphysical
  //events failing cuts will show up in monitoring with these values
  //note that the "cut" for background trigger is nJet,nTrk ==-1, but
  //the actual number of jets and tracks in dR cone is monitored
	
  mCluNum = 0;
  mNumJet = -1;
  mNumTrk = -1;

  declareMonitoredVariable("NumClu", mCluNum);
  declareMonitoredVariable("NumJet", mNumJet);
  declareMonitoredVariable("NumTrk", mNumTrk);


  declareProperty("AcceptAll", m_acceptAll=false);
  declareProperty("nRoIEndCap", m_nRoIEndCap=4);   
  declareProperty("nRoIBarrel", m_nRoIBarrel=3);   
  declareProperty("nEta", m_nEta=1.0);   
  declareProperty("nJet", m_nJet=0);   
  declareProperty("nTrk", m_nTrk=0);   
}

MuonClusterHypo::~MuonClusterHypo(){
}

HLT::ErrorCode MuonClusterHypo::hltInitialize(){

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "in initialize()" << endreq;
  }

  if(m_acceptAll) {
   if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG
            << "Accepting all the events with not cut!"
            << endreq;
   }
  } else {
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG
            << "Selection of muon RoI clusters in |eta|< " << m_nEta << " with number of RoI >= " << m_nRoIBarrel << " in Barrel and >= " <<m_nRoIEndCap << " in EndCap "  
            << " and " << m_nJet << " Jets with Log(H/E)<1 and " << m_nTrk << " Tracks in ID" 
            << endreq; 
    }
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG 
          << "Initialization completed successfully" 
          << endreq;
  }
 
  return HLT::OK;
}

HLT::ErrorCode MuonClusterHypo::hltFinalize(){

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "in finalize()" << endreq;
  }

  return HLT::OK;

}

HLT::ErrorCode MuonClusterHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  // Retrieve store.
  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "in execute()" << endreq;
  }

  m_cutCounter = -1;

  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG 
	    << "Accept property is set: taking all the events"
	    << endreq;
    }
    return HLT::OK;

  } else {

     bool result = true; //set result to true in case of errors for safety

     // Some debug output:
     if(msgLvl() <= MSG::DEBUG) {
       msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
     }

     // Get TrigMuonClusterFeature linked to the outputTE 
     const TrigMuonClusterFeature* MuonClust(0);
     HLT::ErrorCode status = getFeature(outputTE,  MuonClust);
     if(status!=HLT::OK) {
        if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "no TrigMuonClusterFeature found" << endreq;
        }
        return status;
     } else {
       if(msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG << "TrigMuonClusterFeature retrieved" << endreq;
       }
     }

     //Requirements
     int numberRoI = MuonClust->getNRoi();
     int numberJet = MuonClust->getNJet();
     int numberTrk = MuonClust->getNTRK();
     double etaClust = MuonClust->getEta();
     double phiClust = MuonClust->getPhi();

     if(msgLvl() <= MSG::DEBUG) {
       msg() << MSG::DEBUG
             << "Cluster has " << numberRoI << " muon RoIs, at eta=" << etaClust 
	     << " and phi=" << phiClust << " and matches " << numberJet << " Jets with Log(H/E)" 
	     << " and " << numberTrk << " tracks in ID" << endreq;
      }

     // Cut on number of RoIs in the cluster
     if(fabs(etaClust)<=m_nEta) {

      //background trigger has cut of to m_nJet == -1, so bypass jet and track cuts if background trigger
      if((numberJet==m_nJet && m_nJet!=-1) || (m_nJet==-1)) {
    	  
	   if((numberTrk==m_nTrk && m_nTrk!=-1) || (m_nTrk==-1)) {

	     if(fabs(etaClust)<=1.0 && numberRoI >= m_nRoIBarrel) {

	       if(msgLvl() <= MSG::DEBUG) {
	         msg() << MSG::DEBUG
		       << "Cluster passes barrel selection cuts of > " << m_nRoIBarrel << " RoIs and " 
		       << m_nJet << " Jets with Log(H/E)<1 and " << m_nTrk << " tracks in ID... event accepted"
		       << endreq;
	       }
	       
	       // monitored variables
	       mCluNum = numberRoI;
	       mNumJet = numberJet;
	       mNumTrk = numberTrk; //set these equal to actual cluster parameters 
	       result = true;

	     } else if((fabs(etaClust)>=1.0 && fabs(etaClust)<=2.5) && numberRoI >= m_nRoIEndCap) {

	         if(msgLvl() <= MSG::DEBUG) {
	           msg() << MSG::DEBUG
	  		       << "Cluster passes endcap selection cuts of > " << m_nRoIEndCap << " RoIs and " 
	  		       << m_nJet << " Jets with Log(H/E)<1 and " << m_nTrk << " tracks in ID... event accepted"
	  		       << endreq;
	       }
	     
	       // monitored variables
	       mCluNum = numberRoI;
	       mNumJet = numberJet;
	       mNumTrk = numberTrk; //set these equal to actual cluster parameters
	       result = true;

	     } else {
	       if(msgLvl() <= MSG::DEBUG) {
	         msg() << MSG::DEBUG
		       << "Cluster does not satisfy all the conditions... event not accepted"
		       << endreq;
	       }
	       result = false;
	     }
	   } else {
	       if(msgLvl() <= MSG::DEBUG) {
	         msg() << MSG::DEBUG
		       << "Cluster does not satisfy all the conditions... event not accepted"
		       << endreq;
	       }
	       result = false;		   
	   }
      } else {
	       if(msgLvl() <= MSG::DEBUG) {
	         msg() << MSG::DEBUG
		       << "Cluster does not satisfy all the conditions... event not accepted"
		       << endreq;
	       }
	       result = false;	    	  
      }
     } else {
       if(msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG
               << "Cluster does not satisfy all the conditions... event not accepted"
               << endreq;
       } 
       result = false;
     } 
     
     //Store result
     pass = result;
     
     return HLT::OK;
  }
}

// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2DimuonMassHypo.cxx
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after 
 **                electron finding algorithms: should create TrigZee
 **
 **   Author: C.Schiavi <Carlo.Schiavi@ge.infn.it>
 **
 **   Created:   Jul 26 2006
 **   Modified:  
 **                
 **************************************************************************/ 

#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonHypo/TrigL2DimuonMassHypo.h"

// standard stuff
#include <map>
#include <cmath> 
// trigger EDM
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class ISvcLocator;

TrigL2DimuonMassHypo::TrigL2DimuonMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HypoAlgo(name, pSvcLocator)
{
  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=70000.0);
  declareProperty("UpperMassCut", m_upperMassCut=110000.0);

  m_storeGate = 0;
}

TrigL2DimuonMassHypo::~TrigL2DimuonMassHypo()
{ }

HLT::ErrorCode TrigL2DimuonMassHypo::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	  << (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "OppositeCharge       = " 
	  << (m_oppositeCharge==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2DimuonMassHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigL2DimuonMassHypo::hltExecute(const HLT::TriggerElement* outputTE, 
                                      bool& pass)
{
  if(msgLvl() <= MSG::DEBUG) {
    if (m_acceptAll) {
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endreq;
    } else {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	    << endreq;
    }
  }
  
  // Accept-All mode: temporary patch; should be done with force-accept
  if(m_acceptAll) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG 
	        << "Accept property is set: taking all the events"
		<< endreq;
      }
      return HLT::OK;
  }
  
  
  // retrieve pointer to StoreGate
  m_storeGate = store();


  // get input TE: use getVectorOfObjects() as only last step matters
  std::vector<HLT::TriggerElement*> vectorOfInputTE;
  vectorOfInputTE = HLT::Navigation::getDirectPredecessors(outputTE);
  
  // dump and check number of input TEs
  // it must be fixed to two for the moment - this block of code must be removed when moving to the new steering
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Retrieved " << vectorOfInputTE.size() << " input trigger Elements" << endreq;
  }
  
  if(vectorOfInputTE.size()!=2) {
    if(msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Retrieved the wrong number of input trigger elements: found "
	    << vectorOfInputTE.size() << " and expected 2" << endreq;
    }
    return HLT::NAV_ERROR;
  }
  
  // get RoI descriptors and corresponding combined muon candidates
  std::map<const TrigRoiDescriptor*, std::vector<const CombinedMuonFeature*> > muonCandidates;
  std::vector<HLT::TriggerElement*>::iterator inputTE, inputTEEnd=vectorOfInputTE.end();
  for(inputTE=vectorOfInputTE.begin(); inputTE!=inputTEEnd; inputTE++) {

    // get RoI descriptor
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode status = getFeature(static_cast<const HLT::TriggerElement*>(*inputTE), roiDescriptor);
    
    if(roiDescriptor) {
      if(msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << "RoI id " << roiDescriptor->roiId()
	      << " LVL1 id " << roiDescriptor->l1Id()
	      << " located at   phi = " <<  roiDescriptor->phi()
	      << ", eta = " << roiDescriptor->eta() << endreq;
      }
    } else {
      if(msgLvl() <= MSG::WARNING) {
	msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
      }
      return HLT::NAV_ERROR;
    }
    
    // retrieve Combined Muon Features from the inputTE: must retrieve vector first
    std::vector<const CombinedMuonFeature*> vectorOfMuons;
    status = getFeatures(*inputTE,vectorOfMuons);
    if(status != HLT::OK) {
      if(msgLvl() <= MSG::WARNING) {
	msg() << MSG::WARNING << "Failed to get CombinedMuonFeature" << endreq;
      }
      return HLT::NAV_ERROR;
    }
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Got vector with " << vectorOfMuons.size() 
	    << " CombinedMuonFeature" << endreq;
    }

    // add an entry to the muonCandidates map
    muonCandidates.insert(std::make_pair(roiDescriptor,vectorOfMuons));
  }

  // perform combinations
  std::map<const TrigRoiDescriptor*, std::vector<const CombinedMuonFeature*> >::iterator muons1, muons2, muonsEnd=muonCandidates.end();
  // combine RoIs
  for(muons1=muonCandidates.begin(); muons1!=muonsEnd; muons1++) {
    for(muons2=muons1, muons2++; muons2!=muonsEnd; muons2++) {
      // combine muons from the two RoIs
      std::vector<const CombinedMuonFeature*>::iterator muon1, muon1End=(muons1->second).end();
      std::vector<const CombinedMuonFeature*>::iterator muon2, muon2End=(muons2->second).end();
      for(muon1=(muons1->second).begin(); muon1!=muon1End; muon1++) {
	for(muon2=(muons2->second).begin(); muon2!=muon2End; muon2++) {
	  // debug dump
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "1st CombinedMuonFeature: addr=" << (*muon1)
		  << " pt=" << 1/(*muon1)->pt() << " trkAddr=" << (*muon1)->IDTrack() 
		  << endreq;
	    msg() << MSG::DEBUG << "2nd CombinedMuonFeature: addr=" << (*muon2)
		  << " pt=" << 1/(*muon2)->pt() << " trkAddr=" << (*muon2)->IDTrack() 
		  << endreq;
	  }
	  // check charge
	  if(m_oppositeCharge && ((*muon1)->pt()*(*muon2)->pt())>0) {
	    if(msgLvl() <= MSG::DEBUG) {
	      msg() << MSG::DEBUG << "Combination discarded by opposite charge check" << endreq;
	    }
	    continue;
	  }
	  // evaluate mass
	  double mass = invariantMass(*muon1, *muon2);
	  // apply cut on mass
	  if(mass<m_lowerMassCut || mass>m_upperMassCut) {
	    if(msgLvl() <= MSG::DEBUG) {
	      msg() << MSG::DEBUG << "Combination discarded by mass cut: "
		    << mass << " MeV" << endreq;
	    }
	    continue;
	  }
	  // save combination
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Combination retained! Invariant mass value: "
		  << mass/1000 << " GeV" << endreq;
	  }
	}
      }
    }
  }

  return HLT::OK;
}



double TrigL2DimuonMassHypo::invariantMass(const CombinedMuonFeature* muon1, const CombinedMuonFeature* muon2) {

  // check pointers
  if(!(muon1->IDTrack()) || !(muon2->IDTrack())) return 0;

  // get parameters
  double eta1 = muon1->IDTrack()->param()->eta();
  double eta2 = muon2->IDTrack()->param()->eta();
  double Pt1  = fabs(1/muon1->pt());
  double Pt2  = fabs(1/muon2->pt());
  double ph1  = muon1->IDTrack()->param()->phi0();
  double ph2  = muon2->IDTrack()->param()->phi0();
  
  double the1 = 2*atan2((double)exp(-eta1),1.);
  double the2 = 2*atan2((double)exp(-eta2),1.);
  double P1   = fabs(Pt1)/sin(the1);
  double P2   = fabs(Pt2)/sin(the2);
  double m1   = 105.6;
  double m2   = 105.6;
  
  double p1[3],p2[3];
  p1[0]  = Pt1*cos(ph1);
  p1[1]  = Pt1*sin(ph1);
  p1[2]  = P1*cos(the1);
  p2[0]  = Pt2*cos(ph2);
  p2[1]  = Pt2*sin(ph2);
  p2[2]  = P2*cos(the2);

  // evaluate mass
  double Ptot1 = sqrt(std::pow(p1[0],2)+std::pow(p1[1],2)+std::pow(p1[2],2));
  double Ptot2 = sqrt(std::pow(p2[0],2)+std::pow(p2[1],2)+std::pow(p2[2],2));
  double e1 = sqrt(Ptot1*Ptot1+m1*m1);
  double e2 = sqrt(Ptot2*Ptot2+m2*m2);
  double mass = sqrt(m1*m1 + m2*m2 + 2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]); 

  return mass;
}

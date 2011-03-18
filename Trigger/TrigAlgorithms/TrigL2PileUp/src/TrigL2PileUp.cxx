/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2PileUp.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigL2PileUp
//
// AUTHOR:   Alex Pinder, George Lewis
// CREATED:  July 16, 2010
//
// Description: L2 Pile Up Algorithm
// *******************************************************************


#include "TrigL2PileUp/TrigL2PileUp.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

#include "TrigNavigation/Navigation.h"


TrigL2PileUp::TrigL2PileUp(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::AllTEAlgo(name, pSvcLocator) {

  declareProperty("PileUpLabel", m_featureLabel, "Label for the pileup feature in the HLT navigation");

}


HLT::ErrorCode TrigL2PileUp::hltInitialize(){

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "called L2PileUp::hltInitialize()" << endreq;
  }
  
  return HLT::OK;
}

HLT::ErrorCode TrigL2PileUp::hltFinalize(){

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "called L2PileUp::hltFinalize()" << endreq;
  }
  
  return HLT::OK;
}



HLT::ErrorCode TrigL2PileUp::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out) {

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "called TrigL2PileUp::hltExecute()" << endreq;
    msg() << MSG::DEBUG << "Size of tes_in = " << tes_in.size() << endreq;
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    for(unsigned int i=0; i<tes_in.size(); i++) { 
      msg() << MSG::DEBUG << "Size of tes_in[" << i << "] = " << tes_in.at(i).size() << endreq;
    }
  }

  if(tes_in.size() < 1) {
    msg() << MSG::WARNING << "Found " << tes_in.size() << " input trigger elements. Need at least one." << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::BAD_JOB_SETUP);
  }


  // Create output TE:

  HLT::TEVec allTEs;
  if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] ); // Getting MET Trigger Element
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out); //new HLT::TriggerElement;
  outputTE->setActiveState(true);
  

  // Get input FJ Features and attach to output feature:

  m_feature = new TrigPileUpInfo(); // Create new pileup feature
  const LVL1::RecJetRoI* tmpJetRoI; // Dummy FJ object
  HLT::ErrorCode status;
  
  for(unsigned int i=0; i<tes_in.at(0).size(); i++) {
      
    tmpJetRoI = 0; // Reset dummy RoI
    
    status = getFeature(tes_in.at(0).at(i), tmpJetRoI);
    
    if ( status != HLT::OK || !tmpJetRoI ) {
      msg() << MSG::WARNING // ERROR
	    << "Failed to get feature from "
	    << "input trigger element: "
	    << "[0][" <<  i << "]"
	    << endreq;
    } 
    else {
      std::vector<unsigned int> *thrPassed = tmpJetRoI->thresholdsPassed();
      int FJthr = thrPassed->back(); // Get maximum threshold passed by the FJ
      m_feature->addFJ( FJthr ); // Add the FJ threshold to the feature
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Adding FJ with max threshold of: " << FJthr << endreq;
      }
      delete thrPassed;
    }
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    if(m_feature->numFJ()<1) {
      msg() << MSG::DEBUG << "No L1 forward jet RoIs found" << endreq;
    }
  }


  // Get input vertex features and attach to output feature:

  const TrigVertexCollection* vertexColl = 0;
  const TrackInVertexList* vertexTracks = 0;  
  int numTracks = 0;

  if(tes_in.size() < 2) {
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << "No L2 vertex input found - continuing" << endreq;
    }
  }
  else {
    if(tes_in[1].size()!=1) msg() << MSG::WARNING << "Number of vertex collections != 1" << endreq;
    status = getFeature(tes_in[1][0], vertexColl);
    
    if ( status != HLT::OK || !vertexColl ) {
      msg() << MSG::WARNING
	    << "Failed to get feature from primary vertex TE" << endreq;
    }
    else{
      
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Size of Vertex Collection = " << vertexColl->size() << endreq;
      }    
      for(unsigned int i = 0; i < vertexColl->size(); i++){ 
	
	vertexTracks = vertexColl->at(i)->tracks();
	if(!vertexTracks){
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << " Could not find vector of tracks for vertex - setting numTracks to 0" << endreq;           
	  }
	  numTracks = 0;
	  m_feature->addPV( numTracks);  // add number of tracks to the feature
	}  
	else{	  
	  numTracks = vertexTracks->size();
	  m_feature->addPV( numTracks);  // add number of tracks to the feature
	}
      }  
    }
  }


  // Attach pileup feature to the output TE

  status = attachFeature(outputTE, m_feature, m_featureLabel);

  if ( status != HLT::OK ) {
    msg() << MSG::WARNING
	  << "Write of TrigPileUpInfo feature into outputTE failed"
	  << endreq;
    return status;
  }


  return HLT::OK;
}

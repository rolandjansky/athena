/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// RoIPropertyNtupleTool.cxx
//   Source file for class RoIPropertyNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka -at- cern.ch
///////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigTrkValTools/RoIPropertyNtupleTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// constructor
Trk::RoIPropertyNtupleTool::RoIPropertyNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_runNumber(0),
  m_eventNumber(0),
  m_eta(0.),
  m_phi(0.)
{
  declareInterface<IRoIPropertyNtupleTool>(this);
}

// destructor
Trk::RoIPropertyNtupleTool::~RoIPropertyNtupleTool() {}

StatusCode Trk::RoIPropertyNtupleTool::initialize() {
  ATH_MSG_DEBUG ("initialise successful in "<<name());
  return AthAlgTool::initialize();
}

StatusCode Trk::RoIPropertyNtupleTool::finalize() {
  ATH_MSG_DEBUG ("finalize successfull in "<<name());
  return StatusCode::SUCCESS;
}


StatusCode Trk::RoIPropertyNtupleTool::addNtupleItems( TTree* tree, const std::string ) const {
  if (!tree) return StatusCode::FAILURE;
  //-----------------
  // add items
  tree->Branch("RunNumber",   &m_runNumber,   "RunNumber/I");
  tree->Branch("EventNumber", &m_eventNumber, "EventNumber/I");
  tree->Branch("Eta", &m_eta, "Eta/F");
  tree->Branch("Phi", &m_phi, "Phi/F");
  tree->Branch("RoIID", &m_roiID, "RoIID/F");
  tree->Branch("isPassedTrigChain", &m_isPassedTrigChain, "isPassedTrigChain/b");
  return StatusCode::SUCCESS;
}

StatusCode Trk::RoIPropertyNtupleTool::fillRoIData( const TrigRoiDescriptor* roi, bool isPassedTrigChain) const {
  if(!roi) {
            msg(MSG::ERROR) << "RoI pointer points to 0. No data recieved." << endreq;
            return StatusCode::FAILURE;
           }
  
  const EventInfo* eventInfo;
  if ((evtStore()->retrieve(eventInfo)).isFailure()) {
    msg(MSG::WARNING) << "Could not retrieve event info" << endreq;
    m_runNumber   = -999;
    m_eventNumber = -999;
    return StatusCode::SUCCESS;
  } else {

    EventID* myEventID=eventInfo->event_ID();
    m_runNumber   = myEventID->run_number();
    m_eventNumber = myEventID->event_number();
  }
  
  m_eta =  	(float)roi->eta();
  m_phi =  	(float)roi->phi();
  m_roiID =	(float)roi->roiId();
  m_isPassedTrigChain = isPassedTrigChain;
  
  msg(MSG::VERBOSE) << "RoI ID: "<< m_roiID <<" Eta:  " << m_eta  <<" Phi: " << m_phi <<endreq;
  return StatusCode::SUCCESS;

  // remember calling algo is the one to record the content on the tree.
}

StatusCode Trk::RoIPropertyNtupleTool::resetVariables( ) const {
  m_eventNumber = 0;
  m_runNumber   = 0;
  m_eta = 0.;
  m_phi = 0.;
  m_roiID =0;
  m_isPassedTrigChain = false;
  return StatusCode::SUCCESS;
}


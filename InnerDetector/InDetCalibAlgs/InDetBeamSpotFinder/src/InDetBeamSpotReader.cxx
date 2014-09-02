/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotReader.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
//#include "TrkEventPrimitives/VertexType.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"



InDet::InDetBeamSpotReader::InDetBeamSpotReader(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_toolSvc("ToolSvc",name),
  m_beamSpotSvc("BeamCondSvc",name)
{  
  declareProperty("ToolSvc", m_toolSvc);
  declareProperty("BeamCondSvc", m_beamSpotSvc);
}




StatusCode InDet::InDetBeamSpotReader::initialize() {
  msg(MSG::DEBUG) << "in initialize()" << endreq;
  
 
  if ( m_toolSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_toolSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_toolSvc << endreq;


  if ( m_beamSpotSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_beamSpotSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_beamSpotSvc << endreq;
  
  
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotReader::execute(){
 
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in execute()" << endreq;

  //get the set of 
  const DataHandle<EventInfo> eventInfo;
  if (StatusCode::SUCCESS != evtStore()->retrieve( eventInfo ) ){
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot get event info." << endreq;
    return StatusCode::FAILURE;
  }
  EventID* eventID = eventInfo->event_ID();
  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "In event " << (*eventID) << endreq;
    
    msg(MSG::INFO) <<"BeamSpot Position: \n "
		   << m_beamSpotSvc->beamPos() << endreq;
    msg(MSG::INFO) <<"BeamSpot Sigma\n\t"
		   << m_beamSpotSvc->beamSigma(0) << "\n\t"
		   << m_beamSpotSvc->beamSigma(1) << "\n\t"
		   << m_beamSpotSvc->beamSigma(2) << "\n\t" 
		   << endreq;
    msg(MSG::INFO) <<"BeamSpot Tilt\n\t"
		   << m_beamSpotSvc->beamTilt(0) << "\n\t"
		   << m_beamSpotSvc->beamTilt(1) << "\n\t"
		   << endreq;
    msg(MSG::INFO) <<"Beamspot position at PV z-position" << endreq;
  }

  const VxContainer* importedVxContainer =0;
  static const std::string m_containerName = "VxPrimaryCandidate";
  if ( StatusCode::SUCCESS != evtStore()->retrieve(importedVxContainer,m_containerName)){
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "No " << m_containerName << " found in StoreGate" << endreq;
    return StatusCode::FAILURE;
  }
  //get list of PVs
  VxContainer::const_iterator vtxItr;
  for(vtxItr=importedVxContainer->begin(); 
      vtxItr!=importedVxContainer->end(); ++vtxItr) {
    if (static_cast<int>((*vtxItr)->vxTrackAtVertex()->size())==0) continue;
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) <<"PV position:  " 
				 << (*vtxItr)->recVertex().position()<< endreq;
    double z = (*vtxItr)->recVertex().position().z();
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) <<"\n\t"
	  << m_beamSpotSvc->beamPos()(0) 
      + (z - m_beamSpotSvc->beamPos()(2))
      *m_beamSpotSvc->beamTilt(0) << "\n\t"
	  << m_beamSpotSvc->beamPos()(1)
      + (z - m_beamSpotSvc->beamPos()(2))
      *m_beamSpotSvc->beamTilt(1) << endreq;
  }
  
  return StatusCode::SUCCESS;
}



StatusCode InDet::InDetBeamSpotReader::finalize() {
  msg(MSG::DEBUG) << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}

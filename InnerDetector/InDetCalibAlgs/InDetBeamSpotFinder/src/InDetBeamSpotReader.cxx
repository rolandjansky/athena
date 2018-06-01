/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotReader.h"

#include "VxVertex/VxCandidate.h"
//#include "TrkEventPrimitives/VertexType.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "GaudiKernel/ToolHandle.h"

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
  ATH_MSG_DEBUG( "in initialize()" );

  ATH_CHECK( m_toolSvc.retrieve() );
  ATH_CHECK( m_beamSpotSvc.retrieve() );

  ATH_CHECK( m_eventInfo.initialize() );
  ATH_CHECK( m_vxContainer.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotReader::execute(){
  ATH_MSG_DEBUG( "in execute()");

  //get the set of 
  SG::ReadHandle<EventInfo> eventInfo(m_eventInfo);
  EventID* eventID = eventInfo->event_ID();
    ATH_MSG_INFO( "In event " << (*eventID) );
    ATH_MSG_INFO("BeamSpot Position: \n "
		   << m_beamSpotSvc->beamPos() );
    ATH_MSG_INFO("BeamSpot Sigma\n\t"
		   << m_beamSpotSvc->beamSigma(0) << "\n\t"
		   << m_beamSpotSvc->beamSigma(1) << "\n\t"
		   << m_beamSpotSvc->beamSigma(2) << "\n\t");
    ATH_MSG_INFO("BeamSpot Tilt\n\t"
		   << m_beamSpotSvc->beamTilt(0) << "\n\t"
		   << m_beamSpotSvc->beamTilt(1) << "\n\t");
    ATH_MSG_INFO("Beamspot position at PV z-position");

  //get list of PVs
  SG::ReadHandle<VxContainer> importedVxContainer(m_vxContainer);
  VxContainer::const_iterator vtxItr;
  for(vtxItr=importedVxContainer->begin();
      vtxItr!=importedVxContainer->end(); ++vtxItr) {
    if (static_cast<int>((*vtxItr)->vxTrackAtVertex()->size())==0) continue;
    if (msgLvl(MSG::INFO)) ATH_MSG_INFO("PV position:  "
				 << (*vtxItr)->recVertex().position() );
    double z = (*vtxItr)->recVertex().position().z();
    if (msgLvl(MSG::INFO)) ATH_MSG_INFO("\n\t"
	  << m_beamSpotSvc->beamPos()(0)
      + (z - m_beamSpotSvc->beamPos()(2))
      *m_beamSpotSvc->beamTilt(0) << "\n\t"
	  << m_beamSpotSvc->beamPos()(1)
      + (z - m_beamSpotSvc->beamPos()(2))
      *m_beamSpotSvc->beamTilt(1) );
  }

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotReader::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );

  return StatusCode::SUCCESS;
}

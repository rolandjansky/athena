/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotReader.h"
#include "VxVertex/VxCandidate.h"

InDet::InDetBeamSpotReader::InDetBeamSpotReader(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode InDet::InDetBeamSpotReader::initialize() {
  ATH_MSG_DEBUG( "in initialize()" );

  ATH_CHECK( m_beamSpotKey.initialize() );

  ATH_CHECK( m_eventInfo.initialize() );
  ATH_CHECK( m_vxContainer.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotReader::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG( "in execute()");

  //get the set of 
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo, ctx);
    ATH_MSG_INFO( "In event " << (*eventInfo) );
    ATH_MSG_INFO("BeamSpot Position: \n "
		   << beamSpotHandle->beamPos() );
    ATH_MSG_INFO("BeamSpot Sigma\n\t"
		   << beamSpotHandle->beamSigma(0) << "\n\t"
		   << beamSpotHandle->beamSigma(1) << "\n\t"
		   << beamSpotHandle->beamSigma(2) << "\n\t");
    ATH_MSG_INFO("BeamSpot Tilt\n\t"
		   << beamSpotHandle->beamTilt(0) << "\n\t"
		   << beamSpotHandle->beamTilt(1) << "\n\t");
    ATH_MSG_INFO("Beamspot position at PV z-position");

  //get list of PVs
  SG::ReadHandle<VxContainer> importedVxContainer(m_vxContainer, ctx);
  VxContainer::const_iterator vtxItr;
  for(vtxItr=importedVxContainer->begin();
      vtxItr!=importedVxContainer->end(); ++vtxItr) {
    if (static_cast<int>((*vtxItr)->vxTrackAtVertex()->size())==0) continue;
    if (msgLvl(MSG::INFO)) ATH_MSG_INFO("PV position:  "
				 << (*vtxItr)->recVertex().position() );
    double z = (*vtxItr)->recVertex().position().z();
    if (msgLvl(MSG::INFO)) ATH_MSG_INFO("\n\t"
	  << beamSpotHandle->beamPos()(0)
      + (z - beamSpotHandle->beamPos()(2))
      *beamSpotHandle->beamTilt(0) << "\n\t"
	  << beamSpotHandle->beamPos()(1)
      + (z - beamSpotHandle->beamPos()(2))
      *beamSpotHandle->beamTilt(1) );
  }

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotReader::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );

  return StatusCode::SUCCESS;
}

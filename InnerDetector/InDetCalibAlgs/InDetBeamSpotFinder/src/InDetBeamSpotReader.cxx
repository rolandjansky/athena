/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK( m_vxContainer.initialize(!m_vxContainer.empty()) );

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBeamSpotReader::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG( "in execute()");

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  ATH_MSG_INFO( "In event " << ctx.eventID() );
  ATH_MSG_INFO("BeamSpot Position: "
               << beamSpotHandle->beamPos()[0] << " "
               << beamSpotHandle->beamPos()[1] << " "
               << beamSpotHandle->beamPos()[2]
               <<", Sigma: "
               << beamSpotHandle->beamSigma(0) << " "
               << beamSpotHandle->beamSigma(1) << " "
               << beamSpotHandle->beamSigma(2)
               << ", Tilt: "
               << beamSpotHandle->beamTilt(0) << " "
               << beamSpotHandle->beamTilt(1)
               << ", Status: "
               << beamSpotHandle->beamStatus());

  //get list of PVs
  if (!m_vxContainer.empty()) {
    ATH_MSG_INFO("Beamspot position at PV z-position");
    SG::ReadHandle<VxContainer> importedVxContainer(m_vxContainer, ctx);
    for (const Trk::VxCandidate* vtx : *importedVxContainer) {
      if (static_cast<int>(vtx->vxTrackAtVertex()->size())==0) continue;
      ATH_MSG_INFO("PV position: " << vtx->recVertex().position() );
      double z = vtx->recVertex().position().z();
      ATH_MSG_INFO("\n\t"
                   << beamSpotHandle->beamPos()(0)
                   + (z - beamSpotHandle->beamPos()(2))
                   *beamSpotHandle->beamTilt(0) << "\n\t"
                   << beamSpotHandle->beamPos()(1)
                   + (z - beamSpotHandle->beamPos()(2))
                   *beamSpotHandle->beamTilt(1) );
    }
  }

  return StatusCode::SUCCESS;
}

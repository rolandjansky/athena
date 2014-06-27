/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleParametersAtDiPhotonVertex.h"

#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"

namespace D3PD {


TrackParticleParametersAtDiPhotonVertex::TrackParticleParametersAtDiPhotonVertex (const std::string& type,
                      const std::string& name,
                      const IInterface* parent)
  : D3PD::BlockFillerTool<Rec::TrackParticle> (type, name, parent),
    m_trackToVertexTool("Reco::TrackToVertex")
{
  // Avoid coverity warnings.
  book().ignore();

  declareProperty ("DefaultValue", m_defaultValue=0,
                   "Default value to which to set unfilled variables.");
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
}

StatusCode TrackParticleParametersAtDiPhotonVertex::initialize(){

  CHECK( D3PD::BlockFillerTool<Rec::TrackParticle>::initialize() );

  StatusCode sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()){
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackParticleParametersAtDiPhotonVertex::book()
{
  // track parameters at perigee
  CHECK( addVariable ("d0_wrtDiPhotonPV", m_d0, "", m_defaultValue) );
  CHECK( addVariable ("z0_wrtDiPhotonPV", m_z0, "", m_defaultValue) );
  CHECK( addVariable ("phi_wrtDiPhotonPV", m_phi, "", m_defaultValue) );

  return StatusCode::SUCCESS;
}

StatusCode TrackParticleParametersAtDiPhotonVertex::fill (const Rec::TrackParticle& track)
{  
  const VxContainer* vxTES = 0;
  CHECK( evtStore()->retrieve( vxTES , "PAUDiphotonVertexCollection" ));  
  const Trk::VxCandidate* vxcand = vxTES->at(0);
  if (!vxcand) {
    REPORT_MESSAGE (MSG::DEBUG) << "Diphoton vertex is NULL";
    return StatusCode::SUCCESS;
  }
  const Trk::RecVertex* theVertex = &(vxcand->recVertex());

  if(theVertex) {
    const Trk::Perigee *testPerigee = 0;
    testPerigee = m_trackToVertexTool->perigeeAtVertex(track,theVertex->position());
    if ( testPerigee ) {
      *m_z0 = testPerigee->parameters()[Trk::z0];
      *m_d0 = testPerigee->parameters()[Trk::d0];
      *m_phi = testPerigee->parameters()[Trk::phi0];
      delete testPerigee;
      testPerigee = 0;
    } else {
      REPORT_MESSAGE (MSG::DEBUG) << "Failed to get parameters wrt primary vertex";
      return StatusCode::SUCCESS;
    }
  } else {
    REPORT_MESSAGE (MSG::DEBUG) << "Failed to retrieve the primary vertex";
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

} // namespace D3PD

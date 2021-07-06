/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticlePerigeeAtBSAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticlePerigeeAtBSAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its perigee at the beam spot.
 */

#include "TrackParticlePerigeeAtBSAssociationTool.h"

#include "Particle/TrackParticle.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticlePerigeeAtBSAssociationTool::TrackParticlePerigeeAtBSAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_trackToVertexTool("Reco::TrackToVertex")
{
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
}

StatusCode TrackParticlePerigeeAtBSAssociationTool::initialize(){

  CHECK( Base::initialize() );

  ATH_CHECK(m_beamSpotKey.initialize());

  // Pick up the TrackToVertex tool
  StatusCode sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()){
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackParticlePerigeeAtBSAssociationTool::get (const Rec::TrackParticle& track)
{
  // Protect against bad tracks.
  if (track.measuredPerigee()->covariance() && track.measuredPerigee()->covariance()->rows()==0)
    return 0;
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  /// Pick up the beamspot
  Amg::Vector3D beamSpot(0,0,0);
  beamSpot = beamSpotHandle->beamVtx().position();

  return m_trackToVertexTool->perigeeAtVertex(track, beamSpot);
}

/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackParticlePerigeeAtBSAssociationTool::get (const xAOD::TrackParticle& track)
{
  /// Pick up the beamspot
  Amg::Vector3D beamSpot(0,0,0);
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  beamSpot = beamSpotHandle->beamVtx().position();

  return m_trackToVertexTool->perigeeAtVertex(track, beamSpot);
}

/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c get().  The default implementation is a no-op,
 * but if the association dynamically allocates the object which
 * it returned, this gives it a chance to free it.
 */
void TrackParticlePerigeeAtBSAssociationTool::releaseObject (const Trk::TrackParameters* p)
{
  if(p) delete p;
}

} // namespace D3PD

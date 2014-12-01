/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticlePerigeeAtPVAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticlePerigeeAtPVAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its default Perigee.
 */

#include "TrackParticlePerigeeAtPVAssociationTool.h"

#include "Particle/TrackParticle.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/VertexContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "VxVertex/PrimaryVertexSelector.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticlePerigeeAtPVAssociationTool::TrackParticlePerigeeAtPVAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_trackToVertexTool("Reco::TrackToVertex"),
      m_resolver ("TrackParticlePerigeeAtPVAssociationTool",
                  evtStore(),
                  m_vxCandidate)
{
  declareProperty ("SGKey", m_vxCandidate = "PrimaryVertices,VxPrimaryCandidate");
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
}

StatusCode TrackParticlePerigeeAtPVAssociationTool::initialize(){

  CHECK( Base::initialize() );
  CHECK( m_trackToVertexTool.retrieve() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackParticlePerigeeAtPVAssociationTool::get (const Rec::TrackParticle& track)
{
  m_resolver.initialize<VxContainer>().ignore();
  const VxContainer* vxContainer = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_resolver.key());
  if (sc.isFailure() || !vxContainer) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex container: " << m_vxCandidate;
    return 0;
  }

  if(vxContainer->size()<1){
    REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
    return 0;
  }

  if (track.measuredPerigee()->covariance() && track.measuredPerigee()->covariance()->rows() == 0)
  {
    REPORT_MESSAGE (MSG::WARNING) << "Bad track; can't find perigee at vertex.";
    return 0;
  }

  const Trk::VxCandidate* vxI = PrimaryVertexSelector(*vxContainer);
  if (!vxI){
      REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
      return 0;
  }
  
  return m_trackToVertexTool->perigeeAtVertex(track, vxI->recVertex().position());
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackParticlePerigeeAtPVAssociationTool::get (const xAOD::TrackParticle& track)
{
  m_resolver.initialize<xAOD::VertexContainer>().ignore();
  const xAOD::VertexContainer* vxContainer = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_resolver.key());
  if (sc.isFailure() || !vxContainer) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex container: " << m_vxCandidate;
    return 0;
  }

  if(vxContainer->size()<1){
    REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
    return 0;
  }

  for (const xAOD::Vertex* vx : *vxContainer) {
    if (vx->vertexType() == xAOD::VxType::PriVtx)
      return m_trackToVertexTool->perigeeAtVertex(track, vx->position());
  }

  REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
  return 0;
}


/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c get().  The default implementation is a no-op,
 * but since the association dynamically allocates the object which
 * it returned, this gives it a chance to free it.
 */
void TrackParticlePerigeeAtPVAssociationTool::releaseObject (const Trk::TrackParameters* p)
{
  if(p) delete p;
}

} // namespace D3PD

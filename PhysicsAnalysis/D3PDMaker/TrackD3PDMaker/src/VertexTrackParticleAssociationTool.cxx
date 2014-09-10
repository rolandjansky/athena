/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VertexTrackParticleAssociationTool.h"

#include <VxVertex/VxCandidate.h>
#include <VxVertex/VxTrackAtVertex.h>
#include <Particle/TrackParticle.h>
#include <TrkParticleBase/LinkToTrackParticleBase.h>
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


VertexTrackParticleAssociationTool::VertexTrackParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_aod_vx(0),
      m_xaod_vx(0),
      m_itrack(0)
{
  book().ignore();
}


/**
 * @brief Book variables for this block.
 */
StatusCode VertexTrackParticleAssociationTool::book()
{
  CHECK( addVariable ("weight",       m_weight) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode VertexTrackParticleAssociationTool::reset (const Trk::VxCandidate& p)
{
  m_aod_vx = &p;
  m_xaod_vx = 0;
  m_itrack = 0;
  return StatusCode::SUCCESS;
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode VertexTrackParticleAssociationTool::reset (const xAOD::Vertex& p)
{
  m_aod_vx = 0;
  m_xaod_vx = &p;
  m_itrack = 0;
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle*
VertexTrackParticleAssociationTool::next (const Trk::VxCandidate* /*dummy*/)
{
  const std::vector<Trk::VxTrackAtVertex*>* tracks = m_aod_vx->vxTrackAtVertex();

  // reached the end
  if (m_itrack >= tracks->size()) return 0;

  const Trk::VxTrackAtVertex *track = (*tracks)[m_itrack++];

  const Trk::LinkToTrackParticleBase *link = dynamic_cast<const Trk::LinkToTrackParticleBase *>(track->trackOrParticleLink());
  if(0 == link){  // it's not a link to TrackParticle
    REPORT_MESSAGE (MSG::WARNING) << "Trying to associate a TrackParticle to a vertex"
				  << " while the vertex was not fitted using TrackParticles";
    return 0;
  }

  const Trk::TrackParticleBase *particle_base = **link;
  if(0 == particle_base){
    REPORT_MESSAGE (MSG::WARNING) << "link from VxCandidate to TrackParticle is broken";
    return 0;
  }

  const Rec::TrackParticle* particle = dynamic_cast<const Rec::TrackParticle*>(particle_base);

  if(0 == particle){
    REPORT_MESSAGE (MSG::WARNING) << "dynamic cast from Trk::TrackParticleBase to Rec::TrackPArticle failed";
    return 0;
  }

  *m_weight = track->vtxCompatibility();

  return particle;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const xAOD::TrackParticle*
VertexTrackParticleAssociationTool::next (const xAOD::Vertex* /*dummy*/)
{
  // reached the end
  if (m_itrack >= m_xaod_vx->nTrackParticles()) return 0;

  *m_weight = m_xaod_vx->trackWeight(m_itrack);
  return m_xaod_vx->trackParticle(m_itrack++);
}


} // namespace D3PD

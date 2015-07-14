/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VertexTrackAssociationTool.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "Particle/TrackParticle.h"

namespace D3PD {


VertexTrackAssociationTool::VertexTrackAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_trackToVertexTool("Reco::TrackToVertex"),
      m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator")
{
  // Avoid coverity warnings.
  m_fillUnbiasedIP = true;
  book().ignore();

  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
  declareProperty ("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);
  declareProperty ("FillUnbiasedIP", m_fillUnbiasedIP = false);
}

StatusCode VertexTrackAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_trackToVertexTool.retrieve() );
  CHECK( m_trackToVertexIPEstimator.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode VertexTrackAssociationTool::book()
{

  CHECK ( addVariable ("weight", m_weight) );
  if(m_fillUnbiasedIP){
    CHECK ( addVariable ("unbiased_d0", m_unbiased_d0) );
    CHECK ( addVariable ("unbiased_z0", m_unbiased_z0) );
    CHECK ( addVariable ("err_unbiased_d0", m_err_unbiased_d0) );
    CHECK ( addVariable ("err_unbiased_z0", m_err_unbiased_z0) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode VertexTrackAssociationTool::reset (const Trk::VxCandidate& vx)
{

  m_vx = &vx;

  const std::vector<Trk::VxTrackAtVertex*>* tracks = vx.vxTrackAtVertex();

  m_trkItr = tracks->begin();
  m_trkEnd = tracks->end();

  return StatusCode::SUCCESS;
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Trk::VxTrackAtVertex* VertexTrackAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd) return 0;

  const Trk::VxTrackAtVertex *track = *m_trkItr;
  m_trkItr++;

  Trk::Perigee *perigee = getPerigeeAtVertex(track);

  *m_weight = track->weight();
  if(m_fillUnbiasedIP){
    if (perigee) {
      const Trk::ImpactParametersAndSigma* myIPandSigma = m_trackToVertexIPEstimator->estimate(perigee, m_vx, true);
      if(0==myIPandSigma) {
	      //REPORT_MESSAGE ( MSG::DEBUG) << "trackToVertexIPEstimator PV failed !";
      } else {
        *m_unbiased_d0 = myIPandSigma->IPd0;
        *m_unbiased_z0 = myIPandSigma->IPz0;
        *m_err_unbiased_d0 = myIPandSigma->sigmad0;
        *m_err_unbiased_z0 = myIPandSigma->sigmaz0;
        delete myIPandSigma;
      }
    }
  }

  Trk::VxTrackAtVertex *newTrack = new Trk::VxTrackAtVertex(*track);
  newTrack->setPerigeeAtVertex(perigee);

  return newTrack;
}

void VertexTrackAssociationTool::releaseElement (const Trk::VxTrackAtVertex* track){

  if(track) delete track;
}

Trk::Perigee *VertexTrackAssociationTool::getPerigeeAtVertex(const Trk::VxTrackAtVertex *track){

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
    REPORT_MESSAGE (MSG::WARNING) << "dynamic cast from Trk::TrackParticleBase to Rec::TrackParticle failed";
    return 0;
  }

  if (particle->measuredPerigee()->covariance() && particle->measuredPerigee()->covariance()->rows() == 0)
  {
    REPORT_MESSAGE (MSG::WARNING) << "Bad track; can't find perigee at vertex.";
    return 0;
  }

  // Trk::TrackParameters *perigee = 
  // const_cast<Trk::TrackParameters*>(m_trackToVertexTool->perigeeAtVertex(*particle, m_vx->recVertex().position()));

  //Trk::TrackParameters *perigee = m_trackToVertexTool->perigeeAtVertex(*particle, m_vx->recVertex().position());

  Trk::Perigee *perigee =  const_cast<Trk::Perigee*>(m_trackToVertexTool->perigeeAtVertex(*particle, m_vx->recVertex().position()));  

  return perigee;
}


void VertexTrackAssociationTool::clearData(){

  *m_weight = 0;

  *m_unbiased_d0 = 0;
  *m_unbiased_z0 = 0;
  *m_err_unbiased_d0 = 0;
  *m_err_unbiased_z0 = 0;
}

} // namespace D3PD

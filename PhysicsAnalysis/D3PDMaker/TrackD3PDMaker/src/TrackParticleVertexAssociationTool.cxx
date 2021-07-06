/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackParticleVertexAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Associate from a TrackParticle to its reconstructed vertex.
 */

#include "AthenaKernel/errorcheck.h"
#include "TrackParticleVertexAssociationTool.h"
#include "Particle/TrackParticle.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include <TrkParticleBase/LinkToTrackParticleBase.h>
#include <VxVertex/VxTrackAtVertex.h>
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticleVertexAssociationTool::TrackParticleVertexAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
  declareProperty ("VxSGKey", m_targetSGKey="VxPrimaryCandidate");
  declareProperty ("Fast", m_fast = false);

  book().ignore(); // Avoid coverity warnings.
}

/**
  * @brief Standard Gaudi initialize method.
  */
StatusCode TrackParticleVertexAssociationTool::initialize()
{
  ATH_CHECK( m_vertexKey.initialize() );
  return Base::initialize();
}

/**
  * @brief book ntuple variables
  */
StatusCode TrackParticleVertexAssociationTool::book(){

  if(!m_fast) CHECK( addVariable("weight", m_weight) );

  return Base::book();
}

/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::VxCandidate *TrackParticleVertexAssociationTool::get (const Rec::TrackParticle& p) {

  if(m_fast){
    return p.reconstructedVertex();
  }

  *m_weight = 0;

  const VxContainer* vxContainer = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_targetSGKey);
  if (sc.isFailure() || !vxContainer) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex container: " << m_targetSGKey;
    return 0;
  }

  int nVx = vxContainer->size();
  if(nVx < 1){
    return 0;
  }

  for(int v=0; v<nVx-1; v++){
    const Trk::VxCandidate* vx = vxContainer->at(v);
    const std::vector<Trk::VxTrackAtVertex*>* tracks = vx->vxTrackAtVertex();
    int nTracks = 0;
    if(tracks) nTracks = tracks->size();
    for(int t=0; t<nTracks; t++) {
      Trk::VxTrackAtVertex* track = tracks->at(t);
      if(track){
	Trk::ITrackLink* link = track->trackOrParticleLink();
	if (link) {
	  Trk::LinkToTrackParticleBase* linkToTrackParticle = dynamic_cast<Trk::LinkToTrackParticleBase*>(link);
	  if(linkToTrackParticle){
	    const Trk::TrackParticleBase *particle_base = **linkToTrackParticle;
	    if(particle_base == &p){
	      *m_weight = track->weight();
	      return vx;
	    }
	  }
	}
      }
    }
  }

  return vxContainer->at(nVx-1);
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const xAOD::Vertex*
TrackParticleVertexAssociationTool::get (const xAOD::TrackParticle& p)
{
  const xAOD::Vertex* foundVertex { nullptr };
  SG::ReadHandle<xAOD::VertexContainer> vertices { m_vertexKey };
  if (!vertices.isValid())
  {
    ATH_MSG_WARNING("No VertexContainer with key = " << m_vertexKey.key() );
    return foundVertex;
  }
  for ( const auto vx : *vertices ) 
  {
    for ( const auto& tpLink : vx->trackParticleLinks() )
    {
      if ( *tpLink == &p )
      {
	foundVertex = vx;
	break;
      }
    }
    if (foundVertex) break;
  }
  return foundVertex;
}


} // namespace D3PD

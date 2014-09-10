/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VxTrackAtVertexTrackParticleAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VxTrackAtVertexTrackParticleAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a VxTrackAtVertex to a TrackParticle.
 */

#include "VxTrackAtVertexTrackParticleAssociationTool.h"

#include <VxVertex/VxTrackAtVertex.h>
#include <Particle/TrackParticle.h>
#include <TrkParticleBase/LinkToTrackParticleBase.h>
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VxTrackAtVertexTrackParticleAssociationTool::VxTrackAtVertexTrackParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Rec::TrackParticle *VxTrackAtVertexTrackParticleAssociationTool::get (const Trk::VxTrackAtVertex& track)
{

  const Trk::LinkToTrackParticleBase *link = dynamic_cast<const Trk::LinkToTrackParticleBase *>(track.trackOrParticleLink());
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

  return particle;
}

} // namespace D3PD

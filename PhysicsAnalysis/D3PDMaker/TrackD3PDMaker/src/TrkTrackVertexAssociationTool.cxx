/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrkTrackVertexAssociationTool.cxx 502118 2012-05-23 19:10:19Z ssnyder $
/**
 * @file TrackD3PDMaker/src/TrkTrackVertexAssociationTool.cxx
 * @author Remi Zaidan <zaidan@cern.ch>
 * @date Mar, 2011
 * @brief Associate from a TrkTrack to its reconstructed vertex.
 */

#include "AthenaKernel/errorcheck.h"
#include "TrkTrackVertexAssociationTool.h"
#include "TrkTrack/Track.h"
#include "VxVertex/VxCandidate.h"
#include "Particle/TrackParticle.h"
#include <TrkParticleBase/LinkToTrackParticleBase.h>
#include <VxVertex/VxTrackAtVertex.h>

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrkTrackVertexAssociationTool::TrkTrackVertexAssociationTool
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
StatusCode TrkTrackVertexAssociationTool::initialize()
{
  return Base::initialize();
}

/**
  * @brief book ntuple variables
  */
StatusCode TrkTrackVertexAssociationTool::book(){

  CHECK( addVariable("weight", m_weight) );

  return Base::book();
}

/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::VxCandidate *TrkTrackVertexAssociationTool::get (const Trk::Track& p)
{

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
	    const Trk::TrackParticleBase *particleBase = **linkToTrackParticle;
	    if(particleBase){
	      const Trk::Track *originalTtrack = particleBase->originalTrack();
	      if(originalTtrack == &p){
		*m_weight = track->weight();
		return vx;
	      }
	    }
	  }
	}
      }
    }
  }

  return vxContainer->at(nVx-1);
}


} // namespace D3PD

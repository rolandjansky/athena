/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTwoTrackVertexTrackAssociationTool.h"
#include "AthenaKernel/errorcheck.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

namespace D3PD {


JetTagTwoTrackVertexTrackAssociationTool::JetTagTwoTrackVertexTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : base(type, name, parent)
{

}



StatusCode JetTagTwoTrackVertexTrackAssociationTool::initialize()
{


  CHECK ( base::initialize() );

  /// retrieve tools here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagTwoTrackVertexTrackAssociationTool::book()
{

   return StatusCode::SUCCESS;
}


StatusCode JetTagTwoTrackVertexTrackAssociationTool::reset (const JetTagTwoTrackVertexCandidateHolder& p)
{

  const Trk::VxCandidate *vtx = p.getVxCandidate();

  if(!vtx){
    m_trkItr = m_trkEnd;
  }


  const std::vector<Trk::VxTrackAtVertex*>* tracksOfVertex = vtx->vxTrackAtVertex();
  
  m_trkItr=tracksOfVertex->begin();
  m_trkEnd=tracksOfVertex->end();

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagTwoTrackVertexTrackAssociationTool::next()
{

  if(m_trkItr == m_trkEnd)return 0;

  const Trk::VxTrackAtVertex* trkatvx = *m_trkItr;
  ++m_trkItr;

  const Trk::ITrackLink* link=trkatvx->trackOrParticleLink();

  const Trk::LinkToTrackParticleBase* linkToTPBase=dynamic_cast<const Trk::LinkToTrackParticleBase *>(link);

  if(!linkToTPBase){
    ATH_MSG_WARNING(" ITrackLink does not point to a track particle");
    return 0;
  }

  const Trk::TrackParticleBase* myTrackParticleBase= linkToTPBase->cachedElement();
  const Rec::TrackParticle* tp=dynamic_cast<const Rec::TrackParticle*>(myTrackParticleBase);

  if(!tp){
    ATH_MSG_WARNING(" particle base is not a track particle");
    return 0;
  }
                   
  return  tp;


}

} // namespace D3PD
 

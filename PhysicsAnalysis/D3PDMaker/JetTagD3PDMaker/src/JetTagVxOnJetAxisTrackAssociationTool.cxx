/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagVxOnJetAxisTrackAssociationTool.h"
#include "AthenaKernel/errorcheck.h"

#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"


using HepGeom::Vector3D;
using CLHEP::Hep3Vector;


namespace D3PD {


JetTagVxOnJetAxisTrackAssociationTool::JetTagVxOnJetAxisTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : base(type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}



StatusCode JetTagVxOnJetAxisTrackAssociationTool::initialize()
{


  CHECK ( base::initialize() );

  /// retrieve tools here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagVxOnJetAxisTrackAssociationTool::book()
{

  CHECK( addVariable ("phiAtVx" , m_phiAtVx ) );
  CHECK( addVariable ("thetaAtVx" , m_thetaAtVx ) );
  CHECK( addVariable ("ptAtVx" , m_ptAtVx ) );


  return StatusCode::SUCCESS;
}


StatusCode JetTagVxOnJetAxisTrackAssociationTool::reset (const JetTagVxOnJetAxisCandidateHolder& p)
{

  const Trk::VxVertexOnJetAxis* vxonjet = p.getVxVertexOnJetAxis();

  if(!vxonjet){
    m_trkItr = m_trkEnd;
  }


  //// returned by reference - will not be deleted
  const std::vector<Trk::VxTrackAtVertex*>& tracksOfVertex = vxonjet->getTracksAtVertex();
  
  m_trkItr=tracksOfVertex.begin();
  m_trkEnd=tracksOfVertex.end();

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagVxOnJetAxisTrackAssociationTool::next()
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
                   
  const Trk::TrackParameters* perigeeAtVertex=trkatvx->perigeeAtVertex();

  const Amg::Vector3D & momentumTrack = perigeeAtVertex->momentum();
         
  double px=momentumTrack.x();
  double py=momentumTrack.y();
  double pz=momentumTrack.z();
  
  Hep3Vector momentumTrack3Vector(px,py,pz);

  *m_phiAtVx = momentumTrack3Vector.phi();
  *m_thetaAtVx = momentumTrack3Vector.theta();
  *m_ptAtVx = momentumTrack3Vector.perp();


  return  tp;


}

} // namespace D3PD
 

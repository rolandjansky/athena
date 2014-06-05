/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetTrackAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "VxVertex/VxContainer.h"
#include "JetTagEvent/TrackAssociation.h"
//#include "VxVertex/PrimaryVertexSelector.h"


using CLHEP::Hep3Vector;


namespace D3PD {


JetTagJetTrackAssociationTool::JetTagJetTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Rec::TrackParticle>(type, name, parent),
      m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator")
{

  declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);
  declareProperty("PrimaryVertexCollection", m_primaryVertexCollection="VxPrimaryCandidate");
  declareProperty("TrackAssocName", m_trackAssocName="Tracks");
  declareProperty("FillVariables", m_fillVariables=true);

  book().ignore(); // Avoid coverity warnings.
}


StatusCode JetTagJetTrackAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Rec::TrackParticle> base;

  CHECK ( base::initialize() );

  if(m_fillVariables){
    CHECK( m_trackToVertexIPEstimator.retrieve() );
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetTrackAssociationTool::book()
{

  if(m_fillVariables){

    CHECK( addVariable ("signOfIP", m_signOfIP) );
    CHECK( addVariable ("signOfZIP", m_signOfZIP) );

    CHECK( addVariable ("ud0wrtPriVtx", m_ud0wrtPriVtx) );
    CHECK( addVariable ("ud0ErrwrtPriVtx", m_ud0ErrwrtPriVtx) );
    CHECK( addVariable ("uz0wrtPriVtx", m_uz0wrtPriVtx) );
    CHECK( addVariable ("uz0ErrwrtPriVtx", m_uz0ErrwrtPriVtx) );
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetTrackAssociationTool::reset (const  Jet& p)
{
  
  m_tracks=0;
  m_jet = &p;
  const Analysis::TrackAssociation* trackassoc = p.getAssociation<Analysis::TrackAssociation>(m_trackAssocName);

  m_trkItr = m_trkEnd;

  if (trackassoc !=0){
    m_tracks = trackassoc->tracks();
    if(m_tracks->size() !=0){
      m_trkItr = m_tracks->begin();
      m_trkEnd = m_tracks->end();
    }
  }

 if(m_fillVariables){
   const VxContainer* vxContainer(0);
   StatusCode sc = evtStore()->retrieve(vxContainer, m_primaryVertexCollection);
   m_vertex=0;

   if( sc.isSuccess() && vxContainer->size() > 0){
     m_vertex = *(vxContainer->begin()); // PrimaryVertexSelector(*vxContainer);
   }
 }

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagJetTrackAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd){
    if(m_tracks) delete m_tracks;
    m_tracks=0;
    return 0;
  }
  const Rec::TrackParticle* trk= *m_trkItr;

  if(m_fillVariables){

    Hep3Vector jetDirection(m_jet->px(),m_jet->py(),m_jet->pz());

    *m_signOfIP=-999;
    *m_signOfZIP=-999;   
    *m_ud0wrtPriVtx=-999;
    *m_ud0ErrwrtPriVtx=-999;
    *m_uz0wrtPriVtx=-999;
    *m_uz0ErrwrtPriVtx=-999;

    if(m_vertex){

      *m_signOfIP=m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack
	(trk->definingParameters(),jetDirection.unit(), m_vertex->recVertex() );
      *m_signOfZIP = m_trackToVertexIPEstimator->getZLifetimeSignOfTrack
	(trk->definingParameters(),jetDirection.unit(), m_vertex->recVertex());

      const Trk::ImpactParametersAndSigma* iperr=
	m_trackToVertexIPEstimator->estimate(trk, m_vertex, true);
      if(iperr){
	*m_ud0wrtPriVtx=iperr->IPd0;
	*m_ud0ErrwrtPriVtx=iperr->sigmad0;
	*m_uz0wrtPriVtx=iperr->IPz0;
	*m_uz0ErrwrtPriVtx=iperr->sigmaz0;
	delete iperr;
      }
    }
  }

  ++m_trkItr;
  return trk;


}

} // namespace D3PD
 

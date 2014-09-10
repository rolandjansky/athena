/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleGetterTool.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/PrimaryVertexSelector.h"

namespace D3PD {

TrackParticleGetterTool::TrackParticleGetterTool
(const std::string& type,
 const std::string& name,
 const IInterface* parent)
  : Base (type, name, parent),
    m_vertex(0),
    m_hasVertex(0),
    m_trackSelector("")
{
  declareProperty ("VertexSGKey", m_vxCandidateSGKey = "VxPrimaryCandidate");
  declareProperty("TrackSelector", m_trackSelector);
}

StatusCode TrackParticleGetterTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_trackSelector.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrackParticleGetterTool::reset(bool allowMissing)
{

  // tracks
  const Rec::TrackParticleContainer* tracks = get(allowMissing);

  if (!tracks) {
    m_trkItr = m_trkEnd;
    if(allowMissing){
      return StatusCode::SUCCESS;
    }else{
      return StatusCode::FAILURE;
    }
  }

  m_trkItr = tracks->begin();
  m_trkEnd = tracks->end();

  // primary vertex
  const VxContainer* vxContainer = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_vxCandidateSGKey);
  if (sc.isFailure() || !vxContainer) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex container: " << m_vxCandidateSGKey;
    m_hasVertex = false;
  } else {
    // use the primary vertex selector function  
    m_vertex = PrimaryVertexSelector(*vxContainer);
    if (! m_vertex){
        //REPORT_MESSAGE (MSG::DEBUG) << "No primary vertices reconstructed";
        m_hasVertex = false;
    } else 
        m_hasVertex = true;
  }

  return StatusCode::SUCCESS;
}

template<>
const void* CollectionGetterTool<Rec::TrackParticleContainer>::nextUntyped(){
  return 0;
}

const void* TrackParticleGetterTool::nextUntyped()
{

  const Rec::TrackParticle* track = 0;

  do{

    if(m_trkItr == m_trkEnd){
      return 0;
    }

    track = *(m_trkItr);
    m_trkItr++;

  } while(!track || !selectTrack(track));

  return track;
}

bool TrackParticleGetterTool::selectTrack(const  Rec::TrackParticle *track)
{

  if(!m_hasVertex) return false;

  return m_trackSelector->decision(*track, &(m_vertex->recVertex()));
}

} // namespace D3PD

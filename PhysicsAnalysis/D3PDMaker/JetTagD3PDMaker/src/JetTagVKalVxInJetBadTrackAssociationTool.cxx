/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagVKalVxInJetBadTrackAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"	
#include "JetTagEvent/ISvxAssociation.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"

namespace D3PD {


JetTagVKalVxInJetBadTrackAssociationTool::JetTagVKalVxInJetBadTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Rec::TrackParticle>(type, name, parent)
{

  declareProperty("InfoType", m_infoType="InDetVKalVxInJetTool");
}


StatusCode JetTagVKalVxInJetBadTrackAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Rec::TrackParticle> base;

  CHECK ( base::initialize() );

  return StatusCode::SUCCESS;
}


StatusCode JetTagVKalVxInJetBadTrackAssociationTool::book()
{

  /// add variable here

  return StatusCode::SUCCESS;
}


StatusCode JetTagVKalVxInJetBadTrackAssociationTool::reset (const  Jet& p)
{


  const Analysis::ISvxAssociation* newSvxAssociation=
   p.getAssociation<Analysis::ISvxAssociation>(m_infoType);

  if (!newSvxAssociation){
    m_trkItr = m_trkEnd;
    return StatusCode::SUCCESS;
  }

  const Trk::VxSecVertexInfo* myVertexInfo=newSvxAssociation->vertexInfo();

  if (!myVertexInfo){
    m_trkItr = m_trkEnd;
    return StatusCode::SUCCESS;
  }

  const Trk::VxSecVKalVertexInfo* myVKalVertexInfo=dynamic_cast<const Trk::VxSecVKalVertexInfo*>(myVertexInfo);

  if (!myVKalVertexInfo){
    m_trkItr = m_trkEnd;
    return StatusCode::SUCCESS;
  }
 
  //// a copy is returned. need to hold it otherwise it will be deleted and the iter will be invalid
  m_holder = myVKalVertexInfo->badTracksTP();


  m_trkItr=m_holder.begin();
  m_trkEnd=m_holder.end();

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagVKalVxInJetBadTrackAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd) return 0;

 const Rec::TrackParticle* tp=dynamic_cast<const Rec::TrackParticle*>(*m_trkItr);

  ++m_trkItr;

  if(!tp){
    ATH_MSG_WARNING("can't cast TrackParticleBase to TrackParticle");
  }

  return tp;


}

} // namespace D3PD
 

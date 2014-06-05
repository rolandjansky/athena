/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetJFTwoTrackVertexAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"

#include "JetTagEvent/ISvxAssociation.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxVertex/VxCandidate.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"

namespace D3PD {


JetTagJetJFTwoTrackVertexAssociationTool::JetTagJetJFTwoTrackVertexAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : base(type, name, parent),
      m_twoTrackDataHolder(0)
{
  declareProperty("TwoTrackVertexName", m_twoTrackDataName="JetFitterTwoTrackVertexData");
  declareProperty("InfoName",  m_infoName = "NewJetFitterVxFinder" );
}



StatusCode JetTagJetJFTwoTrackVertexAssociationTool::initialize()
{


  CHECK ( base::initialize() );

  /// retrieve tools here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetJFTwoTrackVertexAssociationTool::book()
{

  /// book variable here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetJFTwoTrackVertexAssociationTool::reset (const  Jet& p)
{


  const Analysis::ISvxAssociation* newJFSvxAssociation=
    p.getAssociation<Analysis::ISvxAssociation>(m_infoName);

  if(!newJFSvxAssociation){
    m_vxIter=m_vxEnd;
    return StatusCode::SUCCESS;
  }

  const Trk::VxSecVertexInfo* myVertexInfo=newJFSvxAssociation->vertexInfo();     
  if(!myVertexInfo){
    m_vxIter=m_vxEnd;
    return StatusCode::SUCCESS;
  }

  const Trk::VxJetFitterVertexInfo* myJetFitterInfo=dynamic_cast<const Trk::VxJetFitterVertexInfo*>(myVertexInfo);
  if (!myJetFitterInfo){
    m_vxIter=m_vxEnd;
    return StatusCode::SUCCESS;
  }

  const Trk::TwoTrackVerticesInJet* twoTrackVerticesInfo=myJetFitterInfo->getTwoTrackVerticesInJet();
  if(!twoTrackVerticesInfo){
    m_vxIter=m_vxEnd;
    return StatusCode::SUCCESS;
  }


  const std::vector<const Trk::VxCandidate*>& twoTrackVertices=twoTrackVerticesInfo->getTwoTrackVertice();

 
  m_vxIter=twoTrackVertices.begin();
  m_vxEnd=twoTrackVertices.end();

  CHECK( evtStore()->retrieve(m_twoTrackDataHolder, m_twoTrackDataName) ); 

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const D3PD::JetTagTwoTrackVertexCandidateHolder* JetTagJetJFTwoTrackVertexAssociationTool::next()
{

  if(m_vxIter==m_vxEnd)return 0;

  const Trk::VxCandidate* vxcandidate = *(m_vxIter++);

  JetTagTwoTrackVertexDataHolder::const_iterator iter = m_twoTrackDataHolder->begin();

  for(; iter!=m_twoTrackDataHolder->end(); ++iter){

    if(vxcandidate == (*iter)->getVxCandidate()){
      /// no need to check for neutral tracks!
      break;
    }

  }

  if(iter == m_twoTrackDataHolder->end()){
    ATH_MSG_WARNING("VxCandidate not found for object " <<m_infoName <<" - check your collections" );
    return 0;
  }

  return *iter;

}

} // namespace D3PD
 

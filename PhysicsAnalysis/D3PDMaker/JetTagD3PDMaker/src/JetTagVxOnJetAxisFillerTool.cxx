/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagVxOnJetAxisFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagVxOnJetAxisCandidateHolder.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"


namespace D3PD {
  
  

JetTagVxOnJetAxisFillerTool::JetTagVxOnJetAxisFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<JetTagVxOnJetAxisCandidateHolder> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagVxOnJetAxisFillerTool::book()
{

  CHECK(addVariable ("vtxPos",      m_vtxPos ));
  CHECK(addVariable ("vtxErr",      m_vtxErr ));
  
  return StatusCode::SUCCESS;
}


StatusCode JetTagVxOnJetAxisFillerTool::fill(const JetTagVxOnJetAxisCandidateHolder& p)
{


  *m_vtxPos=-999;
  *m_vtxErr=-999;

  const Trk::VxJetCandidate* vxjet = p.getVxJetCandidate();

  if(!vxjet){
    ATH_MSG_WARNING("VxJetCandidate pointer null");
    return StatusCode::SUCCESS;
  }

  const Trk::RecVertexPositions& recVertexPositions=vxjet->getRecVertexPositions();
  const Amg::VectorX & vertexPosition=recVertexPositions.position();
  const Amg::MatrixX & vertexCovMatrix=recVertexPositions.covariancePosition();

  const Trk::VxVertexOnJetAxis* vxonjet = p.getVxVertexOnJetAxis();

  if(!vxonjet){
   ATH_MSG_WARNING("VxVertexOnJetAxis pointer null");
   return StatusCode::SUCCESS;
  }

  int vspos = vxonjet->getNumVertex()+5;

  *m_vtxPos = vertexPosition[vspos];
  if(vertexCovMatrix(vspos,vspos)>=0) *m_vtxErr = vertexCovMatrix(vspos,vspos);

  return StatusCode::SUCCESS;
}


}// namespace D3PD

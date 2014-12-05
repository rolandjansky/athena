/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetOriginCorrectionTool.cxx

#include "JetMomentTools/JetOriginCorrectionTool.h"

#include "JetUtils/JetOriginHelpers.h"

//**********************************************************************

JetOriginCorrectionTool::JetOriginCorrectionTool(const std::string& myname)
: asg::AsgTool(myname) { 
  declareProperty("VertexContainer", m_vtxContainerName="PrimaryVertices");
  declareProperty("OriginCorrectedName", m_correctionName="JetOriginConstitScaleMomentum");
}

//**********************************************************************

int JetOriginCorrectionTool::modify(xAOD::JetContainer& jetCont) const {

  const xAOD::VertexContainer *vxContainer = 0;

  // retrieve the VertexContainer. if fails, fill the jets with null vector
  if( (evtStore()->retrieve(vxContainer, m_vtxContainerName).isFailure()) || vxContainer->empty() ){
    ATH_MSG_WARNING("Could not find origin Vertex. Filling jet with null "<< m_correctionName);
    xAOD::JetFourMom_t null;    
    for(xAOD::Jet * j : jetCont) j->setAttribute<xAOD::JetFourMom_t>(m_correctionName, null);
    return 0;
  }

  // choose PV.
  const xAOD::Vertex *vx = vxContainer->at(0); 

  ATH_MSG_DEBUG(" correcting  jets ");
  for(xAOD::Jet * jet : jetCont){
    ATH_MSG_DEBUG("  ---->  jet "<< jet);
    ATH_MSG_DEBUG("                     jet pT: "<< jet->pt());
    xAOD::JetFourMom_t fv = jet::clusterOriginCorrection(*jet,*vx);
    ATH_MSG_DEBUG("  " <<  m_correctionName << " pT: " << fv.pt());
    jet->setAttribute<xAOD::JetFourMom_t>(m_correctionName, fv);
    jet->setAssociatedObject("OriginVertex", vx);

  }
  return 0;
}

//**********************************************************************

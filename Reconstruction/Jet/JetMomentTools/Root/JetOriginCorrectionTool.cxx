/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetOriginCorrectionTool.cxx

#include "JetMomentTools/JetOriginCorrectionTool.h"

#include "JetUtils/JetOriginHelpers.h"




//**********************************************************************

JetOriginCorrectionTool::JetOriginCorrectionTool(const std::string& myname)
: asg::AsgTool(myname) { 

}


//**********************************************************************

StatusCode JetOriginCorrectionTool::initialize() {
  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_eventInfo_key.initialize());
  if(m_jetContainerName.empty()) {
    ATH_MSG_ERROR("JetOriginCorrectionTool needs to have its input jet container configured!");
    return StatusCode::FAILURE;
  }

  m_scaleMomentumPtKey = m_jetContainerName + "." + m_scaleMomentumName + "_" + m_scaleMomentumPtKey.key();
  m_scaleMomentumPhiKey = m_jetContainerName + "." + m_scaleMomentumName + "_" + m_scaleMomentumPhiKey.key();
  m_scaleMomentumEtaKey = m_jetContainerName + "." + m_scaleMomentumName + "_" + m_scaleMomentumEtaKey.key();
  m_scaleMomentumMKey = m_jetContainerName + "." + m_scaleMomentumName + "_" + m_scaleMomentumMKey.key();
  m_originVertexKey = m_jetContainerName + "." + m_originVertexKey.key();

  ATH_CHECK(m_scaleMomentumPtKey.initialize());
  ATH_CHECK(m_scaleMomentumPhiKey.initialize());
  ATH_CHECK(m_scaleMomentumEtaKey.initialize());
  ATH_CHECK(m_scaleMomentumMKey.initialize());
  ATH_CHECK(m_originVertexKey.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetOriginCorrectionTool::decorate(const xAOD::JetContainer& jetCont) const {
  SG::WriteDecorHandle<xAOD::JetContainer, float> scaleMomentumPtHandle(m_scaleMomentumPtKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> scaleMomentumPhiHandle(m_scaleMomentumPhiKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> scaleMomentumEtaHandle(m_scaleMomentumEtaKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> scaleMomentumMHandle(m_scaleMomentumMKey);
  SG::WriteDecorHandle<xAOD::JetContainer, ElementLink<xAOD::VertexContainer>> originVertexHandle(m_originVertexKey);

  // static accessor for PV index access
  static SG::AuxElement::ConstAccessor<int> PVIndexAccessor("PVIndex");



  // retrieve the VertexContainer. if fails, fill the jets with null vector
  auto handle = SG::makeHandle (m_vertexContainer_key);
  if (!handle.isValid()){
    ATH_MSG_WARNING("Invalid VertexContainer datahandle: " 
                    <<  m_correctionName
                    << ": filling jet with -1");
    xAOD::JetFourMom_t null;    
    for(const xAOD::Jet * j : jetCont) {
      scaleMomentumPtHandle(*j) = -1;
      scaleMomentumPhiHandle(*j) = -1;
      scaleMomentumEtaHandle(*j) = -1;
      scaleMomentumMHandle(*j) = -1;
    }
    return StatusCode::SUCCESS;
  }

  auto vxContainer = handle.cptr();


  // Retrieve EventInfo to check for a PV# specification != PV0
  // No errors if EventInfo or PV index is not specified, 
  // as this is the standard scenario
  // Warn if EventInfo is specified but cannot be retrieved
  // Specifying the PV index is only for special cases
  int PVindex = 0;
  //if (m_eventInfoName.key() != "") {
  if (m_eventInfo_key.key() != "") {
    // retrieve the VertexContainer. if fails, fill the jets with null vector

    auto eInfo = SG::makeHandle (m_eventInfo_key);
    if (!eInfo.isValid()){
      ATH_MSG_WARNING("Invalid eventInfo datahandle. Defaulting to PV0 for " 
                      <<  m_correctionName);
    } else if (PVIndexAccessor.isAvailable(*(eInfo.cptr()))) {
      PVindex = PVIndexAccessor(*(eInfo.cptr()));
      ATH_MSG_DEBUG("Found PVIndex value of " << PVindex << " for " << m_correctionName);
      
      if (PVindex < 0 || static_cast<size_t>(PVindex) >= vxContainer->size()){
        ATH_MSG_WARNING("Specified PV index of " 
                        << PVindex << " is out of bounds.  Filling jet with -1"
                        <<m_correctionName);
        xAOD::JetFourMom_t null;
        for (const xAOD::Jet* j : jetCont) {
          scaleMomentumPtHandle(*j) = -1;
          scaleMomentumPhiHandle(*j) = -1;
          scaleMomentumEtaHandle(*j) = -1;
          scaleMomentumMHandle(*j) = -1;
        }
        return StatusCode::SUCCESS;
      }
      
    }
  }

  // choose PV.
  const xAOD::Vertex *vx = vxContainer->at(PVindex); 

  ATH_MSG_DEBUG(" correcting  jets ");
  for(const xAOD::Jet * jet : jetCont){
    ATH_MSG_DEBUG("  ---->  jet "<< jet);
    ATH_MSG_DEBUG("                     jet pT: "<< jet->pt());

    if(!m_onlyAssignPV) {
      xAOD::JetFourMom_t fv = jet::clusterOriginCorrection(*jet,*vx);
      ATH_MSG_DEBUG("  " <<  m_correctionName << " pT: " << fv.pt());
      scaleMomentumPtHandle(*jet) = fv.pt();
      scaleMomentumPhiHandle(*jet) = fv.phi();
      scaleMomentumEtaHandle(*jet) = fv.eta();
      scaleMomentumMHandle(*jet) = fv.M();
    }
    originVertexHandle(*jet) = ElementLink<xAOD::VertexContainer>(*vxContainer, vxContainer->at(PVindex)->index());

  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

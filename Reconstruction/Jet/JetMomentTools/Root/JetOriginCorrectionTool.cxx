/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetOriginCorrectionTool.cxx

#include "JetMomentTools/JetOriginCorrectionTool.h"

#include "JetUtils/JetOriginHelpers.h"




//**********************************************************************

JetOriginCorrectionTool::JetOriginCorrectionTool(const std::string& myname)
: asg::AsgTool(myname) { 
  declareProperty("OriginCorrectedName", m_correctionName="JetOriginConstitScaleMomentum");

  declareProperty("OnlyAssignPV", m_onlyAssignPV=false);

  declareProperty("VertexContainer", m_vertexContainer_key="PrimaryVertices");
  declareProperty("EventInfoName", m_eventInfo_key="EventInfo");
}


//**********************************************************************

StatusCode JetOriginCorrectionTool::initialize() {
  ATH_MSG_DEBUG("initializing version with data handles");

  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_eventInfo_key.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetOriginCorrectionTool::modify(xAOD::JetContainer& jetCont) const {
  // static accessor for PV index access
  static SG::AuxElement::ConstAccessor<int> PVIndexAccessor("PVIndex");



  // retrieve the VertexContainer. if fails, fill the jets with null vector
  auto handle = SG::makeHandle (m_vertexContainer_key);
  if (!handle.isValid()){
    ATH_MSG_WARNING("Invalid VertexContainer datahandle: " 
                    <<  m_correctionName
                    << ": filling jet with null");
    xAOD::JetFourMom_t null;    
    for(xAOD::Jet * j : jetCont) j->setAttribute<xAOD::JetFourMom_t>(m_correctionName, null);
    return 0;
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
      ATH_MSG_WARNING("Invalid eventInfo datahandle. Defaulting ti PV0 for " 
                      <<  m_correctionName);
    } else if (PVIndexAccessor.isAvailable(*(eInfo.cptr()))) {
      PVindex = PVIndexAccessor(*(eInfo.cptr()));
      ATH_MSG_DEBUG("Found PVIndex value of " << PVindex << " for " << m_correctionName);
      
      if (PVindex < 0 || static_cast<size_t>(PVindex) >= vxContainer->size()){
        ATH_MSG_WARNING("Specified PV index of " 
                        << PVindex << " is out of bounds.  Filling jet with null "
                        <<m_correctionName);
        xAOD::JetFourMom_t null;
        for (xAOD::Jet* j : jetCont) j->setAttribute<xAOD::JetFourMom_t>(m_correctionName,null);
        return 0;
      }
      
    }
  }

  // choose PV.
  const xAOD::Vertex *vx = vxContainer->at(PVindex); 

  ATH_MSG_DEBUG(" correcting  jets ");
  for(xAOD::Jet * jet : jetCont){
    ATH_MSG_DEBUG("  ---->  jet "<< jet);
    ATH_MSG_DEBUG("                     jet pT: "<< jet->pt());

    if(!m_onlyAssignPV) {
      xAOD::JetFourMom_t fv = jet::clusterOriginCorrection(*jet,*vx);
      ATH_MSG_DEBUG("  " <<  m_correctionName << " pT: " << fv.pt());
      jet->setAttribute<xAOD::JetFourMom_t>(m_correctionName, fv);
    }
    jet->setAssociatedObject("OriginVertex", vx);

  }
  return 0;
}

//**********************************************************************

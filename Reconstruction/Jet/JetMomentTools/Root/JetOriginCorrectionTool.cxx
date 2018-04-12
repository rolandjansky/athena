/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetOriginCorrectionTool.cxx

#include "JetMomentTools/JetOriginCorrectionTool.h"

#include "JetUtils/JetOriginHelpers.h"

#include "xAODEventInfo/EventInfo.h"

//**********************************************************************

JetOriginCorrectionTool::JetOriginCorrectionTool(const std::string& myname)
: asg::AsgTool(myname) { 
  declareProperty("VertexContainer", m_vtxContainerName="PrimaryVertices");
  declareProperty("OriginCorrectedName", m_correctionName="JetOriginConstitScaleMomentum");
  declareProperty("EventInfoName", m_eInfoName="EventInfo");

  declareProperty("OnlyAssignPV", m_onlyAssignPV=false);
  declareProperty("ForceEMScale", m_doForceEMScale=false);
}

//**********************************************************************

int JetOriginCorrectionTool::modify(xAOD::JetContainer& jetCont) const {
  // static accessor for PV index access
  static SG::AuxElement::ConstAccessor<int> PVIndexAccessor("PVIndex");

  const xAOD::VertexContainer *vxContainer = 0;

  // retrieve the VertexContainer. if fails, fill the jets with null vector
  if( (evtStore()->retrieve(vxContainer, m_vtxContainerName).isFailure()) || vxContainer->empty() ){
    ATH_MSG_WARNING("Could not find origin Vertex. Filling jet with null "<< m_correctionName);
    xAOD::JetFourMom_t null;    
    for(xAOD::Jet * j : jetCont) j->setAttribute<xAOD::JetFourMom_t>(m_correctionName, null);
    return 0;
  }

  // Retrieve EventInfo to check for a PV# specification != PV0
  // No errors if EventInfo or PV index is not specified, as this is the standard scenario
  // Warn if EventInfo is specified but cannot be retrieved
  // Specifying the PV index is only for special cases
  int PVindex = 0;
  if (m_eInfoName != "")
  {
    const xAOD::EventInfo* eInfo = 0;
    if ( evtStore()->retrieve(eInfo,m_eInfoName).isFailure() )
      ATH_MSG_WARNING("Failed to retrieve EventInfo object.  Defaulting to PV0 for "<<m_correctionName);
    else if (PVIndexAccessor.isAvailable(*eInfo))
    {
        PVindex = PVIndexAccessor(*eInfo);
        ATH_MSG_DEBUG("Found PVIndex value of " << PVindex << " for " << m_correctionName);
        if (PVindex < 0 || static_cast<size_t>(PVindex) >= vxContainer->size())
        {
          ATH_MSG_WARNING("Specified PV index of " << PVindex << " is out of bounds.  Filling jet with null "<<m_correctionName);
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
      xAOD::CaloCluster::State state = xAOD::CaloCluster::UNKNOWN;
      if(m_doForceEMScale) state = xAOD::CaloCluster::UNCALIBRATED;
      xAOD::JetFourMom_t fv = jet::clusterOriginCorrection(*jet,*vx,state);
      ATH_MSG_DEBUG("  " <<  m_correctionName << " pT: " << fv.pt());
      jet->setAttribute<xAOD::JetFourMom_t>(m_correctionName, fv);
    }
    jet->setAssociatedObject("OriginVertex", vx);

  }
  return 0;
}

//**********************************************************************

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValPFO.h"
#include "xAODPFlow/PFOContainer.h"

PhysValPFO::PhysValPFO (const std::string& type, const std::string& name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ), m_vertexContainerReadHandle("PrimaryVertices"),m_retrievePFOTool("RetrievePFOTool",this), m_useLCScale(false), m_useNeutralPFO(false) {
  declareProperty("RetrievePFOTool",m_retrievePFOTool,"Name of PFO getter");
  declareProperty("useLCScale",m_useLCScale, " Select which PFO setup to use - LC or EM ");
  declareProperty("useNeutralPFO", m_useNeutralPFO, "Select whether to use neutral or charged PFO");
}

PhysValPFO::~PhysValPFO() {}

StatusCode PhysValPFO::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_retrievePFOTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode PhysValPFO::bookHistograms(){

  std::string scale = "EM";
  std::string type = "charged";

  if (m_useLCScale) scale = "LC";

  if (m_useNeutralPFO) type = "neutral";
  else scale = "";

  std::string theName = "PFlow/PFO_JetETMiss/JetETMiss_"+scale+"_"+type;

  std::vector<HistData> hists;
  if (!m_useNeutralPFO){
    m_PFOChargedValidationPlots.reset(new PFOChargedValidationPlots(0,theName, theName));
    m_PFOChargedValidationPlots->setDetailLevel(100);
    m_PFOChargedValidationPlots->initialize();
    hists = m_PFOChargedValidationPlots->retrieveBookedHistograms();
  }
  else if (m_useNeutralPFO){
    m_PFONeutralValidationPlots.reset(new PFONeutralValidationPlots(0,theName, theName));
    m_PFONeutralValidationPlots->setDetailLevel(100);
    m_PFONeutralValidationPlots->initialize();
    hists = m_PFONeutralValidationPlots->retrieveBookedHistograms();
  }
  
  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValPFO::fillHistograms(){

  const xAOD::Vertex* theVertex = nullptr;

  if (!m_useNeutralPFO){
    if(!m_vertexContainerReadHandle.isValid()){
      ATH_MSG_WARNING("Invalid ReadHandle for xAOD::VertexContainer with key: " << m_vertexContainerReadHandle.key());
    }
    else {
      //Vertex finding logic based on logic in JetRecTools/PFlowPseudoJetGetter tool
      //Usually the 0th vertex is the primary one, but this is not always the case. So we will choose the first vertex of type PriVtx
      for (auto vertex : (*m_vertexContainerReadHandle.ptr())) {
	if (xAOD::VxType::PriVtx == vertex->vertexType() ) {
	theVertex = vertex;
	break;
	}//If we have a vertex of type primary vertex
      }//iterate over the vertices and check their type

      if (nullptr == theVertex) ATH_MSG_WARNING("Did not find either a PriVtx or a NoVtx in this event");
      
    }//if valid read handle
  }

  
  const xAOD::PFOContainer* thePFOContainer = nullptr;

  if (!m_useNeutralPFO) thePFOContainer = m_retrievePFOTool->retrievePFO(CP::EM,CP::charged);
  else{
    if (!m_useLCScale) thePFOContainer = m_retrievePFOTool->retrievePFO(CP::EM,CP::neutral);
    else thePFOContainer = m_retrievePFOTool->retrievePFO(CP::LC,CP::neutral);
  }

  if (!thePFOContainer){
    ATH_MSG_WARNING(" Have NULL pointer to xAOD::PFOContainer");
    return StatusCode::SUCCESS;
  }
  
  xAOD::PFOContainer::const_iterator firstPFO = thePFOContainer->begin();
  xAOD::PFOContainer::const_iterator lastPFO = thePFOContainer->end();

  for (; firstPFO != lastPFO; ++firstPFO) {
    const xAOD::PFO* thePFO = *firstPFO;
    if (!m_useNeutralPFO) m_PFOChargedValidationPlots->fill(*thePFO,theVertex);
    else if (m_useNeutralPFO) m_PFONeutralValidationPlots->fill(*thePFO);
  }

  return StatusCode::SUCCESS;

}

StatusCode PhysValPFO::procHistograms(){
   return StatusCode::SUCCESS;
}

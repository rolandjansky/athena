/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValPFO.h"
#include "xAODPFlow/PFOContainer.h"

PhysValPFO::PhysValPFO (const std::string& type, const std::string& name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ), m_useLCScale(false), m_useNeutralPFO(false) {
  declareProperty("useLCScale",m_useLCScale, " Select which PFO setup to use - LC or EM ");
  declareProperty("useNeutralPFO", m_useNeutralPFO, "Select whether to use neutral or charged PFO");
}

PhysValPFO::~PhysValPFO() {}

StatusCode PhysValPFO::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());

  ATH_CHECK(m_vertexContainerReadHandleKey.initialize());
  ATH_CHECK(m_PFOContainerHandleKey.initialize());

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
    SG::ReadHandle<xAOD::VertexContainer> vertexContainerReadHandle(m_vertexContainerReadHandleKey);
    if(!vertexContainerReadHandle.isValid()){
      ATH_MSG_WARNING("Invalid ReadHandle for xAOD::VertexContainer with key: " << vertexContainerReadHandle.key());
    }
    else {
      //Vertex finding logic based on logic in JetRecTools/PFlowPseudoJetGetter tool
      //Usually the 0th vertex is the primary one, but this is not always the case. So we will choose the first vertex of type PriVtx
      for (auto vertex : *vertexContainerReadHandle) {
	if (xAOD::VxType::PriVtx == vertex->vertexType() ) {
	theVertex = vertex;
	break;
	}//If we have a vertex of type primary vertex
      }//iterate over the vertices and check their type

      if (nullptr == theVertex) ATH_MSG_WARNING("Did not find either a PriVtx or a NoVtx in this event");
      
    }//if valid read handle
  }

  StatusCode sc = StatusCode::SUCCESS;
  if (m_useNeutralPFO) {    
    if (m_useLCScale) sc = m_PFOContainerHandleKey.assign("JetETMiss_LCNeutralParticleFlowObjects");
    else sc = m_PFOContainerHandleKey.assign("JetETMissNeutralParticleFlowObjects");
  }
    
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR("Could not assign PFO Container key name");
    return sc;
  }
    
  SG::ReadHandle<xAOD::PFOContainer> PFOContainerReadHandle(m_PFOContainerHandleKey);
  if(!PFOContainerReadHandle.isValid()){
     ATH_MSG_WARNING("Invalid ReadHandle for xAOD::PFOContainer with key: " << PFOContainerReadHandle.key());
     return StatusCode::SUCCESS;
  }
  
  for (auto thePFO : *PFOContainerReadHandle){
    if(thePFO){
       if (!m_useNeutralPFO) m_PFOChargedValidationPlots->fill(*thePFO,theVertex);
       else if (m_useNeutralPFO) m_PFONeutralValidationPlots->fill(*thePFO);
    }
    else ATH_MSG_WARNING("Invalid pointer to xAOD::PFO");
  }
  
  return StatusCode::SUCCESS;

}

StatusCode PhysValPFO::procHistograms(){
   return StatusCode::SUCCESS;
}

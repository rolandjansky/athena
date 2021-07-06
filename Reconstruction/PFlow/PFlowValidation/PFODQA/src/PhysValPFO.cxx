/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValPFO.h"
#include "xAODPFlow/PFOContainer.h"

PhysValPFO::PhysValPFO (const std::string& type, const std::string& name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ), m_useNeutralPFO(false) {
  declareProperty("useNeutralPFO", m_useNeutralPFO, "Select whether to use neutral or charged PFO");
}

PhysValPFO::~PhysValPFO() {}

StatusCode PhysValPFO::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());

  ATH_CHECK(m_vertexContainerReadHandleKey.initialize());
  ATH_CHECK(m_PFOContainerHandleKey.initialize());
  ATH_CHECK(m_eventInfoReadHandleKey.initialize());

  return StatusCode::SUCCESS;
}


StatusCode PhysValPFO::bookHistograms(){
    
  std::string theName = "PFlow/"+m_PFOContainerHandleKey.key();
  
  std::vector<HistData> hists;
  if (!m_useNeutralPFO){
    m_PFOChargedValidationPlots.reset(new PFOChargedValidationPlots(0,theName, theName,""));
    m_PFOChargedValidationPlots->setDetailLevel(100);
    m_PFOChargedValidationPlots->initialize();
    hists = m_PFOChargedValidationPlots->retrieveBookedHistograms();
  }
  else if (m_useNeutralPFO){
    m_PFONeutralValidationPlots.reset(new PFONeutralValidationPlots(0,theName, theName,""));
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

  SG::ReadHandle<xAOD::EventInfo> eventInfoReadHandle(m_eventInfoReadHandleKey);
  if (!eventInfoReadHandle.isValid()){
    ATH_MSG_WARNING("Invalid ReadHandle for xAOD::EventInfo with key: " << eventInfoReadHandle.key());
  }

  SG::ReadHandle<xAOD::PFOContainer> PFOContainerReadHandle(m_PFOContainerHandleKey);
  if(!PFOContainerReadHandle.isValid()){
     ATH_MSG_WARNING("Invalid ReadHandle for xAOD::PFOContainer with key: " << PFOContainerReadHandle.key());
     return StatusCode::SUCCESS;
  }
  for (auto thePFO : *PFOContainerReadHandle){
    if(thePFO){
       if (!m_useNeutralPFO) m_PFOChargedValidationPlots->fill(*thePFO,theVertex,*eventInfoReadHandle);
       else if (m_useNeutralPFO) m_PFONeutralValidationPlots->fill(*thePFO,*eventInfoReadHandle);
    }
    else ATH_MSG_WARNING("Invalid pointer to xAOD::PFO");
  }
  
  return StatusCode::SUCCESS;

}

StatusCode PhysValPFO::procHistograms(){
   return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValFE.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"

PhysValFE::PhysValFE (const std::string& type, const std::string& name, const IInterface* parent ) : 
  ManagedMonitorToolBase( type, name, parent ), 
  m_useNeutralFE(false)
{
  declareProperty("useNeutralFE", m_useNeutralFE, "Select whether to use neutral or charged FE");
}

PhysValFE::~PhysValFE() {}

StatusCode PhysValFE::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_PFOContainerHandleKey.initialize());
  ATH_CHECK(m_vertexContainerReadHandleKey.initialize());
  ATH_CHECK(m_FEContainerHandleKey.initialize());

  return StatusCode::SUCCESS;
}


StatusCode PhysValFE::bookHistograms(){
    
  std::string theName = "PFlow/FlowElement/"+m_FEContainerHandleKey.key();
  
  std::vector<HistData> hists;
  if (!m_useNeutralFE){
    m_FEChargedValidationPlots.reset(new PFOChargedValidationPlots(0,theName,"", theName));
    m_FEChargedValidationPlots->setDetailLevel(100);
    m_FEChargedValidationPlots->initialize();
    hists = m_FEChargedValidationPlots->retrieveBookedHistograms();
  }
  else if (m_useNeutralFE){
    m_FENeutralValidationPlots.reset(new PFONeutralValidationPlots(0,theName, "",theName));
    m_FENeutralValidationPlots->setDetailLevel(100);
    m_FENeutralValidationPlots->initialize();
    hists = m_FENeutralValidationPlots->retrieveBookedHistograms();
  }
  
  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValFE::fillHistograms(){

  const xAOD::Vertex* theVertex = nullptr;
  
  if (!m_useNeutralFE){
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

  SG::ReadHandle<xAOD::FlowElementContainer> FEContainerReadHandle(m_FEContainerHandleKey);
  if(!FEContainerReadHandle.isValid()){
     ATH_MSG_WARNING("Invalid ReadHandle for xAOD::FlowElementContainer with key: " << FEContainerReadHandle.key());
     return StatusCode::SUCCESS;
  }
  bool readMatches=false;

  for (auto theFE : *FEContainerReadHandle){
    if(theFE){
       if (!m_useNeutralFE) m_FEChargedValidationPlots->fill(*theFE,theVertex);
       else if (m_useNeutralFE) m_FENeutralValidationPlots->fill(*theFE);
       if(readMatches){
	 std::cout<<"Element link reading functionality to be added"<<std::endl;
       }
    }
    else ATH_MSG_WARNING("Invalid pointer to xAOD::FlowElement");
  }

  SG::ReadHandle<xAOD::PFOContainer> PFOContainerReadHandle(m_PFOContainerHandleKey);
  if(!PFOContainerReadHandle.isValid()){
    ATH_MSG_WARNING("PFO readhandle is a dud");
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::SUCCESS;

}

StatusCode PhysValFE::procHistograms(){
   return StatusCode::SUCCESS;
}

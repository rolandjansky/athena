/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValFlowElement.h"
#include "xAODPFlow/FlowElementContainer.h"

PhysValFlowElement::PhysValFlowElement (const std::string& type, const std::string& name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ) {
}

PhysValFlowElement::~PhysValFlowElement() {}

StatusCode PhysValFlowElement::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());

  ATH_CHECK(m_PFFlowElementContainerHandleKey.initialize());

  return StatusCode::SUCCESS;
}


StatusCode PhysValFlowElement::bookHistograms(){
    
  std::string theName = "PFlow/"+m_PFFlowElementContainerHandleKey.key();
  
  std::vector<HistData> hists;
  m_PFChargedFlowElementValidationPlots.reset(new PFChargedFlowElementValidationPlots(0,theName, theName));
  m_PFChargedFlowElementValidationPlots->setDetailLevel(100);
  m_PFChargedFlowElementValidationPlots->initialize();
  hists = m_PFChargedFlowElementValidationPlots->retrieveBookedHistograms();

  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValFlowElement::fillHistograms(){

  SG::ReadHandle<xAOD::FlowElementContainer> PFlowElementContainerReadHandle(m_PFFlowElementContainerHandleKey);
  if(!PFlowElementContainerReadHandle.isValid()){
     ATH_MSG_WARNING("Invalid ReadHandle for xAOD::FlowElementContainer with key: " << PFlowElementContainerReadHandle.key());
     return StatusCode::SUCCESS;
  }
  
  for (auto theFE : *PFlowElementContainerReadHandle){
    if(theFE){
       m_PFChargedFlowElementValidationPlots->fill(*theFE, nullptr);
    }
    else ATH_MSG_WARNING("Invalid pointer to xAOD::FlowElement");
  }
  
  return StatusCode::SUCCESS;

}

StatusCode PhysValFlowElement::procHistograms(){
   return StatusCode::SUCCESS;
}

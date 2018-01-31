/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetRec/TrigHLTJetDebug.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventShape/EventShape.h"


TrigHLTJetDebug::TrigHLTJetDebug(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
  declareProperty("chainName", m_chainName = "unknown" );
  declareProperty("eventshape_key", m_eventShapeSGKey = "unknown" );

}


TrigHLTJetDebug::~TrigHLTJetDebug() { }


HLT::ErrorCode TrigHLTJetDebug::hltInitialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
 
 StatusCode sc = service("StoreGateSvc", m_storeGate); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Unable to locate Service StoreGateSvc");
    return HLT::FATAL;
  }
 
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTJetDebug::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}


HLT::ErrorCode TrigHLTJetDebug::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  ATH_MSG_DEBUG("Executing " << name() << "...");
  pass = true;

  // Get the EventShape directly from StoreGate

  const xAOD::EventShape* pEventShape = nullptr;
  StatusCode sc;
  
  sc = m_storeGate-> retrieve(pEventShape, m_eventShapeSGKey);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Unable to locate retrieve event shape with key "
                    << m_eventShapeSGKey);
    return HLT::FATAL;
  }

  double energyDensity;
  if (pEventShape->getDensity(xAOD::EventShape::EventDensityID::Density,
                              energyDensity)){
    ATH_MSG_DEBUG ( "Energy Density from EventShape with SG key  "
                    << m_eventShapeSGKey << " "
                    << energyDensity );
    
  } else {
      ATH_MSG_WARNING (
                       "No Energy Density from EventShape with SG key  "
                       << m_eventShapeSGKey);
  }

  // Get JetContainer from input trigger element
  const xAOD::JetContainer* j_container = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, j_container);  

  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved JetContainer, address" << j_container);
  } else {
    ATH_MSG_ERROR("Failed to retrieve JetContainer.");
    return hltStatus;
  }

  if (j_container == 0){
    ATH_MSG_INFO("JetContainer address = 0, giving up");
    return HLT::OK;
  }

  ATH_MSG_DEBUG("No of jets in the container: " << j_container->size());

  for(auto j: *j_container){
    ATH_MSG_DEBUG("jet area" 
                  << j->getAttribute<xAOD::JetFourMom_t>("ActiveArea4vec"));
    
  }
  
  return HLT::OK;
}

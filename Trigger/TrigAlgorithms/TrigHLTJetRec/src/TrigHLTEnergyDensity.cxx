/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTEnergyDensity:Trigger algorithm that reads CaloClausters from
// an incoming trigger element, converts them to pseudojets, and processes them
// with the offline EventDensityTool object.

#include <string>
#include "TrigHLTJetRec/TrigHLTEnergyDensity.h"
#include "EventShapeTools/EventDensityTool.h"
#include "TrigHLTJetRec/ITriggerPseudoJetGetter.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigHLTJetRec/AnyToPseudoJet.h"
#include "TrigHLTJetRec/ClusterToPseudoJetConverter.h"
#include "xAODEventShape/EventShape.h"


TrigHLTEnergyDensity::TrigHLTEnergyDensity(const std::string& name, 
                                           ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator) {
  declareProperty("energyDensityTool", m_energyDensityTool);
  declareProperty("pseudoJetGetter", m_pseudoJetGetter);
  declareProperty("eventShapeSGKey", m_eventShapeSGKey);
  declareProperty("caloClusterContainerSGKey", m_caloClusterContainerSGKey="");
  declareProperty( "cluster_calib", m_clusterCalib);


  declareMonitoredVariable("energyDensity", m_energyDensity);

}


TrigHLTEnergyDensity::~TrigHLTEnergyDensity() { }


HLT::ErrorCode TrigHLTEnergyDensity::hltInitialize() {
  /* Retrieve the JetRecTool and, separately, its PSeudoJetGetter lower tool
     from the ToolSvc. The python configuration code ensures that ToolSvc
     can access these tool. */

StatusCode sc;
 ATH_MSG_INFO("Initializing " << name() << "...");

 ATH_MSG_INFO("Retrieving tools...");
 sc = m_energyDensityTool.retrieve();

 if (sc.isSuccess()) {
   ATH_MSG_INFO("Retrieved  energyDensityTool "
                <<  m_energyDensityTool -> name());
  }else{
   ATH_MSG_ERROR("Unable to retrieve the energyDensityTool.");
   return HLT::ERROR;
 }

 if  (m_pseudoJetGetter.retrieve().isSuccess()){
   ATH_MSG_INFO("Retrieved  shared PseudoJetGetter "
                <<  m_pseudoJetGetter->name());
 } else {
   ATH_MSG_ERROR("Unable to retrieve shared IPseudoJetGetter");
   return HLT::ERROR;
 }
 
  ATH_MSG_INFO("Tool retrieval completed.");
  
  ATH_MSG_INFO("    " << m_energyDensityTool->name());
  m_energyDensityTool->print();
  
  ATH_MSG_INFO("  Shared PseudoJetGetter:");
  ATH_MSG_INFO("    " << m_pseudoJetGetter->name());
  
  ATH_MSG_INFO("  EventShape storegate key: " << m_eventShapeSGKey);
  return HLT::OK;
}


HLT::ErrorCode TrigHLTEnergyDensity::hltFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}

HLT::ErrorCode
TrigHLTEnergyDensity::hltExecute(const HLT::TriggerElement* inputTE,
                                 HLT::TriggerElement* outputTE) {  

  ATH_MSG_VERBOSE("Executing " << name() << "...");
  ATH_MSG_DEBUG("Executing tool " << m_energyDensityTool->name());
  
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

  /* Get CaloClusterContainer from the output trigger element
     Upstream Algorithms write to this trigger element */
  const xAOD::CaloClusterContainer* clusterContainer = 0;
  HLT::ErrorCode hltStatus;
  hltStatus = getFeature(outputTE, clusterContainer, m_caloClusterContainerSGKey);  
  
  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved CaloClusterContainer, address"
                  << clusterContainer);
  } else {
    ATH_MSG_ERROR("Failed to retrieve CaloClusterContainer.");
    return HLT::ERROR;
  }

  if (clusterContainer == 0){
    ATH_MSG_ERROR("CaloClusterContainer address = 0, giving up");
    return HLT::ERROR;
  }
  
  ATH_MSG_DEBUG("Number of incoming clusters: " << clusterContainer->size());
  
  LabelIndex* indexMap = new LabelIndex("PseudoJetLabelMapTrigger");
  indexMap->addLabel("Topo");
  // setup CaloCluster to PseudoJet convertor
  AnyToPseudoJet<const xAOD::CaloCluster*> ctpj(indexMap);
  PseudoJetVector pjv;

  auto status = this -> getPseudoJets(clusterContainer, indexMap, pjv);
  if (status == HLT::OK) {
    ATH_MSG_DEBUG("Obtained pseudojets");
  } else {
    ATH_MSG_ERROR("Failed to get pseudojets ");
    return status;
  }

  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
  // Load the pseudo jets into the TriggerSPseudoJetGetter tool
  // Despite the name, we push the pseudojets into the tool. This is
  // in contrast to offline PseudoJetGetters which fetch the pseudojets.
  m_pseudoJetGetter->prime(&pjv);  
  m_pseudoJetGetter->print();
  
  ATH_MSG_DEBUG("Executing tool " << m_energyDensityTool->name());
  StatusCode sc = m_energyDensityTool->fillEventShape();
  
  if (sc.isSuccess()) {
    ATH_MSG_DEBUG("filled event shape ");
  }else{
    ATH_MSG_ERROR("Unable to fill the event shape object");
    return HLT::ERROR;
  }

  /* Event shape is retrieved directly from storegate, os not 
     available via navigation (ie via the triggger element) */
  const xAOD::EventShape* eventShape;
 if ( evtStore()->retrieve(eventShape, m_eventShapeSGKey).isFailure()) {
   ATH_MSG_ERROR("Could not recover eventShape with storegate key" <<
                 m_eventShapeSGKey);
   return HLT::ERROR;
  }
 
 // m_energyDensity has been declared to be  monitored variable
 double energyDensity;
 if (!eventShape->getDensity(xAOD::EventShape::Density,  energyDensity)) {
   ATH_MSG_ERROR("  Event density not found in container.");
   return HLT::ERROR;
 }
 m_energyDensity = energyDensity; //  double -> float
 ATH_MSG_DEBUG("EventDensity " << m_energyDensity);
 
  delete indexMap;
  
  return HLT::OK;
}


HLT::ErrorCode 
TrigHLTEnergyDensity::getPseudoJets(const xAOD::CaloClusterContainer* ic,
                                    LabelIndex* indexMap,
                                    PseudoJetVector& pjv){

  // convert elements of DataVector<CaloCluster> to pseudojets
  // after switching the state of the CaloCluster objects to
  // the calibration determined by clusterCalib, which is
  // set when the Algorithm is configured.

  auto uncalibrated = m_clusterCalib == "EM" ? true : false;

  indexMap->addLabel(m_clusterCalib + "Topo");
  AnyToPseudoJet<xAOD::CaloClusterContainer::const_value_type> apj(indexMap);
  
  ClusterToPseudoJetConverter converter(apj, uncalibrated);

  // create the pseudojets
  std::transform(ic->cbegin(),
                 ic->cend(),
                 std::back_inserter(pjv),
                 converter);

  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
  for(auto ps : pjv) {ATH_MSG_VERBOSE("PseudoJetDump " << ps.Et() << " "
                                      << std::boolalpha
                                      << uncalibrated << " "
                                      << m_clusterCalib);}

  return HLT::OK;
}



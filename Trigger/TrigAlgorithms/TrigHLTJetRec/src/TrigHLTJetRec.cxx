/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRec:Trigger algorithm that reads CaloClausters from
// an incoming trigger element, converts them to pseudojets, and processes them
// with the offline JetRecTool object. The resulting jet collection is
// attatched to the outgoing trigger element.
//
// JetRecTool itself is a collection of "lower tools", and is configured
// as part of the TrigHLTJetRec configuration

#include <vector>
#include <string>
#include "TrigHLTJetRec/TrigHLTJetRec.h"
// #include "JetInterface/IJetBuildTool.h"
#include "JetRec/JetRecTool.h"
#include "TrigHLTJetRec/ITriggerPseudoJetGetter.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "JetEDM/LabelIndex.h"
#include "./ClusterToPseudoJet.h"

using jet::LabelIndex;

const xAOD::JetContainer*  make_empty_jetcontainer(){
  xAOD::JetContainer* j_container = new xAOD::JetContainer;
  j_container->setStore(new xAOD::JetTrigAuxContainer);
  return j_container;
}


TrigHLTJetRec::TrigHLTJetRec(const std::string& name, 
                             ISvcLocator* pSvcLocator ):
  HLT::FexAlgo( name, pSvcLocator ) {
  declareProperty( "jetBuildTool", m_jetbuildTool);
  declareProperty( "pseudoJetGetter", m_pseudoJetGetter);
  declareProperty( "cluster_calib", m_clusterCalib);
  declareProperty( "output_collection_label", m_outputCollectionLabel);
}


TrigHLTJetRec::~TrigHLTJetRec() { }


HLT::ErrorCode TrigHLTJetRec::hltInitialize() {
  /* Retrieve the JetRecTool and, separately, its PSeudoJetGetter lower tool
     from the ToolSvc. The python configuration code ensures that ToolSvc
     can access these tool. */

  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_jetbuildTool.retrieve();

  if (sc.isSuccess()) {
    ATH_MSG_INFO("Retrieved  jetBuildTool "
                 <<  m_jetbuildTool -> name());
  }else{
    ATH_MSG_ERROR("Unable to retrieve the jetBuildTool.");
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

  ATH_MSG_INFO("    " << m_jetbuildTool->name());
  m_jetbuildTool->print();

  ATH_MSG_INFO("  Shared PseudojetGetter:");
  ATH_MSG_INFO("    " << m_pseudoJetGetter->name());

  return HLT::OK;
}


HLT::ErrorCode TrigHLTJetRec::hltFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}

HLT::ErrorCode TrigHLTJetRec::hltExecute(const HLT::TriggerElement* inputTE,
                                         HLT::TriggerElement* outputTE) {  
  ATH_MSG_VERBOSE("Executing " << name() << "...");
  ATH_MSG_DEBUG("Executing tool " << m_jetbuildTool->name());

  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

  // Get CaloClusterContainer from input trigger element
  const xAOD::CaloClusterContainer* clusterContainer = 0;
  HLT::ErrorCode hltStatus;
  hltStatus = getFeature(inputTE, clusterContainer);  

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

  PseudoJetVector pjv;

  // setup LabelIndex: m_clusterCalib = "LC" or"EM"
  
  LabelIndex* indexMap = new LabelIndex("PseudoJetLabelMapTrigger");
  indexMap->addLabel(m_clusterCalib + "Topo");

  // setup CaloCluster to PseudoJet convertor
  ClusterToPseudoJet ctpj(indexMap);

  // convert incoming calo clusters to the pseudo jets needed by jetrec
  std::transform(clusterContainer->begin(),
                 clusterContainer->end(), 
                 std::back_inserter(pjv),
                 ctpj);

  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
  // Load the pseudo jets into the TriggerSPseudoJetGetter tool
  // Despite the name, we push the pseudojets into the tool. This is
  // in contrast to offline PseudoJetGetters which fetch the pseudojets.
  m_pseudoJetGetter->prime(&pjv);  
  m_pseudoJetGetter->print();

  const xAOD::JetContainer* j_container;
  ATH_MSG_DEBUG("Executing tool " << m_jetbuildTool->name());
  j_container = m_jetbuildTool->build();

  if (j_container == 0){
    ATH_MSG_ERROR("JetRecTool fail (returned 0 ptr");
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG(j_container->size() << " jets reconstructed");

  delete indexMap;
  hltStatus = attachJetCollection(outputTE, j_container);
  return hltStatus;
}


HLT::ErrorCode
TrigHLTJetRec::attachJetCollection(HLT::TriggerElement* outputTE,
                                   const xAOD::JetContainer* j_container){

  // We have to explicitly delete the aux store, so get a pointer to it.
  auto auxStore = j_container->getStore();
  std::string key = "";

  // label the jet collection according to the sequence.
  // This results in a container name like:
  // xAOD::JetContainer_v1#HLT_a4tcemFS
  HLT::ErrorCode hltStatus = 
    recordAndAttachFeature(outputTE,
                           j_container,
                           key,
                           m_outputCollectionLabel);
  
  // cleanup
  if (hltStatus != HLT::OK) {
    // this is unrecoverable. report error, and do not try to clean up.
    ATH_MSG_ERROR("Failed to attach xAOD::JetContainer into outputTE- status"
                  << hltStatus);
  } else {
    // j_container.setStore(0) is called in downstream code, so the
    // aux container needs to deleted explcitly here.
    delete auxStore;
    ATH_MSG_DEBUG("Attached xAOD::JetContainer into outputTE");
  }
  return hltStatus;
}


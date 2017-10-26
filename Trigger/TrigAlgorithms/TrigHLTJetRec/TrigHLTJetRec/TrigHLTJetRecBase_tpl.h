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
#include "JetRec/JetRecTool.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "JetEDM/LabelIndex.h"
#include "TrigHLTJetRec/AnyToPseudoJet.h"
#include "TrigHLTJetRec/IPseudoJetSelector.h"
#include "TrigHLTJetRec/ITriggerPseudoJetGetter.h"

template<typename InputContainer>
TrigHLTJetRecBase<InputContainer>::TrigHLTJetRecBase(const std::string& name, 
                                                     ISvcLocator* pSvcLocator):
HLT::FexAlgo( name, pSvcLocator ) {
  declareProperty( "jetBuildTool", m_jetbuildTool);
  declareProperty( "pseudoJetGetter", m_pseudoJetGetter);
  declareProperty( "cluster_calib", m_clusterCalib);
  declareProperty( "output_collection_label", m_outputCollectionLabel);
  declareProperty( "pseudojet_labelindex_arg", m_pseudoJetLabelIndexArg);
  declareProperty( "iPseudoJetSelector", m_IPseudoJetSelector);
  declareProperty( "secondary_label", m_secondarylabel);
}


template<typename InputContainer>
TrigHLTJetRecBase<InputContainer>::~TrigHLTJetRecBase() { }


template<typename InputContainer>
HLT::ErrorCode TrigHLTJetRecBase<InputContainer>::hltInitialize() {
  /* Retrieve the JetRecTool and, separately, its PSeudoJetGetter lower tool
     from the ToolSvc. The python configuration code ensures that ToolSvc
     can access these tool. */

  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  auto sc = m_jetbuildTool.retrieve();
  
  if (sc.isSuccess()) {
    ATH_MSG_INFO("Retrieved  jetBuildTool "
                 <<  m_jetbuildTool -> name());
  }else{
    ATH_MSG_ERROR("Unable to retrieve the jetBuildTool.");
    return HLT::ERROR;
  }

  if  (m_IPseudoJetSelector.retrieve().isSuccess()){
    ATH_MSG_INFO("Retrieved IPseudoJetSelector "
                 <<  m_IPseudoJetSelector->name());
  } else {
    ATH_MSG_ERROR("Unable to retrieve the IPseudoJetSelector");
    return HLT::ERROR;
  }

  if  (m_pseudoJetGetter.retrieve().isSuccess()){
      ATH_MSG_INFO("Retrieved  shared PseudoJetGetter "
                   <<  m_pseudoJetGetter->name());
  } else {
    ATH_MSG_ERROR("Unable to retrieve shared PseudoJetGetter");
    return HLT::ERROR;
  }

  ATH_MSG_INFO("Tool retrieval completed.");

  ATH_MSG_INFO("    " << m_jetbuildTool->name());
  m_jetbuildTool->print();

  ATH_MSG_INFO("  Shared PseudojetGetter:");
  ATH_MSG_INFO("    " << m_pseudoJetGetter->name());

  return HLT::OK;
}


template<typename InputContainer>
HLT::ErrorCode TrigHLTJetRecBase<InputContainer>::hltFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}

template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::hltExecute(const HLT::TriggerElement*
                                              inputTE,
                                              HLT::TriggerElement* outputTE) {
  
  /* Retrieve the input container (of trigger Towers,
   * CaloClusters...) of type given by the template arguement. This
   * containees must be of type IParticle. Select the inputs
   * using an AlgTool. Convert the selected inputs to pseudojets.
   * In case of calo clusters, the secondary label is checked for secondary pseudojets.
   * Pass these to the jet build tool. Attach the resulting jet
   * container to the output trigger element.
   */

  ATH_MSG_VERBOSE("Executing " << name() << "...");
  ATH_MSG_DEBUG("Executing tool " << m_jetbuildTool->name());
  
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

  const InputContainer* inContainer = nullptr;
  auto status = this -> getInputContainer(outputTE, inContainer);

  if (status == HLT::OK) {
    ATH_MSG_DEBUG("Obtained jet reconstruction inputs " 
                  << inContainer->size());
  } else {
    ATH_MSG_ERROR("Failed to obtain jet reconstruction constituents");
    return status;
  }


  //convert selected inputs to pseudojets
  // LabelIndex* indexMap = new LabelIndex("PseudoJetLabelMapTrigger");
  jet::LabelIndex* indexMap = new jet::LabelIndex(m_pseudoJetLabelIndexArg);
  jet::PseudoJetVector pjv_in;
  
  status = this -> getPseudoJets(inContainer, indexMap, pjv_in);
  if (status == HLT::OK) {
    ATH_MSG_DEBUG("Obtained pseudojets");
  } else {
    ATH_MSG_ERROR("Failed to get pseudojets ");
    return status;
  }

  // select inputs
  PseudoJetVector pjv;
  auto sc = m_IPseudoJetSelector->select(pjv_in, pjv);

  if (sc == StatusCode::SUCCESS) {
    ATH_MSG_DEBUG("Selected jet reconstruction inputs "
                  << pjv.size());
  } else {
    ATH_MSG_ERROR("Failed to select jet reconstruction constituents");
    return HLT::ERROR;
  }

  for(auto p: pjv)
    {
      ATH_MSG_DEBUG("pseudojet "  
                    << " E " 
                    << p.e() << " " 
                    <<p.eta() );
    }
 

  // Load the pseudo jets into the TriggerSPseudoJetGetter tool
  // Despite the name, we push the pseudojets into the tool. This is
  // in contrast to offline PseudoJetGetters which fetch the pseudojets.
  m_pseudoJetGetter->prime(&pjv);  
  m_pseudoJetGetter->print();

  // 
  // secondary pseudojet set up if secondary exists (at the moment only for clusters)
  //
  jet::PseudoJetVector pjv_secondary;
  status = this->checkforSecondaryPseudoJets(outputTE, indexMap, pjv_secondary);
  
  ATH_MSG_DEBUG("Executing tool " << m_jetbuildTool->name());

  // auto j_container = m_jetbuildTool->build();
  //auto j_container = defaultBuild();
  auto j_container = build();
  

  if (j_container == nullptr){
    ATH_MSG_ERROR("JetRecTool fail (returned 0 ptr");
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG(j_container->size() << " jets reconstructed");
  unsigned int j_count{0};
  for(auto j: *j_container)
    {
      /*
      ATH_MSG_VERBOSE("EMScale E " 
                      << (j->getAttribute<xAOD::JetFourMom_t>("JetEMScaleMomentum")).E());
      ATH_MSG_VERBOSE("Constit E " 
                      << j->getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum").E());
      */
      ++j_count;
  
      ATH_MSG_DEBUG("jet "  
                      << j_count 
                      << " E " 
                      << j->e() << " " 
                      <<j->eta() );

  } 

  delete indexMap;

  status = attachJetCollection(outputTE, j_container);
  return status;
}


template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::attachJetCollection(HLT::TriggerElement*
                                                       outputTE,
                                                       const xAOD::JetContainer*
                                                       j_container){
  
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


template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::getInputContainer(const HLT::TriggerElement*
                                                     outputTE,
                                                     const InputContainer*&
                                                     inContainer){

  // Get the input Container from input trigger element

  // etablish the contianer type to retrieve from the trigger element
  auto hltStatus = getFeature(outputTE, inContainer);  

  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved the input container at address " << inContainer);
  } else {
    ATH_MSG_ERROR("Failed to retrieve the input container");
    return HLT::ERROR;
  }

  if (inContainer == nullptr){
    ATH_MSG_ERROR("The input container address = nullptr, giving up");
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG("Number of incoming objects : " << inContainer->size());

  return HLT::OK;
}


template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::getPseudoJets(const InputContainer* ic,
                                                 jet::LabelIndex* indexMap,
                                                 jet::PseudoJetVector& pjv){

  indexMap->addLabel(m_clusterCalib + "Topo");

  // convert IParticle objects to the fastjet::Pseudojets.
  // setup input object to PseudoJet convertor
  AnyToPseudoJet<typename InputContainer::const_value_type> 
    toPseudoJet(indexMap);

  // create the pseudojets
  std::transform(ic->cbegin(),
                 ic->cend(),
                 std::back_inserter(pjv),
                 toPseudoJet);

  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
  return HLT::OK;
}

template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::checkforSecondaryPseudoJets(
                                                 const HLT::TriggerElement* /*unused*/, 
                                                 jet::LabelIndex* /*unused*/,
                                                 jet::PseudoJetVector& /*unused*/){
  ATH_MSG_INFO("No actions for loading of secondary pseudojets as input not of type calo cluster.");
  return HLT::OK;
}

template<typename InputContainer>
const xAOD::JetContainer*
TrigHLTJetRecBase<InputContainer>::defaultBuild() const{
  return m_jetbuildTool->build();

}

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
#include "fastjet/ClusterSequence.hh"


template<typename InputContainer>
TrigHLTJetRecBase<InputContainer>::TrigHLTJetRecBase(const std::string& name, 
                                                     ISvcLocator* pSvcLocator):
HLT::FexAlgo( name, pSvcLocator ) {
  declareProperty( "jetBuildTool", m_jetBuildTool);
  // declareProperty( "pseudoJetGetter", m_pseudoJetGetter);
  declareProperty( "cluster_calib", m_clusterCalib);
  declareProperty( "output_collection_label", m_outputCollectionLabel);
  // declareProperty( "pseudojet_labelindex_arg", m_pseudoJetLabelIndexArg);
  // declareProperty( "iPseudoJetSelector", m_IPseudoJetSelector);
  declareProperty( "secondary_label", m_secondarylabel);
  // declareProperty( "secondarypseudoJetGetter", m_secondarypseudoJetGetter = NULL);
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
  auto sc = m_jetBuildTool.retrieve();
  
  if (sc.isSuccess()) {
    ATH_MSG_INFO("Retrieved  jetBuildTool "
                 <<  m_jetBuildTool -> name());
    m_jetBuildTool->print();
  }else{
    ATH_MSG_ERROR("Unable to retrieve the jetBuildTool.");
    return HLT::ERROR;
  }

  //PS 3/4/18  if  (m_IPseudoJetSelector.retrieve().isSuccess()){
  //PS 3/4/18    ATH_MSG_INFO("Retrieved IPseudoJetSelector "
  //PS 3/4/18                 <<  m_IPseudoJetSelector->name());
  //PS 3/4/18  } else {
  //PS 3/4/18    ATH_MSG_ERROR("Unable to retrieve the IPseudoJetSelector");
  //PS 3/4/18    return HLT::ERROR;
  //PS 3/4/18  }

  //PS 3/4/18  if  (m_pseudoJetGetter.retrieve().isSuccess()){
  //PS 3/4/18      ATH_MSG_INFO("Retrieved  shared PseudoJetGetter "
  //PS 3/4/18                   <<  m_pseudoJetGetter->name());
  //PS 3/4/18  } else {
  //PS 3/4/18    ATH_MSG_ERROR("Unable to retrieve shared PseudoJetGetter");
  //PS 3/4/18    return HLT::ERROR;
  //PS 3/4/18  }

  //PS 3/4/18   ATH_MSG_DEBUG("checking secondary label for secondary pseudojet getter...");
  //PS 3/4/18   if ( !secondaryLabelisEmpty() ){
  //PS 3/4/18	ATH_MSG_DEBUG("Retrieving secondary pseudojet getter...");
  //PS 3/4/18	if  (m_secondarypseudoJetGetter.retrieve().isSuccess()){
  //PS 3/4/18      		ATH_MSG_DEBUG("Retrieved  secondary PseudoJetGetter "
  //PS 3/4/18                   <<  m_secondarypseudoJetGetter->name());
  //PS 3/4/18  	} else {
  //PS 3/4/18    		ATH_MSG_ERROR("Unable to retrieve secondary PseudoJetGetter");
  //PS 3/4/18    		return HLT::ERROR;
  //PS 3/4/18  	}
  //PS 3/4/18   }
  //PS 3/4/18   else {
  //PS 3/4/18	ATH_MSG_DEBUG("No secondary PseudojetGetter required. Will disable the tool handle.");
  //PS 3/4/18	m_secondarypseudoJetGetter.disable();
  //PS 3/4/18   }

  ATH_MSG_INFO("Tool retrieval completed.");

  //PS 3/4/18  ATH_MSG_INFO("  Shared PseudojetGetter:");
  //PS 3/4/18  ATH_MSG_INFO("    " << m_pseudoJetGetter->name());

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
  ATH_MSG_DEBUG("Executing tool " << m_jetBuildTool->name());
  
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

  // clear the jet build tools
  m_jetBuildTool->resetInputs();
  
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
  //PS 3/4/18 jet::LabelIndex* indexMap = new jet::LabelIndex(m_pseudoJetLabelIndexArg);
  //PS 3/4/18 jet::PseudoJetVector pjv_in;
  
  //PS 3/4/18 status = this -> getPseudoJets(inContainer, indexMap, pjv_in);
  //PS 3/4/18 if (status == HLT::OK) {
  //PS 3/4/18   ATH_MSG_DEBUG("Obtained pseudojets");
  //PS 3/4/18 } else {
  //PS 3/4/18   ATH_MSG_ERROR("Failed to get pseudojets ");
  //PS 3/4/18   return status;
  //PS 3/4/18 }

  //PS 3/4/18 // select inputs
  //PS 3/4/18 PseudoJetVector pjv;
  //PS 3/4/18 auto sc = m_IPseudoJetSelector->select(pjv_in, pjv);

  //PS 3/4/18  if (sc == StatusCode::SUCCESS) {
  //PS 3/4/18   ATH_MSG_DEBUG("Selected jet reconstruction inputs "
  //PS 3/4/18                 << pjv.size());
  //PS 3/4/18 } else {
  //PS 3/4/18   ATH_MSG_ERROR("Failed to select jet reconstruction constituents");
  //PS 3/4/18   return HLT::ERROR;
  //PS 3/4/18 }

  //PS 3/4/18 for(auto p: pjv)
  //PS 3/4/18   {
  //PS 3/4/18     ATH_MSG_DEBUG("pseudojet "  
  //PS 3/4/18                   << " E " 
  //PS 3/4/18                   << p.e() << " " 
  //PS 3/4/18                   <<p.eta() );
  //PS 3/4/18   }
 

  // Load the pseudo jets into the TriggerSPseudoJetGetter tool
  // Despite the name, we push the pseudojets into the tool. This is
  // in contrast to offline PseudoJetGetters which fetch the pseudojets.
  //PS 3/4/18 m_pseudoJetGetter->prime(&pjv);  
  //PS 3/4/18 m_pseudoJetGetter->print();

  // 
  // secondary pseudojet set up if secondary exists (at the moment only for clusters)
  //
  //PS 3/4/18 jet::PseudoJetVector pjv_secondary;
  //PS 3/4/18 status = this->checkforSecondaryPseudoJets(outputTE, indexMap, pjv_secondary);
  
  //PS 3/4/18 ATH_MSG_DEBUG("Executing tool " << m_jetbuildTool->name());

  // auto j_container = m_jetbuildTool->build();
  //auto j_container = defaultBuild();

  m_jetBuildTool->prime(inContainer);
  ATH_MSG_DEBUG("Primed  jet build Tool");


  if ( !secondaryLabelisEmpty() && secondaryLabelisTracks()) {
  	// adding secondary ghost TRACK pseudojet container.
  	const xAOD::TrackParticleContainer* ghostTrackInContainer = nullptr;
  	//const ghostInputContainer* ghostInContainer = nullptr;
	ATH_MSG_DEBUG("Retrieving secondary input container.");
	status = this -> getGhostInputContainer(outputTE, ghostTrackInContainer);	
 	if (status == HLT::OK) {
 	   ATH_MSG_DEBUG("Obtained ghost inputs " 
 	                 << ghostTrackInContainer->size());
 	} else {
 	   ATH_MSG_ERROR("Failed to obtain ghost track constituents");
 	   return status;
 	}
  	m_jetBuildTool->primeGhost(ghostTrackInContainer, m_secondarylabel);
	ATH_MSG_DEBUG("Created ghost track pseudojet container with constituent label '"<<m_secondarylabel<<"'.");
  }




  // add in tracks if appropriate and configured. Implemented in subclasses
  // addTracks(inputTE);

  ATH_MSG_DEBUG("Executing tool " << m_jetBuildTool->name());
  ATH_MSG_DEBUG(m_jetBuildTool->toString(1));

  fastjet::ClusterSequence* clusterSequence{nullptr};
  xAOD::JetContainer* j_container{nullptr};
  status = build(clusterSequence, j_container);
  if(status != HLT::OK){return HLT::ERROR;}
  

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

  // delete indexMap;

  status = attachJetCollection(outputTE, j_container, clusterSequence);
  return status;
}


template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::attachJetCollection(HLT::TriggerElement*
                                                       outputTE,
                                                       const xAOD::JetContainer*
                                                       j_container,
                                                       const fastjet::ClusterSequence*
                                                       cs){
  
  // We have to explicitly delete the aux store, so get a pointer to it.
  auto auxStore = j_container->getStore();
  std::string key = "";

  // label the jet collection according to the sequence.
  // This results in a container name like:
  // xAOD::JetContainer_v1#HLT_a4tcemFS

  /*
  HLT::ErrorCode hltStatus = 
    recordAndAttachFeature(outputTE,
                           cs,
                           key,
                           m_outputCollectionLabel+"clusterseq");

  if (hltStatus != HLT::OK) {
    // this is unrecoverable. report error, and do not try to clean up.
    ATH_MSG_ERROR("Failed to attach fastjet::ClusterSequence to outputTE "
                  << " - status" << hltStatus);
    return hltStatus;
  }
  */

  delete cs;
  HLT::ErrorCode hltStatus =  recordAndAttachFeature(outputTE,
                                                     j_container,
                                                     key,
                                                     m_outputCollectionLabel);
  
  // cleanup if success - leave mess otherwise
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
template<typename ghostInputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::getGhostInputContainer(const HLT::TriggerElement*
                                                     outputTE,
                                                     const ghostInputContainer*&
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


//PS 3/4/18template<typename InputContainer>
//PS 3/4/18HLT::ErrorCode
//PS 3/4/18TrigHLTJetRecBase<InputContainer>::getPseudoJets(const InputContainer* ic,
//PS 3/4/18                                                 jet::LabelIndex* indexMap,
//PS 3/4/18                                                 jet::PseudoJetVector& pjv){

//PS 3/4/18  indexMap->addLabel(m_clusterCalib + "Topo");

  // convert IParticle objects to the fastjet::Pseudojets.
  // setup input object to PseudoJet convertor
//PS 3/4/18  AnyToPseudoJet<typename InputContainer::const_value_type> 
//PS 3/4/18    toPseudoJet(indexMap);

  // create the pseudojets
//PS 3/4/18  std::transform(ic->cbegin(),
//PS 3/4/18                 ic->cend(),
//PS 3/4/18                 std::back_inserter(pjv),
//PS 3/4/18                 toPseudoJet);

//PS 3/4/18  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
//PS 3/4/18  return HLT::OK;
//PS 3/4/18}

//PS 3/4/18template<typename InputContainer>
//PS 3/4/18HLT::ErrorCode
//PS 3/4/18TrigHLTJetRecBase<InputContainer>::checkforSecondaryPseudoJets(
//PS 3/4/18                                                 const HLT::TriggerElement* /*unused*/, 
                                                 // jet::LabelIndex* /*unused*/,
//PS 3/4/18                                                 jet::PseudoJetVector& /*unused*/){
//PS 3/4/18  ATH_MSG_INFO("No actions for loading of secondary pseudojets as input not of type calo cluster.");
//PS 3/4/18  return HLT::OK;
//PS 3/4/18}

template<typename InputContainer>
HLT::ErrorCode
TrigHLTJetRecBase<InputContainer>::defaultBuild(fastjet::ClusterSequence*& cs,
                                                xAOD::JetContainer*& jc) const{
  int rc =  m_jetBuildTool->build(cs, jc);
  if(rc != 0){
    ATH_MSG_ERROR("Error building jets. Build tool error code "<< rc);
    return HLT::ERROR;
  }
  return HLT::OK;
}

template<typename InputContainer>
const PseudoJetContainer*
TrigHLTJetRecBase<InputContainer>::getPseudoJetContainer() const {
  return m_jetBuildTool->getInputPseudoJetContainer();
}

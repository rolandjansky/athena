/*   
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/

#include "TestTrigAFPDijetHypoAlg.h"
#include "xAODJet/Jet.h"

using TrigCompositeUtils::createAndStore; 
using TrigCompositeUtils::DecisionContainer; 
using TrigCompositeUtils::DecisionIDContainer; 
using TrigCompositeUtils::decisionIDs; 
using TrigCompositeUtils::hypoAlgNodeName; 
using TrigCompositeUtils::linkToPrevious; 
using TrigCompositeUtils::newDecisionIn; 
using TrigCompositeUtils::featureString;

TestTrigAFPDijetHypoAlg::TestTrigAFPDijetHypoAlg(const std::string &name, ISvcLocator *pSvcLocator)
  : ::HypoBase(name, pSvcLocator)
{
}

StatusCode TestTrigAFPDijetHypoAlg::initialize(){

  ATH_MSG_DEBUG("In initialize ...");

  ATH_CHECK(m_inputJetsKey.initialize());
  ATH_CHECK(m_AFPtrackCollectionReadKey.initialize());
  ATH_CHECK(m_hypoTools.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TestTrigAFPDijetHypoAlg::execute(const EventContext &context) const {

  ATH_MSG_DEBUG("Executing ...");

  SG::ReadHandle<DecisionContainer> previousDecisionsHandle = SG::makeHandle(decisionInput(), context);   
  ATH_CHECK(previousDecisionsHandle.isValid());

  auto inputJetsHandle = SG::makeHandle(m_inputJetsKey, context);
  auto inputAFPtracksHandle = SG::makeHandle(m_AFPtrackCollectionReadKey, context);

  ATH_MSG_DEBUG("Jet handle valid:"<<inputJetsHandle.isValid());
  ATH_MSG_DEBUG("AFP handle valid:"<<inputAFPtracksHandle.isValid());

  ATH_MSG_DEBUG("Jet collection size:"<<inputJetsHandle.cptr()->size());
  ATH_MSG_DEBUG("AFP collection size:"<<inputAFPtracksHandle.cptr()->size());

  TrigCompositeUtils::DecisionIDContainer prev;   
  TrigCompositeUtils::decisionIDs(previousDecisionsHandle->at(0), prev);

  ATH_MSG_DEBUG("Created decisionIDs");

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);   
  auto decisions = outputHandle.ptr();   
  auto d = newDecisionIn(decisions, hypoAlgNodeName());

  ATH_MSG_DEBUG("Created new decisionsIn");

  linkToPrevious(d, decisionInput().key(), 0);   
  if(inputJetsHandle.cptr()->size()!=0) d->setObjectLink(featureString(), ElementLink<xAOD::JetContainer>(*inputJetsHandle, 0, context));
  if(inputAFPtracksHandle.cptr()->size()!=0) d->setObjectLink(featureString(), ElementLink<xAOD::AFPTrackContainer>(*inputAFPtracksHandle, 0, context));

  ATH_MSG_DEBUG("Linked to previous and to containers");

  TestTrigAFPDijetHypoTool::TrigAFPDijetHypoInfo info = {prev,inputJetsHandle.cptr(),inputAFPtracksHandle.cptr(),d};


  for (const auto &tool : m_hypoTools){
    ATH_CHECK(tool->decide(info));   
  } 

  return StatusCode::SUCCESS;
}

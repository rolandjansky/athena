/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigAFPToFHypoAlg.h"

using namespace TrigCompositeUtils;

TrigAFPToFHypoAlg::TrigAFPToFHypoAlg(const std::string &name, ISvcLocator *pSvcLocator): 
  ::HypoBase(name, pSvcLocator)
{
}

StatusCode TrigAFPToFHypoAlg::initialize()
{
  ATH_MSG_DEBUG("TrigAFPToFHypoAlg::initialize()");
  ATH_CHECK(m_inputPrmVtxKey.initialize()); //Dijet vertex
  renounce( m_inputPrmVtxKey ); //Temporary fix from https://gitlab.cern.ch/atlas/athena/-/merge_requests/52250#note_5517376
  ATH_CHECK(m_inputAfpVtxKey.initialize()); //ToF vertex
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TrigAFPToFHypoAlg::execute(const EventContext &context) const
{
  ATH_MSG_DEBUG("TrigAFPToFHypoAlg::execute()");

  // Create the input and output decision containers
  SG::ReadHandle<DecisionContainer> inputDecisionsHandle(decisionInput(), context);
  SG::WriteHandle<DecisionContainer> outputDecisionsHandle = createAndStore(decisionOutput(), context ); 

  // Jet vertex container
  SG::ReadHandle<xAOD::VertexContainer> prmVtxHandle(m_inputPrmVtxKey, context);
  //ATH_CHECK(prmVtxHandle.isValid());

  // AFP ToF vertex container
  SG::ReadHandle<xAOD::AFPVertexContainer> afpVtxHandle(m_inputAfpVtxKey, context);
  ATH_CHECK(afpVtxHandle.isValid());

  ATH_MSG_DEBUG("Found " << inputDecisionsHandle->size() << " previous decisions.");
  if(prmVtxHandle.isValid()) ATH_MSG_DEBUG("Found " << prmVtxHandle->size()         << " primary vertices."  );
  else ATH_MSG_DEBUG("WARNING: prmVtxHandle is not valid." );
  ATH_MSG_DEBUG("Found " << afpVtxHandle->size()         << " AFP ToF vertices."  );

  // Vector of structs to pass to hypo tool
  std::vector<TrigAFPToFHypoTool::AFPToFHypoToolInfo> hypoToolInput;

  for (size_t i = 0; i < inputDecisionsHandle->size(); ++i) {
    const Decision* previousDecision = inputDecisionsHandle->at(i);
    
    // Placeholder for now: Only compare first ToF vertex and first jet vertex in containers
    // TO-DO: Need to compare DIJET vertex to all ToF vertices? Then keep if dijet vertiex is compatible with any ToF vertex?
    // Eventually will need to know exactly which protons passed the kinematic matching in the dijet hypo and only compare this ToF vertex


    if(!prmVtxHandle.isValid() || afpVtxHandle->size() == 0) continue; //How do I properly deal with decisions when there are no AFP vertices to make a decision on?
    if(prmVtxHandle->size() == 0) continue;

    const xAOD::AFPVertex* afpVtx = afpVtxHandle->at(0);
    const xAOD::Vertex* prmVtx = prmVtxHandle->at(0);

    // Create an output Decision Object to track this object in the trigger navigation.
    Decision* outputDecision = newDecisionIn(outputDecisionsHandle.ptr(), hypoAlgNodeName());

    // We link this 'outputDecision' to its parent, 'inputDecision'
    linkToPrevious(outputDecision, previousDecision, context);

    // We attach the "feature" ElementLink which associates the DecisionObject with the AFP vertex
    outputDecision->setObjectLink(featureString(), ElementLink<xAOD::AFPVertexContainer>(*afpVtxHandle, i, context));

    // We collect the set of chains which were active on this inputDecision coming in to this Step.
    DecisionIDContainer inputPassingChains;
    decisionIDs(previousDecision, inputPassingChains);

    hypoToolInput.emplace_back(inputPassingChains, afpVtx, prmVtx, outputDecision);
  }

  // Each individual chain has its own HypoTool, we loop over them and give them the hypoToolInput to discriminate over
  for (const auto &tool : m_hypoTools) {
    ATH_CHECK(tool->decide(hypoToolInput));
  }

  // Performs some runtime-checks.
  ATH_CHECK( hypoBaseOutputProcessing(outputDecisionsHandle) );

  return StatusCode::SUCCESS;
}

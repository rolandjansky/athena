/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SPCountHypoAlgMT.h"

using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::featureString;

SPCountHypoAlgMT::SPCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode SPCountHypoAlgMT::initialize()
{

  ATH_CHECK(m_spacePointsKey.initialize());

  if (m_spacePointsKey.key() == "Undefined")
  {
ATH_MSG_ERROR("SpacePoint Key name undefined " );
return StatusCode::FAILURE;

  }
  ATH_CHECK(m_hypoTools.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode SPCountHypoAlgMT::execute(const EventContext& context) const
{
  SG::ReadHandle<xAOD::TrigCompositeContainer> SPHandle (m_spacePointsKey, context );
  ATH_MSG_DEBUG ("Size = "<<SPHandle->size());

  if (SPHandle->size() != 1) {
    ATH_MSG_ERROR("Found " << SPHandle->size() <<" Space Point decesions of size other than 1.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::FAILURE;
  }

  if (previousDecisionsHandle->size() == 0) {
    ATH_MSG_DEBUG( "No previous decision, nothing to do.");
    return StatusCode::SUCCESS;
  } else if (previousDecisionsHandle->size() > 1) {
    ATH_MSG_ERROR("Found " << previousDecisionsHandle->size() <<" previous decisions. SPCountHypoAlgMT is a full-scan HypoAlg and expects exactly one previous decision.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  TrigCompositeUtils::Decision* d = newDecisionIn( decisions, previousDecisionsHandle->at(0), "", context);
  d->setObjectLink<xAOD::TrigCompositeContainer>(featureString(), ElementLink<xAOD::TrigCompositeContainer>(*SPHandle, 0) );

  linkToPrevious( d, decisionInput().key(), 0 );

  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs( previousDecisionsHandle->at(0), prev );
  SPCountHypoTool::SPCountsInfo spinfo({d, SPHandle->at(0), prev});

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(spinfo));
  }
  //ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

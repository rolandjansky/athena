/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SPCountHypoAlg.h"
#include "AthViews/ViewHelper.h"

using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::hypoAlgNodeName;

SPCountHypoAlg::SPCountHypoAlg(const std::string &name, ISvcLocator *pSvcLocator) : ::HypoBase(name, pSvcLocator)
{
}
StatusCode SPCountHypoAlg::initialize()
{

  ATH_CHECK(m_spacePointsKey.initialize());
  ATH_CHECK(m_hypoTools.retrieve());
  renounce(m_spacePointsKey);
  return StatusCode::SUCCESS;
}

StatusCode SPCountHypoAlg::execute(const EventContext &context) const
{

  ATH_MSG_DEBUG("Executing " << name() << "...");

  auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context);
  ATH_CHECK(previousDecisionsHandle.isValid());

  if (previousDecisionsHandle->size() > 1)
  {
    ATH_MSG_ERROR("Found " << previousDecisionsHandle->size() << " previous decisions. SPCountHypoAlg is a full-scan HypoAlg and expects exactly one previous decision.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Running with " << previousDecisionsHandle->size() << " implicit ReadHandles for previous decisions");

  const auto viewSP = (previousDecisionsHandle->at(0))->objectLink<ViewContainer>(viewString());
  ATH_CHECK(viewSP.isValid());
  auto spacepointsHandle = ViewHelper::makeHandle(*viewSP, m_spacePointsKey, context);
  ATH_CHECK(spacepointsHandle.isValid());
  ATH_MSG_DEBUG("spacepoint handle size: " << spacepointsHandle->size() << "...");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  auto decisions = outputHandle.ptr();
  auto d = newDecisionIn(decisions, hypoAlgNodeName());
  d->setObjectLink(featureString(), ViewHelper::makeLink<xAOD::TrigCompositeContainer>(*viewSP, spacepointsHandle, 0));
  linkToPrevious(d, decisionInput().key(), 0);

  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs(previousDecisionsHandle->at(0), prev);

  SPCountHypoTool::SPCountsInfo spinfo({d, spacepointsHandle->at(0), prev});

  for (auto &tool : m_hypoTools)
  {
    ATH_CHECK(tool->decide(spinfo));
  }
  return StatusCode::SUCCESS;
}

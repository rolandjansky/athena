/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MbtsHypoAlg.h"
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::newDecisionIn;

MbtsHypoAlg::MbtsHypoAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : ::HypoBase(name, pSvcLocator)
{
}

MbtsHypoAlg::~MbtsHypoAlg()
{
}

StatusCode MbtsHypoAlg::initialize()
{
  ATH_CHECK(m_hypoTools.retrieve());
  ATH_CHECK(m_MbtsBitsKey.initialize());
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MbtsHypoAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode MbtsHypoAlg::execute(const EventContext &context) const
{
  ATH_MSG_DEBUG("Executing ...");

  SG::ReadHandle<DecisionContainer> previousDecisionsHandle = SG::makeHandle(decisionInput(), context);
  ATH_CHECK(previousDecisionsHandle.isValid());
  auto bitsHandle = SG::makeHandle(m_MbtsBitsKey, context);
  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs(previousDecisionsHandle->at(0), prev);

  ATH_MSG_DEBUG("Obtained input");

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  auto decisions = outputHandle.ptr();
  auto d = newDecisionIn(decisions, hypoAlgNodeName());
  linkToPrevious(d, decisionInput().key(), 0);

  MbtsHypoTool::MbtsHypoInfo info = {prev, bitsHandle.cptr()->at(0), d};

  for (const auto &tool : m_hypoTools)
  {
    ATH_CHECK(tool->decide(info));
  }

  return StatusCode::SUCCESS;
}

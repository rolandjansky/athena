/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigZVertexHypoAlg.h"
#include "AthViews/ViewHelper.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;

TrigZVertexHypoAlg::TrigZVertexHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  ::HypoBase(name, pSvcLocator)
{
}

TrigZVertexHypoAlg::~TrigZVertexHypoAlg()
{
}

StatusCode TrigZVertexHypoAlg::initialize()
{
  ATH_CHECK(m_hypoTools.retrieve());
  ATH_CHECK(m_zVertexCollKey.initialize());
  renounce(m_zVertexCollKey);
  return StatusCode::SUCCESS;
}

StatusCode TrigZVertexHypoAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigZVertexHypoAlg::execute(const EventContext& context) const
{

  ATH_MSG_DEBUG("Executing " << name() << "...");
  auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context);

  if (not previousDecisionsHandle.isValid()) {
    ATH_MSG_DEBUG("No implicit RH for previous decisions " << decisionInput().key() << ": is this expected?");
    return StatusCode::SUCCESS;
  }

  if (previousDecisionsHandle->size() == 0) {
    ATH_MSG_DEBUG("No previous decision, nothing to do.");
    return StatusCode::SUCCESS;
  }

  const auto view = (previousDecisionsHandle->at(0))->objectLink<ViewContainer>(viewString());
  ATH_CHECK(view.isValid());
  auto zVertexHandle = ViewHelper::makeHandle(*view, m_zVertexCollKey, context);

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  auto decisions = outputHandle.ptr();

  auto d = newDecisionIn(decisions, hypoAlgNodeName());
  linkToPrevious(d, decisionInput().key(), 0);
  d->setObjectLink(featureString(), ViewHelper::makeLink<xAOD::TrigCompositeContainer>(*view, zVertexHandle, 0));
  
  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs(previousDecisionsHandle->at(0), prev);

  TrigZVertexHypoTool::ZVertexInfo info{ d, zVertexHandle.cptr(), prev };

  for (const auto& tool : m_hypoTools) {
    ATH_CHECK(tool->decide(info));
  }
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}


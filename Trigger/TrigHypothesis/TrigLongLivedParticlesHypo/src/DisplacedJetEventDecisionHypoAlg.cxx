/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DisplacedJetEventDecisionHypoAlg.h"
#include "DisplacedJetDispHypoTool.h"
#include "AthViews/ViewHelper.h"

using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::hypoAlgNodeName;

DisplacedJetEventDecisionHypoAlg::DisplacedJetEventDecisionHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode DisplacedJetEventDecisionHypoAlg::initialize()
{

  ATH_CHECK(m_hypoTools.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode DisplacedJetEventDecisionHypoAlg::execute(const EventContext& context) const
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );

  ATH_CHECK( previousDecisionsHandle.isValid() );

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();
  
  if (previousDecisionsHandle->size() == 0) {
    ATH_MSG_DEBUG( "No previous decision, nothing to do.");
    return StatusCode::SUCCESS;
  }

  std::vector<DisplacedJetEventDecisionHypoTool::DecisionTuple> tuples;

  for(const TrigCompositeUtils::Decision* previousDecision: *previousDecisionsHandle){
    const auto viewELInfo = findLink< ViewContainer >( previousDecision, viewString() );
    ATH_CHECK( viewELInfo.isValid() );

    TrigCompositeUtils::Decision* d = newDecisionIn(decisions, previousDecision, hypoAlgNodeName(), context);

    TrigCompositeUtils::DecisionIDContainer prev;
    TrigCompositeUtils::decisionIDs( previousDecision, prev );

    //get the linked jet feature
    //for safety check that I have 1 jet by getting all links
    std::vector<TrigCompositeUtils::LinkInfo<xAOD::JetContainer>> jet_feature_links = TrigCompositeUtils::findLinks<xAOD::JetContainer>(previousDecision, TrigCompositeUtils::featureString());
    ATH_CHECK(jet_feature_links.size() == 1); //ensure we only have 1 link
    const TrigCompositeUtils::LinkInfo<xAOD::JetContainer> jet_feature_link = jet_feature_links.at(0);
    //verify if the feature link is valid
    ATH_CHECK(jet_feature_link.isValid());
    const xAOD::Jet* jet = *(jet_feature_link.link);

    //reattach jet feature link
    d->setObjectLink(featureString(), jet_feature_link.link);

    //make a tuple for this jet
    DisplacedJetEventDecisionHypoTool::DecisionTuple t{d, prev, jet};

    tuples.push_back(t);
  }

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(tuples));
  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

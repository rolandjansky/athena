/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "DisplacedJetDispHypoAlg.h"
#include "DisplacedJetDispHypoTool.h"
#include "AthViews/ViewHelper.h"
#include "FourMomUtils/P4Helpers.h"

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

DisplacedJetDispHypoAlg::DisplacedJetDispHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode DisplacedJetDispHypoAlg::initialize()
{
  ATH_CHECK(m_lrtTracksKey.initialize());
  renounce(m_lrtTracksKey);
  ATH_CHECK(m_vtxKey.initialize());
  ATH_CHECK(m_beamSpotKey.initialize());

  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode DisplacedJetDispHypoAlg::execute(const EventContext& context) const
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );

  ATH_CHECK( previousDecisionsHandle.isValid() );

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  ATH_MSG_DEBUG("Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions. Looking for :"<<viewString());

  auto vtxHandle = SG::makeHandle(m_vtxKey, context);
  ATH_CHECK( vtxHandle.isValid() );
  const xAOD::VertexContainer* vtxs = vtxHandle.get();

  //get the primary vertex
  const xAOD::Vertex_v1* primary_vertex = nullptr;

  for(auto v: *vtxs){
    if(v->vertexType()==xAOD::VxType::PriVtx){
      primary_vertex = v;
    }
  }

  if(primary_vertex == nullptr){
    ATH_MSG_DEBUG("missing primary vertex");
    return StatusCode::SUCCESS;
  }

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, context };

  ATH_CHECK(beamSpotHandle.isValid());

  DisplacedJetBeamspotInfo beamspot_info(beamSpotHandle.retrieve());

  for(const TrigCompositeUtils::Decision* previousDecision: *previousDecisionsHandle){
    const auto viewELInfo = findLink< ViewContainer >( previousDecision, viewString() );
    ATH_CHECK( viewELInfo.isValid() );

    TrigCompositeUtils::Decision* d = newDecisionIn(decisions, previousDecision, hypoAlgNodeName(), context);

    TrigCompositeUtils::DecisionIDContainer prev;
    TrigCompositeUtils::decisionIDs( previousDecision, prev );

    
    ATH_MSG_DEBUG("Getting LRT Tracks "<<m_lrtTracksKey);

    //get containers
    auto lrtHandle = ViewHelper::makeHandle(*viewELInfo.link, m_lrtTracksKey, context);

    ATH_CHECK( lrtHandle.isValid() );

    const xAOD::TrackParticleContainer* lrtTracks = lrtHandle.get();

    //get the linked jet feature
    std::vector<TrigCompositeUtils::LinkInfo<xAOD::JetContainer>> jet_feature_links = TrigCompositeUtils::findLinks<xAOD::JetContainer>(previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
    ATH_CHECK(jet_feature_links.size() == 1); //ensure we only have 1 link
    const TrigCompositeUtils::LinkInfo<xAOD::JetContainer> jet_feature_link = jet_feature_links.at(0);
    //verify if the feature link is valid
    ATH_CHECK(jet_feature_link.isValid());
    const xAOD::Jet* jet = *(jet_feature_link.link);

    //reattach jet feature link
    d->setObjectLink(featureString(), jet_feature_link.link);

    //get my count object which has been linked to the decision
    auto count_links = TrigCompositeUtils::findLinks<xAOD::TrigCompositeContainer>(previousDecision, "djtrig_counts");
    ATH_CHECK(count_links.size() == 1); //ensure we only have 1 link
    auto count_link = count_links.at(0);
    ATH_CHECK(count_link.isValid());
    const xAOD::TrigComposite* count = *(count_link.link);


    //count the number of tracks in the RoI (at this point it is running in RoI mode)
    //apply the nprompt, nother, ndisp requirements and generated the per jet decision

    DisplacedJetDispHypoTool::Info info{d, prev, jet, lrtTracks, primary_vertex, count, beamspot_info};

    for(auto &tool:m_hypoTools)
    {
      ATH_CHECK(tool->decide(info));
    }
  }


  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

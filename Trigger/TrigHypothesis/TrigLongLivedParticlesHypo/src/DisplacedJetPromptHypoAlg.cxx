/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DisplacedJetPromptHypoAlg.h"
#include "DisplacedJetPromptHypoTool.h"
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

DisplacedJetPromptHypoAlg::DisplacedJetPromptHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode DisplacedJetPromptHypoAlg::initialize()
{
  ATH_CHECK(m_jetContainerKey.initialize());
  ATH_CHECK(m_stdTracksKey.initialize());
  ATH_CHECK(m_vtxKey.initialize());
  ATH_CHECK(m_countsKey.initialize());

  ATH_CHECK(m_hypoTools.retrieve());
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode DisplacedJetPromptHypoAlg::execute(const EventContext& context) const
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );

  ATH_CHECK( previousDecisionsHandle.isValid() );

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  if (previousDecisionsHandle->size() == 0) {
    ATH_MSG_DEBUG( "No previous decision, nothing to do.");
    return StatusCode::SUCCESS;
  } else if (previousDecisionsHandle->size() > 1) {
    ATH_MSG_ERROR("Found " << previousDecisionsHandle->size() <<" previous decisions.");
    return StatusCode::FAILURE;
  }

  auto mon_max_pt = Monitored::Scalar<float>("leading_jet_pt");
  Monitored::Group mon_group(m_monTool, mon_max_pt);

  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs( previousDecisionsHandle->at(0), prev );

  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions. Looking for :"<<viewString());


  ATH_MSG_DEBUG( "Getting Jets Handle "<<m_jetContainerKey);
  ATH_MSG_DEBUG("Getting STD Handle "<<m_stdTracksKey);
  ATH_MSG_DEBUG("Getting vtx Handle "<<m_vtxKey);

  //get containers
  auto jetsHandle = SG::makeHandle(m_jetContainerKey, context);
  auto stdHandle = SG::makeHandle(m_stdTracksKey, context);
  auto vtxHandle = SG::makeHandle(m_vtxKey, context);
  SG::WriteHandle<xAOD::TrigCompositeContainer> countsHandle(m_countsKey, context);

  ATH_CHECK( jetsHandle.isValid() );
  ATH_CHECK( stdHandle.isValid() );
  ATH_CHECK( vtxHandle.isValid() );

  const xAOD::TrackParticleContainer* stdTracks = stdHandle.get();
  const xAOD::JetContainer* jets = jetsHandle.get();
  const xAOD::VertexContainer* vtxs = vtxHandle.get();

  std::map<const xAOD::Jet_v1*, TrigCompositeUtils::Decision*> jet_decisions;
  std::map<const xAOD::Jet_v1*, xAOD::TrigComposite*> jet_counts;
  //get primary vertex
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

  //check if the max(jet-pt) is above the threshold
  double max_pt = 0.0;
  for(auto jet: *jets){
    if(jet->pt() > max_pt){
      max_pt = jet->pt();
    }
  }

  mon_max_pt = max_pt/Gaudi::Units::GeV;

  ATH_MSG_DEBUG("have "<<jets->size()<<" jets");

  if((max_pt/Gaudi::Units::GeV)<m_min_evt_jet_pt){
    //reject event
    //do not add a decision id which is how athena does its rejection
    //this has to happen after the output decision handle has been created (createAndStore)
    ATH_MSG_DEBUG("Event rejected DJ prompt min_evt_pt pT="<<max_pt/Gaudi::Units::GeV);
    return StatusCode::SUCCESS;
  }

  auto countsContainer = std::make_unique< xAOD::TrigCompositeContainer>();
  auto countsContainerAux = std::make_unique< xAOD::TrigCompositeAuxContainer>();
  countsContainer->setStore(countsContainerAux.get());

  std::map<TrigCompositeUtils::Decision*, int> count_index_map;

  //create decision objects for each jet
  //the displaced tracking step wants to run over each jet on its own
  for(size_t jet_idx=0; jet_idx < jets->size(); jet_idx ++){
      TrigCompositeUtils::Decision* jet_dec = newDecisionIn(decisions, hypoAlgNodeName());

      //attach to the previous decision
      linkToPrevious(jet_dec, previousDecisionsHandle->at(0), context);

      //attach the jet
      jet_dec->setObjectLink(featureString(), ElementLink<xAOD::JetContainer>(*jetsHandle, jet_idx, context));

      const xAOD::Jet* jet = jetsHandle->at(jet_idx);

      jet_decisions[jet] = jet_dec;

      //create a counts object
      auto count = new xAOD::TrigComposite();
      auto count_idx = countsContainer->size();

      countsContainer->push_back(count);

      count_index_map[jet_dec]= count_idx;

      jet_counts[jet] = count;
  }

  ATH_CHECK(countsHandle.record(std::move(countsContainer), std::move(countsContainerAux)));

  //link all of the counts to the decisions
  for(auto pair: count_index_map){
    auto jet_dec = pair.first;
    auto idx = pair.second;
    jet_dec->setObjectLink("djtrig_counts", ElementLink<xAOD::TrigCompositeContainer>(*countsHandle, idx, context));
  }

  //do jet<->track association
  std::map<const xAOD::Jet_v1*, std::vector<const xAOD::TrackParticle_v1*>> jets_to_tracks;
  //association is fairly simple loop over particles and find the closest jet, then add it to that jets vector


  for(auto track: *stdTracks){
    if(track->pt()/Gaudi::Units::GeV < m_min_trk_pt) continue;

    const xAOD::Jet_v1* best_jet = nullptr;
    double best_dr = 1000.0;

    for(auto jet: *jets){
      double dr = jet->p4().DeltaR(track->p4());

      if(dr < best_dr && dr <= m_drcut){
        best_jet = jet;
        best_dr = dr;
      }
    }

    if(best_dr <= 0.4){
      //associate track to jet
      jets_to_tracks[best_jet].push_back(track);
    }
  }

  DisplacedJetPromptHypoTool::Info info{prev,jet_decisions, jets, jets_to_tracks, primary_vertex, jet_counts};

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(info));
  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

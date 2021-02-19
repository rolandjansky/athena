/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigLongLivedParticlesHypo/FastTrackFinderLRTHypoAlg.h"
#include "TrigLongLivedParticlesHypo/FastTrackFinderLRTHypoTool.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
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

FastTrackFinderLRTHypoAlg::FastTrackFinderLRTHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode FastTrackFinderLRTHypoAlg::initialize()
{
  ATH_CHECK(m_tracksKey.initialize());
  ATH_CHECK(m_trackCountKey.initialize());
  renounce(m_tracksKey);
  ATH_CHECK(m_min_pt.size()==m_min_d0.size());

  if (m_tracksKey.key() == "Undefined" || m_trackCountKey.key() == "Undefined") {
    ATH_MSG_ERROR("either track Key name or track count key name is undefined " );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_hypoTools.retrieve());
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode FastTrackFinderLRTHypoAlg::execute(const EventContext& context) const
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );

  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;
  }

  if (previousDecisionsHandle->size() == 0) {
    ATH_MSG_DEBUG( "No previous decision, nothing to do.");
    return StatusCode::SUCCESS;
  } else if (previousDecisionsHandle->size() > 1) {
    ATH_MSG_ERROR("Found " << previousDecisionsHandle->size() <<" previous decisions.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions. Looking for :"<<viewString());

  std::vector<int> counts(m_min_pt.size());


  //taken from isohightpt

  ATH_MSG_DEBUG( "Getting Track Handle "<<m_tracksKey);
  // get tracks from the key :
  auto trackHandle = SG::makeHandle(m_tracksKey, context );

  ATH_CHECK( trackHandle.isValid() );
  ATH_MSG_DEBUG ( "track handle size: " << trackHandle->size() << "..." );

  int ntrks = trackHandle->size();

  const xAOD::TrackParticleContainer * AllTracks = trackHandle.get(); //Later this could be updated to run with tracks above a certain threshold

  // Loop over all tracks and get a track decision associated with it and create the input
  for ( const xAOD::TrackParticle_v1* track : *AllTracks) {
    const double pT = track->pt();
    const float d0 = track->d0();
    const double xi2 = 0.0; //fq.chiSquared();
    const float abs_d0 = std::fabs(d0);
    for (long unsigned int i=0;i<m_min_pt.size();i++){
      if(pT >= m_min_pt[i] && abs_d0 < m_max_d0[i] && abs_d0 >= m_min_d0[i] && std::fabs(xi2) < m_max_xi2[i]) counts[i]++;
    }
  }

  //end taken section

  // Recording Data
  auto trackCountContainer = std::make_unique< xAOD::TrigCompositeContainer>();
  auto trackCountContainerAux = std::make_unique< xAOD::TrigCompositeAuxContainer>();
  trackCountContainer->setStore(trackCountContainerAux.get());

  xAOD::TrigComposite * trackCount = new xAOD::TrigComposite();
  trackCountContainer->push_back(trackCount);
  trackCount->setDetail("ntrks", ntrks);
  trackCount->setDetail("pTcuts", static_cast<std::vector<float>>(m_min_pt));
  trackCount->setDetail("d0mincuts", static_cast<std::vector<float>>(m_min_d0));
  trackCount->setDetail("d0maxcuts", static_cast<std::vector<float>>(m_max_d0));
  trackCount->setDetail("xi2cuts", static_cast<std::vector<float>>(m_max_xi2));
  trackCount->setDetail("counts", counts);

  // TODO revisit

  auto mon_ntrks = Monitored::Scalar<int>("ntrks",ntrks);
  Monitored::Group(m_monTool,mon_ntrks);
  for(long unsigned int i=0;i<counts.size();i++){
    auto mon_counts = Monitored::Scalar<int>("counts"+std::to_string(i),counts[i]);
    Monitored::Group(m_monTool,mon_counts);
  }

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  TrigCompositeUtils::Decision* d = newDecisionIn(decisions, previousDecisionsHandle->at(0), hypoAlgNodeName(), context);

  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs( previousDecisionsHandle->at(0), prev );

  FastTrackFinderLRTHypoTool::TrkCountsInfo trkinfo{d, trackCount, prev};

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(trkinfo));
  }

  SG::WriteHandle<xAOD::TrigCompositeContainer> trackCountHandle(m_trackCountKey, context);
  ATH_CHECK(trackCountHandle.record( std::move( trackCountContainer ), std::move( trackCountContainerAux ) ) );
  d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>( m_trackCountKey.key(), 0) );
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

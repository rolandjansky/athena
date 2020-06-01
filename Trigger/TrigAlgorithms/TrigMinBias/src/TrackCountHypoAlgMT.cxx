/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCountHypoAlgMT.h"
#include "TrackCountHypoTool.h"
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

TrackCountHypoAlgMT::TrackCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode TrackCountHypoAlgMT::initialize()
{
  ATH_CHECK(m_tracksKey.initialize());
  ATH_CHECK(m_trackCountKey.initialize());
  renounce(m_tracksKey);
  ATH_CHECK(m_min_pt.size()==m_max_z0.size());

  if (m_tracksKey.key() == "Undefined" || m_trackCountKey.key() == "Undefined") {
    ATH_MSG_ERROR("either track Key name or track count key name is undefined " );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_hypoTools.retrieve());
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TrackCountHypoAlgMT::execute(const EventContext& context) const
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

  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");


  const auto viewTrk = (previousDecisionsHandle->at(0))->objectLink<ViewContainer>( viewString() );
  ATH_CHECK( viewTrk.isValid() );
  auto tracksHandle = ViewHelper::makeHandle( *viewTrk, m_tracksKey, context );
  ATH_CHECK( tracksHandle.isValid() );
  ATH_MSG_DEBUG ( "spacepoint handle size: " << tracksHandle->size() << "..." );

  int ntrks=tracksHandle->size();
  ATH_MSG_DEBUG ("Successfully retrieved track container of size" <<ntrks);

  std::vector<int> counts(m_min_pt.size());
  for (const auto  trackPtr: *tracksHandle){
    const Trk::Perigee& aMeasPer = trackPtr->perigeeParameters();
    const double pT = aMeasPer.pT();
    const double z0 = aMeasPer.parameters()[Trk::z0];

    for (long unsigned int i=0;i<m_min_pt.size();i++){
      if(pT >= m_min_pt[i] && std::fabs(z0) < m_max_z0[i]) counts[i]++;
    }
  }
  for (long unsigned int i=0;i<m_min_pt.size();i++){
    ATH_MSG_DEBUG( "Number of tracks found with pT cut= "<<m_min_pt[i] << " MeV =" <<"and z0 cut= "<<m_max_z0[i] <<  counts[i] );
  }

  // Recording Data
  auto trackCountContainer = std::make_unique< xAOD::TrigCompositeContainer>();
  auto trackCountContainerAux = std::make_unique< xAOD::TrigCompositeAuxContainer>();
  trackCountContainer->setStore(trackCountContainerAux.get());

  xAOD::TrigComposite * trackCount = new xAOD::TrigComposite();
  trackCountContainer->push_back(trackCount);
  trackCount->setDetail("ntrks", ntrks);
  trackCount->setDetail("pTcuts", static_cast<std::vector<float>>(m_min_pt));
  trackCount->setDetail("z0cuts", static_cast<std::vector<float>>(m_max_z0));
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

  auto d = newDecisionIn( decisions);
  linkToPrevious( d, decisionInput().key(), 0 );

  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs( previousDecisionsHandle->at(0), prev );

  TrackCountHypoTool::TrkCountsInfo trkinfo{d, trackCount, prev};

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(trkinfo));
  }

  SG::WriteHandle<xAOD::TrigCompositeContainer> trackCountHandle(m_trackCountKey, context);
  ATH_CHECK(trackCountHandle.record( std::move( trackCountContainer ), std::move( trackCountContainerAux ) ) );
  d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>( m_trackCountKey.key(), 0) );
  //ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

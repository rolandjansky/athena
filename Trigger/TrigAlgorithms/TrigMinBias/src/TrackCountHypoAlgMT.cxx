/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCountHypoAlgMT.h"
#include "TrackCountHypoTool.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::featureString;

TrackCountHypoAlgMT::TrackCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode TrackCountHypoAlgMT::initialize()
{

  ATH_CHECK(m_tracksKey.initialize());
  ATH_CHECK(m_trackCountKey.initialize());
  
  ATH_CHECK(m_min_pt.size()==m_max_z0.size());

  if (m_tracksKey.key() == "Undefined" || m_trackCountKey.key() == "Undefined") {
    ATH_MSG_ERROR("either track Key name or track count key name is undefined " );
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;

  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TrackCountHypoAlgMT::execute(const EventContext& context) const
{
  SG::ReadHandle<xAOD::TrackParticleContainer> TrkHandle (m_tracksKey, context );
  int ntrks=TrkHandle->size();
  ATH_MSG_DEBUG ("Successfully retrieved track container of size" <<ntrks);

  float pT{},z0{};
  std::vector<int> count(m_min_pt.size());
  for (const auto  trackPtr: *TrkHandle){
    const Trk::Perigee& aMeasPer = (trackPtr)->perigeeParameters();
    pT = aMeasPer.pT();
    z0 = aMeasPer.parameters()[Trk::z0];

    for (long unsigned int i=0;i<m_min_pt.size();i++){
      if(pT >= m_min_pt[i] && std::fabs(z0) < m_max_z0[i]) count[i]++;
    }
  }
  for (long unsigned int i=0;i<m_min_pt.size();i++){
    ATH_MSG_DEBUG( "Number of tracks found with pT cut= "<<m_min_pt[i] << " MeV =" <<"and z0 cut= "<<m_max_z0[i] <<  count[i] );
  }

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

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  TrigCompositeUtils::Decision* d = newDecisionIn( decisions, previousDecisionsHandle->at(0), "", context);

  TrigCompositeUtils::DecisionIDContainer prev;
  TrigCompositeUtils::decisionIDs( previousDecisionsHandle->at(0), prev );


  // Recording Data

  auto tracks = std::make_unique< xAOD::TrigCompositeContainer>();
  auto tracksAux = std::make_unique< xAOD::TrigCompositeAuxContainer>();
  tracks->setStore(tracksAux.get());

  xAOD::TrigComposite * trackCounts = new xAOD::TrigComposite();
  tracks->push_back(trackCounts);
  trackCounts->setDetail("ntrks", ntrks);
  trackCounts->setDetail("pTcuts", static_cast<std::vector<float>>(m_min_pt));
  trackCounts->setDetail("z0cuts", static_cast<std::vector<float>>(m_max_z0));
  trackCounts->setDetail("counts", count);

  auto mon_ntrks = Monitored::Scalar<int>("ntrks",ntrks);
  Monitored::Group(m_monTool,mon_ntrks);

  for(long unsigned int i=0;i<=m_min_pt.size();i++){
    auto mon_counts = Monitored::Scalar<int>("counts",count[i]);
    Monitored::Group(m_monTool,mon_counts);
  }

  TrackCountHypoTool::TrkCountsInfo trkinfo{d, tracks->at(0), prev};

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(trkinfo));
  }
  SG::WriteHandle<xAOD::TrigCompositeContainer> TrackCountHandle(m_trackCountKey, context);
  ATH_CHECK(TrackCountHandle.record( std::move( tracks), std::move( tracksAux ) ) );
  d->setObjectLink<xAOD::TrigCompositeContainer>(featureString(), ElementLink<xAOD::TrigCompositeContainer>(m_trackCountKey.key(), 0) );

  //ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}

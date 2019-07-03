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

TrackCountHypoAlgMT::~TrackCountHypoAlgMT()
{
}
StatusCode TrackCountHypoAlgMT::initialize()
{

  ATH_CHECK(m_tracksKey.initialize());
  ATH_CHECK(m_trackCountKey.initialize());

  if (m_tracksKey.key() == "Undefined")
  {
    ATH_MSG_ERROR("track Key name undefined " );
    return StatusCode::FAILURE;

  }
  ATH_CHECK(m_hypoTools.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TrackCountHypoAlgMT::finalize()
{
  return StatusCode::SUCCESS;
}
StatusCode TrackCountHypoAlgMT::execute(const EventContext& context) const
{
  SG::ReadHandle<xAOD::TrackParticleContainer> TrkHandle (m_tracksKey, context );
  int ntrks=TrkHandle->size();
  ATH_MSG_DEBUG ("Successfully retrieved track container of size" <<ntrks);

  float pT{};
  std::vector<int> count(m_min_pt.size());
  for (const auto  trackPtr: *TrkHandle){
    const Trk::Perigee& aMeasPer = (trackPtr)->perigeeParameters();
    pT = aMeasPer.pT()*1e-3;

    for (long unsigned int i=0;i<m_min_pt.size();i++){
      if(pT >= m_min_pt[i] ) count[i]++;
    }
  }
  for (long unsigned int i=0;i<m_min_pt.size();i++){
    ATH_MSG_DEBUG( "Number of tracks found with cut= "<<m_min_pt[i] << " GeV =" << count[i] );
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
  trackCounts->setDetail("counts", count);


  TrackCountHypoTool::TrkCountsInfo trkinfo{d, tracks->at(0), prev};

  for(auto &tool:m_hypoTools)
  {
    ATH_CHECK(tool->decide(trkinfo));
  }

  SG::WriteHandle<xAOD::TrigCompositeContainer> TrackCountHandle(m_trackCountKey, context);
  ATH_CHECK(TrackCountHandle.record( std::move( tracks), std::move( tracksAux ) ) );
  //ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  d->setObjectLink<xAOD::TrigCompositeContainer>(featureString(), ElementLink<xAOD::TrigCompositeContainer>(m_trackCountKey.key(), 0) );

  return StatusCode::SUCCESS;
}

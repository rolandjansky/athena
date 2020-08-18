/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigMultiTrkComboHypo.cxx
 **
 **   Description: multi-track hypothesis algorithm
 **
 **   Author: Heather Russell
 **
 **************************************************************************/

#include <algorithm>
#include <numeric>

#include "TrigMultiTrkComboHypo.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "AthViews/View.h"
#include "AthViews/ViewHelper.h"

#include "Math/GenVector/VectorUtil.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;


TrigMultiTrkComboHypo::TrigMultiTrkComboHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::ComboHypo(name, pSvcLocator) {}


StatusCode TrigMultiTrkComboHypo::initialize() {
  ATH_MSG_DEBUG( "TrigMultiTrkHypo::initialize()" );

  ATH_CHECK( ::ComboHypo::initialize() );

  // check consistency of the properties
  if (m_trkMass.size() != m_nTrk) {
    ATH_MSG_ERROR( "Requested " << m_nTrk << " tracks per vertex, but only provided " << m_trkMass.size() << " track masses");
    return StatusCode::FAILURE;
  }
  if (m_trkPt.size() != m_nTrk) {
    ATH_MSG_ERROR( "Requested " << m_nTrk << " tracks per vertex, but only provided " << m_trkPt.size() << " track pT thresholds");
    return StatusCode::FAILURE;
  }

  for (const auto& range : m_massRange.value()) {
    if (range.first > range.second) {
      ATH_MSG_ERROR( "Mass range incorrect values: {Mi_min, Mi_max}" );
      return StatusCode::FAILURE;
    }
  }

  // dump numerical values
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG << "vertex topology:" << std::endl;
    for (size_t i = 0; i < m_nTrk; ++i) {
      msg() << MSG::DEBUG << "  " << i << "  trk: mass = " <<  m_trkMass[i] << ", Pt > " << m_trkPt[i] << std::endl;
    }
    msg() << MSG::DEBUG << "  mass range: {";
    for (const auto& range : m_massRange.value()) {
      msg() << MSG::DEBUG << " { " << range.first << ", " << range.second << " }";
    }
    msg() << MSG::DEBUG << " }" << std::endl;
  }

  if (m_trigLevelString == "L2") {
    m_trigLevel = xAOD::TrigBphys::L2;
    ATH_CHECK( m_muonContainerKey.initialize(false) );
    ATH_CHECK( m_trackParticleContainerKey.initialize() );
    renounce(m_trackParticleContainerKey);
    ATH_CHECK( m_trigBphysContainerKey.initialize(false) );
  }
  else if (m_trigLevelString == "EF") {
    m_trigLevel = xAOD::TrigBphys::EF;
    ATH_CHECK( m_trackParticleContainerKey.initialize(false) );
    ATH_CHECK( m_muonContainerKey.initialize() );
    renounce(m_muonContainerKey);
    ATH_CHECK( m_trigBphysContainerKey.initialize() );
  }
  else {
    m_trigLevel = xAOD::TrigBphys::UNKOWNLEVEL;
    ATH_MSG_ERROR( "trigLevel should be L2 or EF, but " << m_trigLevelString << " provided" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_vertexFitter.retrieve() );
  ATH_CHECK( m_vertexPointEstimator.retrieve() );

  // allowed IDs to filter out incoming decisions at L2 level
  for (const auto& item : triggerMultiplicityMap()) {
    const HLT::Identifier id = HLT::Identifier::fromToolName(item.first);
    m_allowedIDs.insert(id.numeric());
    if (item.second.size() > 1) {
      for (size_t i = 0; i < item.second.size(); i++) {
        m_allowedIDs.insert(TrigCompositeUtils::createLegName(id, i).numeric());
      }
    }
  }
  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Allowed decisions:" );
    for (const DecisionID& id : m_allowedIDs) {
      ATH_MSG_DEBUG( " +++ " << HLT::Identifier(id) );
    }
  }

  // add IDs to ComboHypoTools to check that each trigger leg fulfill TrigCompositeUtils::passed() requirement
  for (auto& tool : hypoTools()) {
    const HLT::Identifier id = tool->decisionId();
    const auto itr = triggerMultiplicityMap().find(id.name());
    if (itr == triggerMultiplicityMap().end()) {
      ATH_MSG_ERROR( "No entry found for " << tool->name() << " in triggerMultiplicityMap" );
    }
    const std::vector<int>& multiplicity = itr->second;
    std::vector<HLT::Identifier> legDecisionIds;
    if (multiplicity.size() == 1) {
      std::fill_n(std::back_inserter(legDecisionIds), multiplicity[0], id);
    }
    else {
      size_t n = static_cast<size_t>(std::accumulate(multiplicity.begin(), multiplicity.end(), 0));
      for (size_t i = 0; i < n; i++) {
        legDecisionIds.push_back(TrigCompositeUtils::createLegName(id, i));
      }
    }
    tool->setLegDecisionIds(legDecisionIds);
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Leg decisions for tool " << tool->name() );
      for (const auto& id : legDecisionIds) {
        ATH_MSG_DEBUG( " +++ " << id );
      }
    }
  }

  if (!m_monTool.empty()) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG( "GenericMonitoringTool name:" << m_monTool );
  }
  else {
    ATH_MSG_DEBUG( "No GenericMonitoringTool configured: no monitoring histograms will be available" );
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::finalize() {
  TrigConf::HLTUtils::hashes2file();
  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::execute(const EventContext& context) const {

  ATH_MSG_DEBUG( "TrigMultiTrkHypo::execute() starts" );

  if (m_trigLevel == xAOD::TrigBphys::L2) {
    ATH_CHECK( executeL2(context) );
  }
  else if (m_trigLevel == xAOD::TrigBphys::EF) {
    ATH_CHECK( executeEF(context) );
  }

  ATH_MSG_DEBUG( "TrigMultiTrkHypo::execute() terminates with StatusCode::SUCCESS" );
  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::executeL2(const EventContext& context) const {

  ATH_MSG_DEBUG( "decision input key: " << decisionsInput().at(0).key() );
  auto previousDecisionsHandle = SG::makeHandle(decisionsInput().at(0), context);
  CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() << " previous decisions" );

  // create the mutable output DecisionContainer and register it to StoreGate
  SG::WriteHandle<DecisionContainer> outputHandle = TrigCompositeUtils::createAndStore(decisionsOutput().at(0), context);
  DecisionContainer* decisions = outputHandle.ptr();

  // monitored variables
  auto mon_nTrk = Monitored::Scalar<int>("nTrk", 0);
  auto mon_nAcceptedTrk = Monitored::Scalar<int>("nAcceptedTrk", 0);
  auto mon_isEventAccepted = Monitored::Scalar<int>("acceptance", 0);
  auto mon_timer = Monitored::Timer( "TIME_all" );

  auto group = Monitored::Group(m_monTool,
    mon_nTrk, mon_nAcceptedTrk, mon_isEventAccepted,
    mon_timer);

  // combine all tracks from the event views, make overlap removal
  std::vector<ElementLink<xAOD::TrackParticleContainer>> tracks;
  size_t viewCounter = 0;
  for (const Decision* previousDecision : *previousDecisionsHandle) {
    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(previousDecision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    auto viewEL = viewLinkInfo.link;

    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracksFromView;
    auto tracksHandle = ViewHelper::makeHandle(*viewEL, m_trackParticleContainerKey, context);
    CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG( "tracks handle " << m_trackParticleContainerKey << " size: " << tracksHandle->size() );

    for (size_t idx = 0; idx < tracksHandle->size(); ++idx) {
      tracksFromView.emplace_back(ViewHelper::makeLink<xAOD::TrackParticleContainer>(*viewEL, tracksHandle, idx));
    }
    mon_nTrk += tracksFromView.size();

    for (const auto& trackEL : tracksFromView) {
      const xAOD::TrackParticle* track = *trackEL;
      if (track->definingParametersCovMatrixVec().empty() || track->pt() < m_trkPt.value().back()) continue;

      if (viewCounter == 0 ||
          std::find_if(tracks.begin(), tracks.end(),
                       [this,track = track](const auto& x){ return isIdenticalTracks(track, *x); }) == tracks.end()) {
        tracks.emplace_back(trackEL);
      }
    }

    viewCounter++;
  }
  std::sort(tracks.begin(), tracks.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs)->pt() > (*rhs)->pt()); });

  mon_nAcceptedTrk = tracks.size();
  ATH_MSG_DEBUG( "Select " << mon_nAcceptedTrk << " tracks and send them to vertex fitter" );

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Dump found tracks before vertex fit: " );
    for (const auto& x : tracks) {
      const xAOD::TrackParticle* track = *x;
      ATH_MSG_DEBUG( "  -- track pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
    }
  }

  mon_isEventAccepted = 0;
  for (size_t itrk1 = 0; itrk1 < tracks.size(); ++itrk1) {
    if (mon_isEventAccepted) break;
    const xAOD::TrackParticle* trk1 = *tracks[itrk1];
    auto p1 = trk1->genvecP4();
    p1.SetM( m_trkMass[0] );
    if (p1.Pt() < m_trkPt[0]) continue;

    for (size_t itrk2 = itrk1 + 1; itrk2 < tracks.size(); ++itrk2) {
      const xAOD::TrackParticle* trk2 = *tracks[itrk2];
      auto p2 = trk2->genvecP4();
      p2.SetM( m_trkMass[1] );
      if (p2.Pt() < m_trkPt[1]) continue;

      double mass = (p1 + p2).M();
      ATH_MSG_DEBUG( "track 1: " << p1.Pt()<< " / " << p1.Eta() << " / " << p1.Phi() << " / " << trk1->charge() );
      ATH_MSG_DEBUG( "track 2: " << p2.Pt()<< " / " << p2.Eta() << " / " << p2.Phi() << " / " << trk2->charge() );
      ATH_MSG_DEBUG( "track pair mass: " << mass );

      if (!isInMassRange(mass)) continue;

      xAOD::TrigBphys* trigBphys = fit(std::vector<ElementLink<xAOD::TrackParticleContainer>>{tracks[itrk1], tracks[itrk2]});
      if (!trigBphys) continue;

      ATH_MSG_DEBUG( "Found good dimuon pair at L2 level: stop looking for dimuon pairs" );
      mon_isEventAccepted = 1;
      delete trigBphys;
      break;
    }
  }

  if (mon_isEventAccepted) {
    ATH_MSG_DEBUG( "Copying decisions from " << decisionsInput().at(0).key() << " to " << decisionsOutput().at(0).key() );
    for (const Decision* previousDecision : *previousDecisionsHandle) {
      DecisionIDContainer previousDecisionIDs;
      TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
      DecisionIDContainer decisionIDs;
      std::set_intersection(previousDecisionIDs.begin(), previousDecisionIDs.end(), m_allowedIDs.begin(), m_allowedIDs.end(),
                            std::inserter(decisionIDs, decisionIDs.end()));

      Decision* decision = TrigCompositeUtils::newDecisionIn(decisions);
      TrigCompositeUtils::linkToPrevious(decision, previousDecision, context);
      TrigCompositeUtils::insertDecisionIDs(decisionIDs, decision);
    }
  }
  else {
    ATH_MSG_DEBUG( "No dimuon pairs found: no desicions will be copied to " << decisionsOutput().at(0).key() );
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::executeEF(const EventContext& context) const {

  ATH_MSG_DEBUG( "decision input key: " << decisionsInput().at(0).key() );
  auto previousDecisionsHandle = SG::makeHandle(decisionsInput().at(0), context);
  CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() << " previous decisions" );

  // create the mutable output DecisionContainer and register it to StoreGate
  SG::WriteHandle<DecisionContainer> outputHandle = TrigCompositeUtils::createAndStore(decisionsOutput().at(0), context);
  DecisionContainer* decisions = outputHandle.ptr();

  auto trigBphysHandle = SG::makeHandle(m_trigBphysContainerKey, context);
  ATH_CHECK( trigBphysHandle.record(std::make_unique<xAOD::TrigBphysContainer>(),
                                    std::make_unique<xAOD::TrigBphysAuxContainer>()) );

  // monitored variables
  auto mon_nTrk = Monitored::Scalar<int>("nTrk", 0);
  auto mon_nAcceptedTrk = Monitored::Scalar<int>("nAcceptedTrk", 0);
  auto mon_nCombination = Monitored::Scalar<int>("nCombination", 0);
  auto mon_nCombinationBeforeFit = Monitored::Scalar<int>("nCombinationBeforeFit", 0);
  auto mon_nBPhysObject = Monitored::Scalar<int>("nBPhysObject", 0);

  std::vector<float> trkMassBeforeFit;
  std::vector<float> bphysMass;
  std::vector<int> bphysCharge;
  auto mon_trkMassBeforeFit = Monitored::Collection("trkMassBeforeFit", trkMassBeforeFit);
  auto mon_bphysChi2 = Monitored::Collection("bphysChi2", *trigBphysHandle, &xAOD::TrigBphys::fitchi2);
  auto mon_bphysFitMass = Monitored::Collection("bphysFitMass", *trigBphysHandle, [](const xAOD::TrigBphys* x){ return x->fitmass()*0.001; } );
  auto mon_bphysMass = Monitored::Collection("bphysMass", bphysMass);
  auto mon_bphysCharge = Monitored::Collection("bphysCharge", bphysCharge);

  auto mon_timer = Monitored::Timer( "TIME_all" );

  auto group = Monitored::Group(m_monTool,
    mon_nTrk, mon_nAcceptedTrk, mon_nCombination, mon_nCombinationBeforeFit, mon_nBPhysObject,
    mon_trkMassBeforeFit, mon_bphysChi2, mon_bphysFitMass, mon_bphysMass, mon_bphysCharge,
    mon_timer);

  // combine all muons from the event views, make overlap removal
  std::vector<std::pair<ElementLink<xAOD::TrackParticleContainer>, ElementLinkVector<TrigCompositeUtils::DecisionContainer>>> tracks;
  size_t viewCounter = 0;
  for (const Decision* previousDecision : *previousDecisionsHandle) {
    auto previousDecisionEL = TrigCompositeUtils::decisionToElementLink(previousDecision, context);

    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(previousDecision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    auto viewEL = viewLinkInfo.link;

    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracksFromView;
    auto muonsHandle = ViewHelper::makeHandle(*viewEL, m_muonContainerKey, context);
    CHECK( muonsHandle.isValid() );
    ATH_MSG_DEBUG( "muons handle " << m_muonContainerKey << " size: " << muonsHandle->size() );

    for (const xAOD::Muon* muon : *muonsHandle) {
      if (!muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
      const ElementLink<xAOD::TrackParticleContainer>& trackEL = muon->inDetTrackParticleLink();
      CHECK( trackEL.isValid() );
      tracksFromView.emplace_back(trackEL);
    }
    mon_nTrk += tracksFromView.size();

    for (const auto& trackEL : tracksFromView) {
      const xAOD::TrackParticle* track = *trackEL;
      if (track->definingParametersCovMatrixVec().empty() || track->pt() < m_trkPt.value().back()) continue;

      auto itr = (viewCounter ? std::find_if(tracks.begin(), tracks.end(),
                                             [this,track = track](const auto& x){ return isIdenticalTracks(track, *x.first); })
                              : tracks.end());
      if (itr == tracks.end()) {
        tracks.emplace_back(std::make_pair(trackEL, ElementLinkVector<TrigCompositeUtils::DecisionContainer>(1, previousDecisionEL)));
      }
      else {
        (*itr).second.push_back(previousDecisionEL);
      }
    }
    viewCounter++;
  }
  std::sort(tracks.begin(), tracks.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs.first)->pt() > (*rhs.first)->pt()); });

  mon_nAcceptedTrk = tracks.size();
  ATH_MSG_DEBUG( "Select " << mon_nAcceptedTrk << " tracks and send them to vertex fitter" );

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Dump found tracks/muons before vertex fit: " );
    for (const auto& x : tracks) {
      const xAOD::TrackParticle* track = *x.first;
      ATH_MSG_DEBUG( "  -- track pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
    }
  }

  std::vector<std::vector<size_t>> trigBphysTrackIdx;
  for (size_t itrk1 = 0; itrk1 < tracks.size(); ++itrk1) {
    const xAOD::TrackParticle* trk1 = *tracks[itrk1].first;
    auto p1 = trk1->genvecP4();
    p1.SetM( m_trkMass[0] );
    int charge1 = static_cast<int>(trk1->charge());
    if (p1.Pt() < m_trkPt[0]) continue;

    for (size_t itrk2 = itrk1 + 1; itrk2 < tracks.size(); ++itrk2) {
      const xAOD::TrackParticle* trk2 = *tracks[itrk2].first;
      auto p2 = trk2->genvecP4();
      p2.SetM( m_trkMass[1] );
      int charge2 = static_cast<int>(trk2->charge());
      if (p2.Pt() < m_trkPt[1]) continue;

      double mass = (p1 + p2).M();
      mon_nCombination++;
      trkMassBeforeFit.push_back(mass * 0.001);

      ATH_MSG_DEBUG( "track 1: " << p1.Pt()<< " / " << p1.Eta() << " / " << p1.Phi() << " / " << trk1->charge() );
      ATH_MSG_DEBUG( "track 2: " << p2.Pt()<< " / " << p2.Eta() << " / " << p2.Phi() << " / " << trk2->charge() );
      ATH_MSG_DEBUG( "track pair mass: " << mass );

      if (!isInMassRange(mass)) continue;

      mon_nCombinationBeforeFit++;

      xAOD::TrigBphys* trigBphys = fit(std::vector<ElementLink<xAOD::TrackParticleContainer>>{tracks[itrk1].first, tracks[itrk2].first});
      if (!trigBphys) continue;

      trigBphysHandle->push_back(trigBphys);
      trigBphysTrackIdx.emplace_back(std::vector<size_t>{itrk1, itrk2});

      mon_nBPhysObject++;
      bphysMass.push_back(mass * 0.001);
      bphysCharge.push_back(charge1 + charge2);
    }
  }

  std::map<ElementLinkVector<TrigCompositeUtils::DecisionContainer>, DecisionIDContainer> legDecisionIDsMap;
  for (const auto& leg : tracks) {
    auto itr = legDecisionIDsMap.find( leg.second );
    if (itr == legDecisionIDsMap.end()) {
      DecisionIDContainer legDecisionIDs;
      for (const ElementLink<DecisionContainer>& decisionEL : leg.second) {
        TrigCompositeUtils::decisionIDs(*decisionEL, legDecisionIDs);
      }
      legDecisionIDsMap[leg.second] = legDecisionIDs;
    }
  }

  size_t idx = 0;
  for (const xAOD::TrigBphys* trigBphys : *trigBphysHandle) {
    ATH_MSG_DEBUG( "Found xAOD::TrigBphys: mass = " << trigBphys->mass() );

    // create a new output Decision object, backed by the 'decisions' container.
    Decision* decision = TrigCompositeUtils::newDecisionIn(decisions);

    std::vector<DecisionIDContainer*> previousDecisionIDs;
    for (size_t itrk : trigBphysTrackIdx[idx]) {
      // attach all previous decisions: if the same previous decision is called twice, that's fine - internally takes care of that
      for (const ElementLink<DecisionContainer>& previousDecisionEL : tracks[itrk].second) {
        TrigCompositeUtils::linkToPrevious(decision, *previousDecisionEL, context);
      }
      auto& legDecisionIDs = legDecisionIDsMap[tracks[itrk].second];
      previousDecisionIDs.push_back(&legDecisionIDs);
    }

    // set mandatory feature ElementLink to xAOD::TrigBphys object
    decision->setObjectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString(),
                                                      ElementLink<xAOD::TrigBphysContainer>(*trigBphysHandle, trigBphys->index()));

    for (const auto& tool : hypoTools()) {
      ATH_MSG_DEBUG( "Go to " << tool );
      ATH_CHECK( tool->decideOnSingleObject(decision, previousDecisionIDs) );
    }
    ++idx;
  }

  return StatusCode::SUCCESS;
}


xAOD::TrigBphys* TrigMultiTrkComboHypo::fit(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks) const {
  xAOD::TrigBphys* result = nullptr;

  ATH_MSG_DEBUG( "Perform vertex fit" );
  std::vector<const xAOD::TrackParticle*> tracklist(trackParticleLinks.size(), nullptr);
  std::transform(trackParticleLinks.begin(), trackParticleLinks.end(), tracklist.begin(),
                 [](const ElementLink<xAOD::TrackParticleContainer>& link){ return *link; });

  const Trk::Perigee& perigee1 = tracklist[0]->perigeeParameters();
  const Trk::Perigee& perigee2 = tracklist[1]->perigeeParameters();
  int flag = 0;
  int errorcode = 0;
  Amg::Vector3D startingPoint = m_vertexPointEstimator->getCirclesIntersectionPoint(&perigee1, &perigee2, flag, errorcode);
  if (errorcode != 0) startingPoint = Amg::Vector3D::Zero(3);
  ATH_MSG_DEBUG( "Starting point: (" << startingPoint(0) << ", " << startingPoint(1) << ", " << startingPoint(2) << ")" );

  auto fitterState = m_vertexFitter->makeState();
  m_vertexFitter->setMassInputParticles(m_trkMass, *fitterState);
  xAOD::Vertex* vertex = m_vertexFitter->fit(tracklist, startingPoint, *fitterState);
  if (!vertex) {
    ATH_MSG_DEBUG( "Vertex fit fails" );
    return result;
  }
  if (vertex->chiSquared() > 150.) {
    ATH_MSG_DEBUG( "Fit is successful, but vertex chi2 is too high, we are not going to save it (chi2 = " << vertex->chiSquared() << ")" );
    delete vertex;
    return result;
  }
  ATH_MSG_DEBUG( "Fit is successful" );

  double invariantMass = 0.;
  double invariantMassError = 0.;
  if (!m_vertexFitter->VKalGetMassError(invariantMass, invariantMassError, *fitterState).isSuccess()) {
    ATH_MSG_DEBUG( "Warning from TrkVKalVrtFitter: can not calculate uncertainties" );
    invariantMass = -9999.;
  }

  xAOD::TrackParticle::GenVecFourMom_t momentum;
  for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
    auto p = vertex->trackParticle(i)->genvecP4();
    p.SetM(m_trkMass[i]);
    momentum += p;
  }

  result = new xAOD::TrigBphys();
  result->makePrivateStore();
  result->initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), xAOD::TrigBphys::MULTIMU, momentum.M(), m_trigLevel);

  result->setFitmass(invariantMass);
  result->setFitchi2(vertex->chiSquared());
  result->setFitndof(vertex->numberDoF());
  result->setFitx(vertex->x());
  result->setFity(vertex->y());
  result->setFitz(vertex->z());
  result->setTrackParticleLinks(trackParticleLinks);

  delete vertex;

  ATH_MSG_DEBUG(
    "TrigBphys objects:\n\t  " <<
    "roiId:         " << result->roiId()  << "\n\t  " <<
    "particleType:  " << result->particleType() << "\n\t  " <<
    "level:         " << result->level() << "\n\t  " <<
    "eta:           " << result->eta() << "\n\t  " <<
    "phi:           " << result->phi() << "\n\t  " <<
    "mass:          " << result->mass() << "\n\t  " <<
    "fitmass:       " << result->fitmass() << "\n\t  " <<
    "chi2/NDF:      " << result->fitchi2() << " / " << result->fitndof() << "\n\t  " <<
    "vertex:        (" << result->fitx() << ", " << result->fity() << ", " << result->fitz() << ")" );

  return result;
}


bool TrigMultiTrkComboHypo::isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const {

  if (lhs->charge() * rhs->charge() < 0.) return false;
  return (ROOT::Math::VectorUtil::DeltaR(lhs->genvecP4(), rhs->genvecP4()) < m_deltaR);
}


bool TrigMultiTrkComboHypo::isInMassRange(double mass) const {

  bool result = false;
  for (const auto& range : m_massRange) {
    if (mass > range.first && mass < range.second) {
      result = true;
      break;
    }
  }
  return result;
}

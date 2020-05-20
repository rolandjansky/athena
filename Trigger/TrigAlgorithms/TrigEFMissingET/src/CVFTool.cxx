/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/**
 * TODO
 *
 * After a discussion with Nick Styles I learned some interesting things about
 * tracking software.
 *
 * Basically the vertex and track particle links between tracks and vertices
 * respectively are rather misleadingly named. They refer only to tracks that
 * were used in the fitting procedure to create vertices, *not* which vertex is
 * the best match for which track!
 * This means that we shouldn't be using the links in a physics way at all...
 * For that we need the track -> vertex association tool
 */

#include "CVFTool.h"

namespace {
  constexpr float TRACK_EXTRAP_OUT_OF_ACCEPTANCE = -999;
}

CVFTool::CVFTool(const std::string& name) :
  asg::AsgTool(name)
{
  declareProperty("ClusterMatchDR", m_clusterMatchDR=0.1,
      "Max DR to match clusters to extrapolated tracks");
  declareProperty("TrackClusterLinkName", m_trackClusLinkName="",
      "If set, name to use to link tracks to clusters");
  declareProperty("TrackExtrapolationQuarticTerm",
      m_trackExtrapolationQuartic=-14.6027571,
      "The quartic term in the track extrapolation");
  declareProperty("TrackExtrapolationQuadraticTerm",
      m_trackExtrapolationQuadratic=-44.7818374,
      "The quadratic term in the track extrapolation");
  declareProperty("TrackExtrapolationLinearTerm",
      m_trackExtrapolationLinear=540.656643,
      "The linear term in the track extrapolation");
  declareProperty("LegacyMode", m_doLegacy=true,
      "Use legacy code to make things the same as before");
  declareProperty("LinkOnlyPV", m_linkOnlyPV=true,
      "Only link primary vertex tracks to the clusters");
  // Extrapolation taken from ATL-COM-PHYS-2016-430
}

CVFTool::~CVFTool() {}

StatusCode CVFTool::initialize()
{
  if (!m_trackClusLinkName.empty() )
    m_trackClusLinks = std::make_unique<decTrackLinks_t>(m_trackClusLinkName);
  return StatusCode::SUCCESS;
}

StatusCode CVFTool::calculateCVF(
    xAOD::CaloClusterContainer* clusters,
    const xAOD::TrackParticleContainer* tracks,
    const xAOD::Vertex* priVtx)
{
  if (!priVtx) {
    ATH_MSG_DEBUG("No primary vertex received");
    // Previously clusters got 0 if there were no PV. It makes more sense in
    // this case to ignore tracking info and return -1. If we want to reject
    // events without a PV then we should do that explicitly...
    float defaultValue = (m_doLegacy ? 0. : -1.);
    for (xAOD::CaloCluster* iclus : *clusters) {
      iclus->insertMoment(xAOD::CaloCluster::VERTEX_FRACTION, defaultValue);
      if (m_trackClusLinks)
        (*m_trackClusLinks)(*iclus);
    }
    // We need to make sure the decoration is there as it's anticipated
    // downstream
    return StatusCode::SUCCESS;
  }
  // Start by extrapolating all of the tracks to the calorimeter
  // TODO - I suspect this would be faster to use one of the newer versions with
  // the correct coordinate system...
  std::vector<std::pair<const xAOD::TrackParticle*, TLorentzVector>> extrapolated;
  extrapolated.reserve(tracks->size() );
  // To speed things up later, check the maximum positive and negative range of
  // the track etas. This means we can avoid too many loops and dR comparisons
  // later on.
  double minTrkEta = 0;
  double maxTrkEta = 0;

  bool trackContainerIsOwning = tracks->ownPolicy() == SG::OWN_ELEMENTS;

  std::vector<const xAOD::TrackParticle*> PVTracks;
  if (m_doLegacy) {
    const auto& links = priVtx->trackParticleLinks();
    PVTracks.reserve(links.size() );
    for (const auto& link : links)
      PVTracks.push_back(*link);
  }
  else {
    ATH_MSG_ERROR("NOT IMPLEMENTED");
    return StatusCode::FAILURE;
  }

  for (const xAOD::TrackParticle* track : *tracks) {
    float dPhi = estimateExtrapolation(track);
    if (dPhi == TRACK_EXTRAP_OUT_OF_ACCEPTANCE)
      continue;
    TLorentzVector newP4 = track->p4();
    newP4.SetPhi(track->phi() - dPhi);
    extrapolated.push_back(std::make_pair(track, std::move(newP4) ) );
    minTrkEta = std::min(track->eta(), minTrkEta);
    maxTrkEta = std::max(track->eta(), maxTrkEta);
  }
  for (xAOD::CaloCluster* iclus : *clusters) {
    // The weight tools before us will set the cluster energies to 0 if they
    // fail one of the cuts so we should check that. Note that they will set it
    // *directly* to 0 so FPE is safe.
    if (iclus->e() == 0
        // Skip any cluster that could never match to a track
        || iclus->eta() < (minTrkEta-m_clusterMatchDR)
        || iclus->eta() > (maxTrkEta+m_clusterMatchDR) ) {
      iclus->insertMoment(xAOD::CaloCluster::VERTEX_FRACTION, -1);
      if (m_trackClusLinks)
        (*m_trackClusLinks)(*iclus);
      continue;
    }
    // track pt sum associated to pileup
    float pileupSum = 0;
    // track pt sum associated to PV
    float pvSum = 0;
    // Also keep a vector of all associated tracks
    std::vector<const xAOD::TrackParticle*> associatedTracks;
    for (const auto& trackPair : extrapolated) {
      if (trackPair.second.DeltaR(iclus->p4() ) < m_clusterMatchDR) {
        bool isFromPV = false;
        // Previously this was done with a DR requirement on the track...
        for (const xAOD::TrackParticle* trk : PVTracks) {
          if (trk->p4().DeltaR(trackPair.first->p4() ) < 0.01) {
            isFromPV = true;
            break;
          }
        }
        if (isFromPV)
          pvSum += trackPair.first->pt();
        else
          pileupSum += trackPair.first->pt();
        if (isFromPV || !m_linkOnlyPV)
          associatedTracks.push_back(trackPair.first);
      }
    }
    // Calculate CVF and NCVF
    float CVF = ((pvSum + pileupSum) == 0 ? -1 : pvSum / (pvSum+pileupSum) );
    iclus->insertMoment(xAOD::CaloCluster::VERTEX_FRACTION, CVF);
    // If we've asked for links, make them
    if (m_trackClusLinks) {
      vecTrackLink_t& links = (*m_trackClusLinks)(*iclus);
      links.reserve(associatedTracks.size() );
      for (const xAOD::TrackParticle* track : associatedTracks) {
        if (trackContainerIsOwning)
          links.emplace_back(*tracks, track->index() );
        else {
          links.emplace_back();
          links.back().setElement(track);
        }
      }
    }
  } //> end loop over clusters
  return StatusCode::SUCCESS;
}

float CVFTool::estimateExtrapolation(const xAOD::TrackParticle* track)
{
  float sinDPhi = (
      m_trackExtrapolationLinear +
      m_trackExtrapolationQuadratic*pow(track->eta(), 2) +
      m_trackExtrapolationQuartic*pow(track->eta(), 4) ) / (track->pt() *track->charge() );
  if (fabs(sinDPhi) > 1.0)
    return TRACK_EXTRAP_OUT_OF_ACCEPTANCE;
  else
    return asin(sinDPhi);
}

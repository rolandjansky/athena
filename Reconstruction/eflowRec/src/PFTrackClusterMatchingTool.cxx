/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchingTool.cxx
 *
 *  Created on: 15.04.2014
 *      Author: tlodd
 */

#include "eflowRec/PFTrackClusterMatchingTool.h"

#include "eflowRec/PFMatcher.h"
#include "eflowRec/PFMatchDistance.h"
#include "eflowRec/PFMatchPositions.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"

using namespace PFMatch;

PFTrackClusterMatchingTool::PFTrackClusterMatchingTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_trackPositionType("EM2EtaPhi"),
    m_clusterPositionType("GeomCenterEtaPhi"),
    m_distanceType("EtaPhiSquareSignificance"),
    m_matchCut(1.64*1.64),
    m_matcher(nullptr) {
  declareInterface<PFTrackClusterMatchingTool>(this);
  declareProperty("TrackPositionType",m_trackPositionType);
  declareProperty("ClusterPositionType",m_clusterPositionType);
  declareProperty("DistanceType",m_distanceType);
  declareProperty("MatchCut", m_matchCut);
}

StatusCode PFTrackClusterMatchingTool::initialize() {

  m_matcher = std::make_unique<PFMatch::TrackClusterMatcher>(DistanceFactory::Get(m_distanceType, TrackPositionFactory::Get(m_trackPositionType), ClusterPositionFactory::Get(m_clusterPositionType)),m_matchCut);

  ATH_MSG_VERBOSE("In initialize:");
  ATH_MSG_VERBOSE("Track position type is \"" << m_trackPositionType << "\"");
  ATH_MSG_VERBOSE("Cluster position type is \"" << m_clusterPositionType << "\"");
  ATH_MSG_VERBOSE("Distance type is \"" << m_distanceType << "\"");
  ATH_MSG_VERBOSE("Match cut is " << m_matchCut);

  return StatusCode::SUCCESS;
}

StatusCode PFTrackClusterMatchingTool::finalize() {

  return StatusCode::SUCCESS;
}

std::vector<std::pair<eflowRecCluster*, float>>
PFTrackClusterMatchingTool::doMatches(const eflowRecTrack* track,
                                      eflowRecClusterContainer* clusters,
                                      int nMatches) const
{
  std::vector<eflowRecCluster*> vec_clusters(clusters->begin(),
                                             clusters->end());
  return doMatches(track, vec_clusters, nMatches);
}

std::vector<std::pair<eflowRecCluster*, float>>
PFTrackClusterMatchingTool::doMatches(
  const eflowRecTrack* track,
  std::vector<eflowRecCluster*>& clusters,
  int nMatches) const
{
  auto matched_tracks_pt = Monitored::Scalar<float>("matched_tracks_pt");
  auto matched_clusters_e = Monitored::Scalar<float>("matched_clusters_e");
  auto matched_clusters_eta = Monitored::Scalar<float>("matched_clusters_eta");
  auto matched_clusters_phi = Monitored::Scalar<float>("matched_clusters_phi");
  auto group = Monitored::Group(m_monTool, matched_tracks_pt, matched_clusters_e, 
                                matched_clusters_eta, matched_clusters_phi);

  /* Transform the vector of eflowRecCluster into a vector of eflowMatchClusters
   */
  std::vector<eflowMatchCluster*> matchClusters;
  matchClusters.reserve(clusters.size());

  for (auto& cluster : clusters) {
    matchClusters.push_back(cluster->getMatchCluster());
  }

  /* Use the TrackClusterMatcher to retrieve the matches */
  eflowRecMatchTrack matchTrack(track);
  std::vector<MatchDistance> allMatches = m_matcher->bestMatches(
    &matchTrack, matchClusters, nMatches, 0.1 * track->getTrack()->e());

  /* Transform the vector of MatchDistance objects into a vector of
   * eflowRecClusters and return it */
  std::vector<std::pair<eflowRecCluster*, float>> results;
  for (MatchDistance& match : allMatches) {
    // The matching cannot change the cluster type, this started as
    // eflowMatchCluster and remains of that type -- no need to test cast from
    // ICluster*
    eflowMatchCluster* thisMatch = static_cast<eflowMatchCluster*>(match.first);
    results.emplace_back(thisMatch->getEfRecCluster(), match.second);
    matched_clusters_e = thisMatch->e() * m_invGeV;
    matched_clusters_eta = thisMatch->eta();
    matched_clusters_phi = thisMatch->phi();
  }
  matched_tracks_pt = track->getTrack()->pt() / 1000; //Conversion to GeV
  return results;
}

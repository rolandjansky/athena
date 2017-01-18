/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    m_matcher(0),
    m_tracksProcessed(0),
    m_tracksMatched(0) {
  declareInterface<PFTrackClusterMatchingTool>(this);
  declareProperty("TrackPositionType",m_trackPositionType);
  declareProperty("ClusterPositionType",m_clusterPositionType);
  declareProperty("DistanceType",m_distanceType);
  declareProperty("MatchCut", m_matchCut);
}

StatusCode PFTrackClusterMatchingTool::initialize() {
  IDistanceProvider* distanceProvider = DistanceFactory::Get(m_distanceType, TrackPositionFactory::Get(m_trackPositionType), ClusterPositionFactory::Get(m_clusterPositionType));
  m_matcher = new PFMatch::TrackClusterMatcher(distanceProvider, m_matchCut);

  msg(MSG::INFO) << "In initialize:" << endmsg;
  msg(MSG::INFO) << "Track position type is \"" << m_trackPositionType << "\"" << endmsg;
  msg(MSG::INFO) << "Cluster position type is \"" << m_clusterPositionType << "\"" << endmsg;
  msg(MSG::INFO) << "Distance type is \"" << m_distanceType << "\"" << endmsg;
  msg(MSG::INFO) << "Match cut is " << m_matchCut << endmsg;

  //delete distanceProvider;

  return StatusCode::SUCCESS;
}

StatusCode PFTrackClusterMatchingTool::finalize() {
  msg(MSG::INFO) << "Final summary:" << endmsg;
  msg(MSG::INFO) << "Processed " << m_tracksProcessed << " tracks." << endmsg;
  msg(MSG::INFO) << "Produced  " << m_tracksMatched<< " matches." << endmsg;

  delete m_matcher;

  return StatusCode::SUCCESS;
}

std::vector<eflowRecCluster*> PFTrackClusterMatchingTool::doMatches(const eflowRecTrack* track, const eflowRecClusterContainer* clusters, int nMatches) {
  std::vector<eflowRecCluster*> vec_clusters;
  for (unsigned int iCluster = 0; iCluster < clusters->size(); ++iCluster) {
    eflowRecCluster* thisEFRecCluster = const_cast<eflowRecCluster*>(clusters->at(iCluster));
    vec_clusters.push_back(thisEFRecCluster);
  }
  const std::vector<eflowRecCluster*> const_clusters = vec_clusters;
  return doMatches(track, const_clusters, nMatches);
}

std::vector<eflowRecCluster*> PFTrackClusterMatchingTool::doMatches(const eflowRecTrack* track, const std::vector<eflowRecCluster*> clusters, int nMatches) {
  m_tracksProcessed++;

  /* Transform the vector of eflowRecCluster into a vector of eflowMatchClusters */
  unsigned int nClusters = clusters.size();
  std::vector<const eflowMatchCluster*> matchClusters;
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
      matchClusters.push_back(clusters.at(iCluster)->getMatchCluster());
  }

  /* Use the TrackClusterMatcher to retrieve the matches */
  eflowRecMatchTrack matchTrack(track);
  std::vector<MatchDistance> allMatches = m_matcher->bestMatches(&matchTrack, matchClusters, nMatches, 0.1*track->getTrack()->e());

  /* Transform the vector of MatchDistance objects into a vector of eflowRecClusters and return it */
  std::vector<eflowRecCluster*> results;
  for (unsigned int imatch = 0; imatch < allMatches.size(); ++imatch) {
    const eflowMatchCluster* thisMatch = dynamic_cast<const eflowMatchCluster*>(allMatches.at(
        imatch).m_cluster);
    if (thisMatch) results.push_back(thisMatch->getEfRecCluster());
  }
  m_tracksMatched += results.size(); // Increment the matched tracks counter appropriately
  return results;
}

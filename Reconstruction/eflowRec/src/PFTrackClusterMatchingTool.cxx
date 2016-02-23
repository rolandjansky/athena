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

  msg(MSG::INFO) << "In initialize:" << endreq;
  msg(MSG::INFO) << "Track position type is \"" << m_trackPositionType << "\"" << endreq;
  msg(MSG::INFO) << "Cluster position type is \"" << m_clusterPositionType << "\"" << endreq;
  msg(MSG::INFO) << "Distance type is \"" << m_distanceType << "\"" << endreq;
  msg(MSG::INFO) << "Match cut is " << m_matchCut << endreq;

  //delete distanceProvider;

  return StatusCode::SUCCESS;
}

StatusCode PFTrackClusterMatchingTool::finalize() {
  msg(MSG::INFO) << "Final summary:" << endreq;
  msg(MSG::INFO) << "Processed " << m_tracksProcessed << " tracks." << endreq;
  msg(MSG::INFO) << "Produced  " << m_tracksMatched<< " matches." << endreq;

  delete m_matcher;

  return StatusCode::SUCCESS;
}

eflowRecCluster* PFTrackClusterMatchingTool::bestMatch(const eflowRecTrack* track, const std::vector<eflowRecCluster*>& clusters) {
  m_tracksProcessed++;

  /* Transform the vector of eflowRecCluster into a vector of eflowMatchClusters */
  unsigned int nClusters = clusters.size();
  const float trackE=track->getTrack()->e();
  std::vector<const eflowMatchCluster*> matchClusters;
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    if (clusters[iCluster]->getCluster()->e()>0.1*trackE) matchClusters.push_back(clusters[iCluster]->getMatchCluster());
  }

  /* Use the TrackClusterMatcher to retrieve the best match */
  eflowRecMatchTrack matchTrack(track);
  MatchDistance bestMatch = m_matcher->bestMatch(&matchTrack, matchClusters);

  /* If there's no match, return a null pointer */
  if (!bestMatch._isMatch) { return 0; }

  /* Increment the match counter and return the corresponding eflowRecCluster */
  m_tracksMatched++;
  const eflowMatchCluster* bestMatchCluster = dynamic_cast<const eflowMatchCluster*>(bestMatch._cluster);
  
  if (bestMatchCluster) return bestMatchCluster->getEfRecCluster();
  else return nullptr;
}

std::vector<eflowRecCluster*> PFTrackClusterMatchingTool::allMatches(const eflowRecTrack* track, const std::vector<eflowRecCluster*>& clusters) {
  m_tracksProcessed++;

  /* Transform the vector of eflowRecCluster into a vector of eflowMatchClusters */
  unsigned int nClusters = clusters.size();
  std::vector<const eflowMatchCluster*> matchClusters(nClusters);
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    matchClusters[iCluster] = clusters[iCluster]->getMatchCluster();
  }

  /* Use the TrackClusterMatcher to retrieve the all matches */
  eflowRecMatchTrack matchTrack(track);
  std::vector<MatchDistance> allMatches = m_matcher->allMatches(&matchTrack, matchClusters);

  /* Transform the vector of MatchDistance objects into a vector of eflowRecClusters and return it */
  std::vector<eflowRecCluster*> result;
  unsigned int nMatches = allMatches.size();
  for (unsigned int iMatch = 0; iMatch < nMatches; ++iMatch) {
    const eflowMatchCluster* thisMatch = dynamic_cast<const eflowMatchCluster*>(allMatches[iMatch]._cluster);
    if (thisMatch) result.push_back(thisMatch->getEfRecCluster());
  }
  m_tracksMatched+=result.size(); // Increment the matched tracks counter appropriately

  return result;
}

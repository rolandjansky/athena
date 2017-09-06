/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchingTool.h
 *
 *  Created on: 15.04.2014
 *      Author: tlodd
 */

#include "AthenaBaseComps/AthAlgTool.h"

#ifndef PFMATCHINGTOOL_H_
#define PFMATCHINGTOOL_H_

#include "eflowRec/PFMatcher.h"

class eflowRecCluster;
class eflowRecTrack;
class eflowRecClusterContainer;
class eflowMatchCluster;

static const InterfaceID IID_PFTrackClusterMatchingTool("PFTrackClusterMatchingTool", 1, 0);

class PFTrackClusterMatchingTool: public AthAlgTool {

public:
  PFTrackClusterMatchingTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~PFTrackClusterMatchingTool() { }

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode finalize();

  /** Get n clusters that matches best to a given track */
  std::vector<eflowRecCluster*> doMatches(const eflowRecTrack* track, const eflowRecClusterContainer* clusters, int n);
  std::vector<eflowRecCluster*> doMatches(const eflowRecTrack* track, const std::vector<eflowRecCluster*> clusters, int n);

private:

  /** The type of track position to be used for matching */
  std::string m_trackPositionType;
  /** The type of cluster position to be used for matching */
  std::string m_clusterPositionType;
  /** The type of distance measure to be used for matching */
  std::string m_distanceType;
  /** The cut value on the distance measure */
  double m_matchCut;

  /** The track cluster matcher to perform the actual matching */
  std::unique_ptr<PFMatch::TrackClusterMatcher> m_matcher;
  
  /** Count the number of tracks processed -- for the summary in finalize() */
  unsigned int m_tracksProcessed;
  /** Count the number of matches created -- for the summary in finalize() */
  unsigned int m_tracksMatched;
};

inline const InterfaceID& PFTrackClusterMatchingTool::interfaceID() { return IID_PFTrackClusterMatchingTool; }


#endif /* PFMATCHINGTOOL_H_ */

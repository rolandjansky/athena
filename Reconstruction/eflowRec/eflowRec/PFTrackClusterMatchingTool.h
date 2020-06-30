/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include <utility>
#include "eflowRec/PFMatcher.h"

class eflowRecCluster;
class eflowRecTrack;
class eflowRecClusterContainer;
class eflowMatchCluster;

static const InterfaceID IID_PFTrackClusterMatchingTool("PFTrackClusterMatchingTool", 1, 0);

/**
This is the tool, which inherits from AthAlgTool, which clients can use for track-cluster matching. Clients should define the type of cluster position, the type of track position and the type of distance to use. This tool uses the PF::TrackClusterMatcher tool and returns a vector of matched eflowRecCluster to the client. The client must provide an eflowRecTrack and a list of eflowRecCluster to consider for matching.
*/
class PFTrackClusterMatchingTool: public AthAlgTool {

public:
  PFTrackClusterMatchingTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~PFTrackClusterMatchingTool() { }

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode finalize();

  /** Get n clusters that matches best to a given track */
  std::vector<std::pair<eflowRecCluster*,float> > doMatches(const eflowRecTrack* track, eflowRecClusterContainer* clusters, int n) const;
  std::vector<std::pair<eflowRecCluster*,float> > doMatches(const eflowRecTrack* track, const std::vector<eflowRecCluster*> clusters, int n) const;

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
};

inline const InterfaceID& PFTrackClusterMatchingTool::interfaceID() { return IID_PFTrackClusterMatchingTool; }


#endif /* PFMATCHINGTOOL_H_ */

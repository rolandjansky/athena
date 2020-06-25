/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatcher.cxx
 *
 *  Created on: 03.04.2014
 *      Author: tlodd
 */

#include "eflowRec/PFMatcher.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
namespace PFMatch {

MatchDistance TrackClusterMatcher::match(const ITrack* track, const ICluster* cluster) const {
  double distance = m_distanceProvider->distanceBetween(track, cluster);
  return MatchDistance(cluster, distance);
}
void TrackClusterMatcher::setDRParameters(){
  std::string theFileName = PathResolver::find_file ("rmsvsdr.dat", "DATAPATH");
  std::ifstream DRPARAM(theFileName.c_str());
  for (int i=0;i<8;i++) DRPARAM>>m_drcut_par[i][0]>>m_drcut_par[i][1]>>m_drcut_par[i][2];

}

  
}

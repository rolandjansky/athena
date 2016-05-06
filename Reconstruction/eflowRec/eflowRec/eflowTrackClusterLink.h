/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowTrackClusterLink.h
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#ifndef EFLOWTRACKCLUSTERLINK_H_
#define EFLOWTRACKCLUSTERLINK_H_

#include <utility>
#include <map>
#include <vector>
#include <iostream>

#include <cassert>
#include "GaudiKernel/ToolHandle.h"

#include "CxxUtils/fpcompare.h"
#include "eflowRec/eflowRecTrack.h"

class eflowRecTrack;
class eflowRecCluster;

class eflowTrackClusterLink {
private:
  typedef std::map<std::pair<eflowRecTrack*, eflowRecCluster*>, eflowTrackClusterLink*> InstanceMap;

  eflowTrackClusterLink(eflowRecTrack* track, eflowRecCluster* cluster) :
      m_track(track), m_cluster(cluster)/*,  m_eExpect(NAN), m_varEExpect(NAN)*/ { }

  virtual ~eflowTrackClusterLink() { }

public:
  static eflowTrackClusterLink* getInstance(eflowRecTrack* track, eflowRecCluster* cluster){
    std::pair<eflowRecTrack*, eflowRecCluster*> thisPair(std::make_pair(track, cluster));
    eflowTrackClusterLink* dummy(0);
    /* If the pair was already in the map, insert().first is an iterator to the existing entry, otherwise to the newly inserted one
     * iterator->second is a reference to the corresponding eflowTrackClusterLink pointer */
    eflowTrackClusterLink*& result = m_instances.insert(std::make_pair(thisPair, dummy)).first->second;
    if (!result) {
      result = new eflowTrackClusterLink(track, cluster);
    }
    return result;
  }
  static void clearInstances();

  eflowRecCluster* getCluster() const { return m_cluster; }
  eflowRecTrack* getTrack() const { return m_track; }

  void setClusterIntegral(const std::vector<double>& clusterIntegral) {
    if (!m_clusterIntegral.empty()){
      std::cout << "WARNING\tResetting cluster integral for track-cluster link!" << std::endl;
    }
    m_clusterIntegral = clusterIntegral;
  }
  const std::vector<double>& getClusterIntegral() const { return m_clusterIntegral; }

private:
  eflowRecTrack* m_track;
  eflowRecCluster* m_cluster;

  std::vector<double> m_clusterIntegral;

  static InstanceMap m_instances;
};

#endif /* EFLOWTRACKCLUSTERLINK_H_ */

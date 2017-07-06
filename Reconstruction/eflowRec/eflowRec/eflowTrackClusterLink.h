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
  typedef std::map<std::pair<eflowRecTrack*, eflowRecCluster*>, std::unique_ptr<eflowTrackClusterLink> > InstanceMap;
  
public:
  eflowTrackClusterLink(eflowRecTrack* track, eflowRecCluster* cluster) :
      m_track(track), m_cluster(cluster) { }

  virtual ~eflowTrackClusterLink() { }

  static eflowTrackClusterLink* getInstance(eflowRecTrack* track, eflowRecCluster* cluster){
    std::pair<eflowRecTrack*, eflowRecCluster*> thisPair(std::make_pair(track, cluster));    

    /* The find returns a valid iterator. If there is no existing entry it returns the end iterator */
    InstanceMap::iterator mapIterator = m_instances.find(thisPair);
    
    if (m_instances.end() == mapIterator){
      /* If no existing entry we create a new unique_ptr and add an entry into the map */
      m_instances[thisPair] = std::make_unique<eflowTrackClusterLink>(track,cluster);
      return m_instances[thisPair].get();
    }
    else return (*mapIterator).second.get();
  }

  eflowRecCluster* getCluster() const { return m_cluster; }
  eflowRecTrack* getTrack() const { return m_track; }

  void setClusterIntegral(const std::vector<double>& clusterIntegral) {
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

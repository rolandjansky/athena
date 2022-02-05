/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include <unordered_map>
#include <vector>
#include <iostream>
#include <mutex>

#include "GaudiKernel/ToolHandle.h"

#include "CxxUtils/fpcompare.h"
#include "eflowRec/eflowRecTrack.h"

class eflowRecTrack;
class eflowRecCluster;

/**
 Stores pointers to an eflowRecTrack and an eflowRecCluster.
 These pointers kept in a cache internal to the getInstance function.
 We also store a vector of energy density, around the track impact point
 in each calorimeter layer, corresponding to the layers in the calorimeter.
*/
class eflowTrackClusterLink {
public:
  eflowTrackClusterLink(eflowRecTrack* track, eflowRecCluster* cluster) :
      m_track(track), m_cluster(cluster) { }

  virtual ~eflowTrackClusterLink() { }

  static eflowTrackClusterLink* getInstance(eflowRecTrack* track,
                                            eflowRecCluster* cluster,
                                            const EventContext& ctx = Gaudi::Hive::currentContext());

  eflowRecCluster* getCluster() { return m_cluster; }
  const eflowRecCluster* getCluster() const { return m_cluster; }
  eflowRecTrack* getTrack() { return m_track; }
  const eflowRecTrack* getTrack() const { return m_track; }

  void setClusterIntegral(const std::vector<double>& clusterIntegral) {
    m_clusterIntegral = clusterIntegral;
  }
  const std::vector<double>& getClusterIntegral() const { return m_clusterIntegral; }

private:
  using key_t = std::pair<eflowRecTrack*, eflowRecCluster*>;
  struct Hasher
  {
    size_t operator() (const key_t& k) const
    {
      return std::hash<eflowRecTrack*>()(k.first) ^
        std::hash<eflowRecCluster*>()(k.second);
    }
  };
  struct Cache
  {
    std::mutex m_mutex;
    std::unordered_map<key_t, std::unique_ptr<eflowTrackClusterLink>, Hasher > m_map;
    EventContext::ContextEvt_t m_evt = static_cast<EventContext::ContextEvt_t>(-1);
  };

  eflowRecTrack* m_track;
  eflowRecCluster* m_cluster;

  std::vector<double> m_clusterIntegral;
};

#endif /* EFLOWTRACKCLUSTERLINK_H_ */

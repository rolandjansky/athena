/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTCLUSTER_H
#define TRIGHTTOBJECTS_HTTCLUSTER_H

/*
 * HTTCluster.h: This file declares the class used to represent clusters.
 * Declarations in this file:
 *      class HTTCluster
 * Author: Alex Martyniuk
 * Email: martyniu@cern.ch
 */

#include <TObject.h>
#include "TrigHTTObjects/HTTHit.h"

 /*
  * Clusters resulting from HTT clustering algorithms as stored in m_clusterEquiv as a HTTHit
  * The raw hits used to form this cluster are stored in the m_hits, the first entry is the seed that initiated this cluster.
  */
typedef std::vector<HTTHit> hitVector;
class HTTCluster : public TObject
{
public:
  virtual ~HTTCluster() = default;

  // get private members
  hitVector const& getHitList() const { return m_hits; }
  HTTHit const& getClusterEquiv() const { return m_clusterEquiv; }

  // set private members
  void setHitList(const hitVector& input) { m_hits = input; }
  void setClusterEquiv(const HTTHit& input) { m_clusterEquiv = input; }

  // filling functions
  void push_backHitList(const HTTHit& input) { m_hits.push_back(input); }

  virtual void Print(Option_t* opts = "") const;

private:
  hitVector m_hits; // list of hits that make the cluster, the seed of the cluster will be the first entry in this list.
  HTTHit m_clusterEquiv; // This is the cluster

  ClassDef(HTTCluster, 3);
};

#endif // HTTCLUSTER_H

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCLUSTER_H
#define HTTCLUSTER_H

/*
 * HTTCluster.h: This file declares the class used to represent clusters.
 * Declarations in this file:
 *      class HTTCluster
 * Author: Alex Martyniuk
 * Email: martyniu@cern.ch
 */

#include <TObject.h>

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <memory>
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTUtils/HTTTypes.h"

/*
 * Clusters resulting from HTT clustering algorithms as stored in m_clusterEquiv as a HTTHit
 * The raw hits used to form this cluster are stored in the m_hitlist, the first entry is the seed that initiated this cluster.
 */
typedef std::vector<HTTHit> hitVector;
class HTTCluster : public TObject
{
    public:
        virtual ~HTTCluster() = default;

        // get private members
        hitVector const & getHitList() const { return m_hitlist; }
        HTTHit const & getClusterEquiv() const { return m_clusterEquiv; }

        // set private members
        void setHitList(const hitVector &input) { m_hitlist = input; }
        void setClusterEquiv(const HTTHit &input) { m_clusterEquiv = input; }

        // filling functions
        void push_backHitList(const HTTHit& input ) { m_hitlist.push_back(input); }

    private:
        hitVector m_hitlist; // list of hits that make the cluster, the seed of the cluster will be the first entry in this list.
        HTTHit m_clusterEquiv; // This is the cluster

        ClassDef(HTTCluster, 3);
};

#endif // HTTCLUSTER_H

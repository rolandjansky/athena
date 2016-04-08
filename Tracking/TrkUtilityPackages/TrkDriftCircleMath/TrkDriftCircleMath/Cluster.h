/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CLUSTER_H
#define DCMATH_CLUSTER_H

#include "TrkDriftCircleMath/LocPos.h"
#include "TrkDriftCircleMath/ClusterId.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <vector>

namespace TrkDriftCircleMath {

  /** class representing a cluster meaurement */
  class Cluster
  {
  public:
    /** minimal constructor: 
	@param pos local position of cluster 
	@param err error on cluster position */
    Cluster (const LocPos& pos, double err)
      : m_pos (pos), m_err (err),m_id{},m_index{} {}
            
    /** full constructor:
	@param pos local position of cluster 
	@param err error on cluster position
	@param id cluster identifier
	@param index index of cluster use to link it back to the original RIO_OnTrack */
    Cluster (const LocPos& pos, double err, ClusterId id, int index)
	: m_pos (pos), m_err (err), m_id (id), m_index (index) {}
            
    ~Cluster() {}

    /** local position of the cluster */
    const LocPos& position() const { return m_pos; }
    
    /** cluster identifier */
    const ClusterId&    id() const { return m_id; }
    
    /** direct access to local x coordinate of cluster */
    double x() const { return m_pos.x(); }
    
    /** direct access to y coordinate of cluster */
    double y() const { return m_pos.y(); }
    
    /** error on cluster position */
    double err() const { return m_err; }
    
    /** index of cluster in input vector, use to link back the cluster to the MuonClusterOnTrack */
    int index() const { return m_index; }
    
  private:
    LocPos     m_pos;
    double     m_err;
    ClusterId  m_id;
    int        m_index;
  };

  typedef std::vector<Cluster>     CLVec;
  typedef CLVec::iterator          CLIt;
  typedef CLVec::const_iterator    CLCit;
}

std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::Cluster& dc );

MsgStream   & operator << (MsgStream   & os, const TrkDriftCircleMath::Cluster& dc);

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// original filename: Group.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - The Groups of spacepoints after pattern recognition
// 
// date: 29/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#ifndef __HOUGH_BIN_CLUSTER_H__
#define __HOUGH_BIN_CLUSTER_H__

#include <list>
#include <iostream>
#include <iomanip>

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigSiSpacePointUtils.h"


class HoughBinCluster {

public:

  HoughBinCluster() : m_binX(0), m_binY(0), m_nEntries(0) {};  
  ~HoughBinCluster(){};
   
  void AddSpacePoints( std::list<const TrigSiSpacePointBase* >&, long, long);
  long NumberOfHits() const;
  std::list< const TrigSiSpacePointBase* >&  spacePoints() {
    return m_spList;
  }
  const std::list< const TrigSiSpacePointBase* >& spacePoints() const {
    return m_spList;
  }

  double averageBinX() const;
  double averageBinY() const;

  void removeDuplicates();

 private:
  std::list< const TrigSiSpacePointBase* > m_spList;   // list of ptrs to SpPs
  long m_binX;// weighted sum of X-coordinates of bins in the cluster
  long m_binY;// weighted sum of Y-coordinates of bins in the cluster
  long m_nEntries;
};

inline std::ostream& operator<<(std::ostream& s, const HoughBinCluster& g) { 
  const std::list<const TrigSiSpacePointBase*>& hits = g.spacePoints(); 
  s << "nhits=" << hits.size() 
    << "\taddr=" << std::ios::hex << (void*)&g << std::ios::dec << "\t]";
  std::list<const TrigSiSpacePointBase*>::const_iterator  hitr(hits.begin());   
  std::list<const TrigSiSpacePointBase*>::const_iterator  hend(hits.end());   
  while ( hitr!=hend ) s << "\n\t" << *(*hitr++); 
  return s;
}


inline double HoughBinCluster::averageBinX() const {
  return double(m_binX)/double(m_nEntries);
}


inline double HoughBinCluster::averageBinY() const {
  return double(m_binY)/double(m_nEntries);
}

inline void HoughBinCluster::AddSpacePoints( std::list< const TrigSiSpacePointBase* >& binSpList, long binX, long binY ) { 
  long entries = binSpList.size();
  m_nEntries += entries;
  m_binX += entries*binX;
  m_binY += entries*binY;
  m_spList.splice(m_spList.end(), binSpList); 
}

  
inline long HoughBinCluster::NumberOfHits() const { 
  return m_spList.size(); 
}

inline void HoughBinCluster::removeDuplicates() {
  m_spList.sort(orderSpacePointsInIndex());
  m_spList.unique(equalSpacePointsInIndex());
  m_spList.sort(orderSpacePointsInLayer());
}

#endif


/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTOPTIONALEVENTINFO_H
#define TRIGHTTOBJECTS_HTTOPTIONALEVENTINFO_H

#include <TObject.h>
#include <vector>
#include <iostream>
#include <sstream>

#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTTruthTrack.h"

class HTTOptionalEventInfo : public  TObject {

public:

  HTTOptionalEventInfo() {};
  virtual ~HTTOptionalEventInfo();

  void reset();

  // Offline Clusters
  const std::vector<HTTCluster>& getOfflineClusters() const { return m_OfflineClusters; }
  size_t nOfflineClusters() const { return m_OfflineClusters.size(); }
  void addOfflineCluster(HTTCluster c) { m_OfflineClusters.push_back(c); }

  // Offline Tracks
  const std::vector<HTTOfflineTrack>& getOfflineTracks() const { return m_OfflineTracks; }
  size_t nOfflineTracks() const { return m_OfflineTracks.size(); }
  void addOfflineTrack(HTTOfflineTrack t) { m_OfflineTracks.push_back(t); };

  // Truth Tracks
  const std::vector<HTTTruthTrack>& getTruthTracks() const { return m_TruthTracks; }
  size_t nTruthTracks() const { return m_TruthTracks.size(); }
  void addTruthTrack(HTTTruthTrack t) { m_TruthTracks.push_back(t); };


  //reserve sizes
  void reserveOfflineClusters(size_t size) { m_OfflineClusters.reserve(size); }
  void reserveOfflineTracks(size_t size) { m_OfflineTracks.reserve(size); }
  void reserveTruthTracks(size_t size) { m_TruthTracks.reserve(size); }


private:

  std::vector<HTTCluster>       m_OfflineClusters;
  std::vector<HTTOfflineTrack>  m_OfflineTracks;
  std::vector<HTTTruthTrack>    m_TruthTracks;


  ClassDef(HTTOptionalEventInfo, 2)
};

std::ostream& operator<<(std::ostream&, const HTTOptionalEventInfo&);
#endif
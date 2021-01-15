/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTOPTIONALEVENTINFO_H
#define HTTOPTIONALEVENTINFO_H

#include <TObject.h>
#include <vector>
#include <iostream>
#include <sstream>

#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"

class HTTOptionalEventInfo: public  TObject {

 public:
  
  HTTOptionalEventInfo() {};
  virtual ~HTTOptionalEventInfo();

  void reset();

  //offline clusters
  const std::vector<HTTCluster>& getOfflineClusters()  const { return m_OfflineClusters; }
  int  nOfflineClusters()  const { return m_OfflineClusters.size(); }  
  void addOfflineClusters(HTTCluster cluster) { m_OfflineClusters.push_back(cluster); };

   // tracks
  const std::vector<HTTTruthTrack>& getTruthTracks()  const { return m_Truth; }
  int  nTruthTracks()  const { return m_Truth.size(); }  
  void addTruthTrack(HTTTruthTrack d) { m_Truth.push_back(d); };

  //offline tracks
  const std::vector<HTTOfflineTrack>&  getOfflineTracks()  const { return m_Track; }
  int  nOfflineTracks()  const { return m_Track.size(); }
  void addOfflineTrack(HTTOfflineTrack d) { m_Track.push_back(d); };

 
 private:

  std::vector<HTTCluster>   m_OfflineClusters;
  std::vector<HTTTruthTrack>   m_Truth;
  std::vector<HTTOfflineTrack> m_Track; 
  
 
  ClassDef(HTTOptionalEventInfo, 1)
};

std::ostream& operator<<(std::ostream& , const HTTOptionalEventInfo& );
#endif // HTTOptionalEventInfo_H

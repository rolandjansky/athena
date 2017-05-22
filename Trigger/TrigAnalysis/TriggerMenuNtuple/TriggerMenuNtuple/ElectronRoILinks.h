/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_ELECTRONROILINKS_H
#define TRIGGERMENUNTUPLE_ELECTRONROILINKS_H
/*
  ElectronRoILinks.h
*/
#include <vector>
#include "TriggerMenuNtuple/FeatureIndex.h"

class ElectronRoILinks {
public:
  ElectronRoILinks();
  ~ElectronRoILinks();

  const FeatureIndex& getEMClusterIndex() const { return m_EMClusterIndex; }
  const std::vector<FeatureIndex>& getInDetTrackIndex() const {
    return m_InDetTrackIndex;
  }
  const std::vector<FeatureIndex>& getTrigElectronIndex() const {
    return m_TrigElectronIndex;
  }
  const std::vector<FeatureIndex>& getEgammaIndex() const {
    return m_EgammaIndex;
  }

  void set(const FeatureIndex& index_EMCluster, 
	   const std::vector<FeatureIndex>& index_InDetTracks, 
	   const std::vector<FeatureIndex>& index_TrigElectrons);
	   
  void setElectronRoILinks(int index_EMCluster, 
			   const std::vector<int>& index_InDetTracks, 
			   const std::vector<int>& index_TrigElectron,
			   const std::vector<int>& index_Egamma,
			   int status_EMCluster, 
			   int status_InDetTracks, 
			   int status_TrigElectron,
			   int status_Egamma);

  void dump() const;
  bool isSameRoI(const ElectronRoILinks& x) const;
  bool operator==(const ElectronRoILinks& x) const;

private:
  FeatureIndex m_EMClusterIndex;
  std::vector<FeatureIndex> m_InDetTrackIndex;
  std::vector<FeatureIndex> m_TrigElectronIndex;
  std::vector<FeatureIndex> m_EgammaIndex;
};

#endif // TRIGGERMENUNTUPLE_ELECTRONROILINKS_H

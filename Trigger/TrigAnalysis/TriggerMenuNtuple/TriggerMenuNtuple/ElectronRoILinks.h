/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ElectronRoILinks_h__
#define __ElectronRoILinks_h__
/*
  ElectronRoILinks.h
*/
#include <vector>
#include "TriggerMenuNtuple/FeatureIndex.h"

class ElectronRoILinks {
public:
  ElectronRoILinks();
  ~ElectronRoILinks();

  const FeatureIndex& getEMClusterIndex() const { return EMClusterIndex; }
  const std::vector<FeatureIndex>& getInDetTrackIndex() const {
    return InDetTrackIndex;
  }
  const std::vector<FeatureIndex>& getTrigElectronIndex() const {
    return TrigElectronIndex;
  }
  const std::vector<FeatureIndex>& getEgammaIndex() const {
    return EgammaIndex;
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
  FeatureIndex EMClusterIndex;
  std::vector<FeatureIndex> InDetTrackIndex;
  std::vector<FeatureIndex> TrigElectronIndex;
  std::vector<FeatureIndex> EgammaIndex;
};

#endif // __ElectronRoILinks_h__

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_MUONROILINKS_H
#define TRIGGERMENUNTUPLE_MUONROILINKS_H
/*
  MuonRoILinks.h
*/
#include <vector>
#include "TriggerMenuNtuple/FeatureIndex.h"

class MuonRoILinks {
public:
  MuonRoILinks();
  ~MuonRoILinks();

  const FeatureIndex& getMuonFeatureIndex() const;
  const std::vector<FeatureIndex>& getInDetTrackIndex() const;
  const FeatureIndex& getCombinedMuonFeatureIndex() const;

  void set(const FeatureIndex& index_MuonFeature, 
	   const std::vector<FeatureIndex>& index_InDetTracks, 
	   const FeatureIndex& index_CombinedMuonFeature);
  void setMuonRoILinks(int index_MuonFeature, 
		       const std::vector<int>& index_InDetTracks, 
		       int index_CombinedMuonFeature,
		       int status_MuonFeature, 
		       int status_InDetTracks, 
		       int status_CombinedMuonFeature);

  bool operator==(const MuonRoILinks& x) const;

private:
  FeatureIndex m_MuonFeatureIndex;
  std::vector<FeatureIndex> m_InDetTrackIndex;
  FeatureIndex m_CombinedMuonFeatureIndex;

};

#endif // TRIGGERMENUNTUPLE_MUONROILINKS_H

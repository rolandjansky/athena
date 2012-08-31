/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MuonRoILinks_h__
#define __MuonRoILinks_h__
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
  FeatureIndex MuonFeatureIndex;
  std::vector<FeatureIndex> InDetTrackIndex;
  FeatureIndex CombinedMuonFeatureIndex;

};

#endif // __MuonRoILinks_h__

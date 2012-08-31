/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  MuonRoILinks.cxx
*/
#include "TriggerMenuNtuple/MuonRoILinks.h"

MuonRoILinks::MuonRoILinks() {
}

MuonRoILinks::~MuonRoILinks() {
}

void MuonRoILinks::set(const FeatureIndex& index_MuonFeature, 
		       const std::vector<FeatureIndex>& index_InDetTracks, 
		       const FeatureIndex& index_CombinedMuonFeature) {
  MuonFeatureIndex = index_MuonFeature;
  InDetTrackIndex = index_InDetTracks;
  CombinedMuonFeatureIndex = index_CombinedMuonFeature;
}

void MuonRoILinks::setMuonRoILinks(int index_MuonFeature, 
				const std::vector<int>& index_InDetTracks, 
				int index_CombinedMuonFeature,
				int status_MuonFeature, 
				int status_InDetTracks, 
				int status_CombinedMuonFeature) {
  MuonFeatureIndex.set(index_MuonFeature, status_MuonFeature);
  for(int i=0; i < static_cast<int>(index_InDetTracks.size()); i++){
    FeatureIndex x;
    x.set(index_InDetTracks[i], status_InDetTracks);
    InDetTrackIndex.push_back(x);
 }
  CombinedMuonFeatureIndex.set(index_CombinedMuonFeature,status_CombinedMuonFeature);
}


const FeatureIndex& MuonRoILinks::getMuonFeatureIndex() const {
  return MuonFeatureIndex;
}
const std::vector<FeatureIndex>& MuonRoILinks::getInDetTrackIndex() const {
  return InDetTrackIndex;
}
const FeatureIndex& MuonRoILinks::getCombinedMuonFeatureIndex() const {
  return CombinedMuonFeatureIndex;
}

bool MuonRoILinks::operator==(const MuonRoILinks& x) const {
  if (MuonFeatureIndex == x.MuonFeatureIndex && 
      InDetTrackIndex == x.InDetTrackIndex && 
      CombinedMuonFeatureIndex == x.CombinedMuonFeatureIndex) {
    return true;
  } else {
    return false;
  }
}


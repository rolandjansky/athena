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
  m_MuonFeatureIndex = index_MuonFeature;
  m_InDetTrackIndex = index_InDetTracks;
  m_CombinedMuonFeatureIndex = index_CombinedMuonFeature;
}

void MuonRoILinks::setMuonRoILinks(int index_MuonFeature, 
				const std::vector<int>& index_InDetTracks, 
				int index_CombinedMuonFeature,
				int status_MuonFeature, 
				int status_InDetTracks, 
				int status_CombinedMuonFeature) {
  m_MuonFeatureIndex.set(index_MuonFeature, status_MuonFeature);
  for(int i=0; i < static_cast<int>(index_InDetTracks.size()); i++){
    FeatureIndex x;
    x.set(index_InDetTracks[i], status_InDetTracks);
    m_InDetTrackIndex.push_back(x);
 }
  m_CombinedMuonFeatureIndex.set(index_CombinedMuonFeature,status_CombinedMuonFeature);
}


const FeatureIndex& MuonRoILinks::getMuonFeatureIndex() const {
  return m_MuonFeatureIndex;
}
const std::vector<FeatureIndex>& MuonRoILinks::getInDetTrackIndex() const {
  return m_InDetTrackIndex;
}
const FeatureIndex& MuonRoILinks::getCombinedMuonFeatureIndex() const {
  return m_CombinedMuonFeatureIndex;
}

bool MuonRoILinks::operator==(const MuonRoILinks& x) const {
  if (m_MuonFeatureIndex == x.m_MuonFeatureIndex && 
      m_InDetTrackIndex == x.m_InDetTrackIndex && 
      m_CombinedMuonFeatureIndex == x.m_CombinedMuonFeatureIndex) {
    return true;
  } else {
    return false;
  }
}


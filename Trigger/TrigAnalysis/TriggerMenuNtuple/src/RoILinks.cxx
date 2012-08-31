/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoILinks.cxx
*/
#include "TriggerMenuNtuple/RoILinks.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include <algorithm>
#include <iostream>

using namespace std;

RoILinks::RoILinks() {
}

RoILinks::~RoILinks() {
}

int RoILinks::addCombLinks(const CombLinks& links) {
  std::vector<CombLinks>::const_iterator p;
  int i=0;
  bool foundit=false;
  for (p=CombIndex.begin(); p!=CombIndex.end(); ++p) {
    if (p->RoIType() != links.RoIType()) continue;
    if ( (*p) == links) {
      foundit = true;
      break;
    }
    i ++;
  }
  if (!foundit) {
    CombIndex.push_back(links);
    i = -1;
    for (p=CombIndex.begin(); p!=CombIndex.end(); ++p) {
      if (p->RoIType() == links.RoIType()) i++;
    }
  }
  return i;
}

int RoILinks::addMuonRoILinks(const MuonRoILinks& MuonRoI) {
  std::vector<MuonRoILinks>::const_iterator p;
  int i=0;
  bool foundit=false;
  for (p=MuonRoIIndex.begin(); p!=MuonRoIIndex.end(); ++p) {
    if ( (*p) == MuonRoI) {
      foundit = true;
      break;
    }
    i ++;
  }
  if (!foundit) {
    MuonRoIIndex.push_back(MuonRoI);
    i = static_cast<int>(MuonRoIIndex.size()) - 1;
  }
  return i;
}
int RoILinks::addMuonRoILinks(int index_MuonFeature, 
			      const std::vector<int>& index_InDetTracks, 
			      int index_CombinedMuonFeature,
			      int status_MuonFeature, 
			      int status_InDetTracks, 
			      int status_CombinedMuonFeature){
  MuonRoILinks MuonRoI;
  MuonRoI.setMuonRoILinks(index_MuonFeature, 
			  index_InDetTracks, 
			  index_CombinedMuonFeature,
			  status_MuonFeature, 
			  status_InDetTracks, 
			  status_CombinedMuonFeature);
  return addMuonRoILinks(MuonRoI);
}

int RoILinks::addElectronRoILinks(const ElectronRoILinks& ElectronRoI) {
  std::vector<ElectronRoILinks>::const_iterator p;
  int i=0;
  bool foundit=false;
  for (p=ElectronRoIIndex.begin(); p!=ElectronRoIIndex.end(); ++p) {
    if ( (*p) == ElectronRoI) {
      foundit = true;
      break;
    }
    i ++;
  }
  if (!foundit) {
    ElectronRoIIndex.push_back(ElectronRoI);
    i = static_cast<int>(ElectronRoIIndex.size()) - 1;
  }
  return i;
}

int RoILinks::addMuonRoILinks(const FeatureIndex& index_MuonFeature, 
			      const std::vector<FeatureIndex>& index_InDetTracks, 
			      const FeatureIndex& index_CombinedMuonFeature) {
  MuonRoILinks x;
  x.set(index_MuonFeature, 
	index_InDetTracks, 
	index_CombinedMuonFeature);
  addMuonRoILinks(x);
  return 0;
}

int RoILinks::addElectronRoILinks(const FeatureIndex& index_EMCluster, 
				  const std::vector<FeatureIndex>& index_InDetTracks, 
				  const std::vector<FeatureIndex>& index_TrigElectrons) {
  ElectronRoILinks x;
  x.set(index_EMCluster, 
	index_InDetTracks, 
	index_TrigElectrons);
  addElectronRoILinks(x);
  return 0;
}

int RoILinks::addElectronRoILinks(int index_EMCluster,
				  const std::vector<int>& index_InDetTracks, 
				  const std::vector<int>& index_TrigElectron,
				  const std::vector<int>& index_Egamma,
				  int status_EMCluster, 
				  int status_InDetTracks, 
				  int status_TrigElectron,
				  int status_Egamma){
  ElectronRoILinks ElectronRoI;
  ElectronRoI.setElectronRoILinks(index_EMCluster, 
				  index_InDetTracks, 
				  index_TrigElectron,
				  index_Egamma,
				  status_EMCluster, 
				  status_InDetTracks, 
				  status_TrigElectron,
				  status_Egamma);
  return addElectronRoILinks(ElectronRoI);
}


const std::vector<MuonRoILinks>& RoILinks::getMuonRoILinks() const {
  return MuonRoIIndex;
}
const std::vector<ElectronRoILinks>& RoILinks::getElectronRoILinks() const {
  return ElectronRoIIndex;
}

std::vector<ElectronRoILinks> 
RoILinks::getElectronRoILinks(const ChainEntry& chain) const {
  const std::vector<int>& roi_types = chain.getRoITypes();
  const std::vector<int>& roi_indices = chain.getRoIIndices();
  const unsigned int n = roi_types.size();
  std::vector<ElectronRoILinks> v;

  for (unsigned int i=0; i<n; ++i) {
    if (roi_types[i] == ChainEntry::kElectronRoIType) {
      v.push_back(ElectronRoIIndex[roi_indices[i]]);
    }
  }
  return v;
}

std::vector<CombLinks> RoILinks::getCombLinks(int roi_type) const {
  std::vector<CombLinks> x;
  std::vector<CombLinks>::const_iterator p;

  for (p=CombIndex.begin(); p!=CombIndex.end(); ++p) {
    if (roi_type < 0 || p->RoIType() == roi_type) x.push_back(*p);
  }
  return x;
}

std::vector<CombLinks> RoILinks::getCombLinks(const ChainEntry& chain) const {
  const std::vector<int>& types = chain.getRoITypes();
  const std::vector<int>& indices = chain.getRoIIndices();
//   cout << "types: " << types.size() << endl;
//   cout << "indices: " << indices.size() << endl;

  std::vector<CombLinks> x;
  unsigned int i;
  const unsigned int n = indices.size();

  for (i=0; i<n; ++i) {
    //    cout << "index " << i << endl;
    const std::vector<CombLinks>& links = getCombLinks(types[i]);
    //    cout  << "links size: " << links.size() << endl;
    int index = indices[i];
    if (index < static_cast<int>(links.size())) {
      x.push_back(links[index]);
    } else {
      // Index out-of-range for this RoI type, this should never happen      
    }
  }
  return x;
}


void RoILinks::clear() {
  MuonRoIIndex.clear();
  ElectronRoIIndex.clear();
  TauRoIIndex.clear();
  JetRoIIndex.clear();
  CombIndex.clear();
}


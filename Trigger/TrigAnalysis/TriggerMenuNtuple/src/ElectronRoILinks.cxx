/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ElectronRoILinks.cxx
*/
#include "TriggerMenuNtuple/ElectronRoILinks.h"
#include <iostream>

using namespace std;

ElectronRoILinks::ElectronRoILinks() {
}

ElectronRoILinks::~ElectronRoILinks() {
}

void ElectronRoILinks::set(const FeatureIndex& index_EMCluster, 
			   const std::vector<FeatureIndex>& index_InDetTracks, 
			   const std::vector<FeatureIndex>& index_TrigElectrons) {
  EMClusterIndex = index_EMCluster;
  InDetTrackIndex = index_InDetTracks;
  TrigElectronIndex = index_TrigElectrons;
}

void ElectronRoILinks::setElectronRoILinks(int index_EMCluster, 
					   const std::vector<int>& index_InDetTracks, 
					   const std::vector<int>& index_TrigElectron,
					   const std::vector<int>& index_Egamma,
					   int status_EMCluster, 
					   int status_InDetTracks, 
					   int status_TrigElectron,
					   int status_Egamma) {
  EMClusterIndex.set(index_EMCluster, status_EMCluster);

  for(int i=0; i < static_cast<int>(index_InDetTracks.size()); i++){
    FeatureIndex x;
    x.set(index_InDetTracks[i], status_InDetTracks);
    InDetTrackIndex.push_back(x);
  }

  for(int i=0; i < static_cast<int>(index_TrigElectron.size()); i++){
    FeatureIndex x;
    x.set(index_TrigElectron[i],status_TrigElectron);
    TrigElectronIndex.push_back(x);
  }

  for(int i=0; i < static_cast<int>(index_Egamma.size()); i++){
    FeatureIndex x;
    x.set(index_Egamma[i],status_Egamma);
    EgammaIndex.push_back(x);
  }

}


void ElectronRoILinks::dump() const {
  EMClusterIndex.dump("L2 EMCluster");
  FeatureIndex::dump(InDetTrackIndex.begin(), InDetTrackIndex.end(), 
		     "InDetTrack");
  FeatureIndex::dump(TrigElectronIndex.begin(), TrigElectronIndex.end(), 
		     "TrigElectron");
//   std::vector<FeatureIndex>::const_iterator p;
//   cout << "L2 EMCluster : " << EMClusterIndex << endl;
//   int i=0;
//   for (p=InDetTrackIndex.begin(); p!=InDetTrackIndex.end(); ++p, ++i) {
//     cout << "InDetTrack[" << i << "] : " << (*p) << endl;
//   }
//   for (p=TrigElectronIndex.begin(),i=0; p!=TrigElectronIndex.end(); ++p,++i) {
//     cout << "TrigElectron[" << i << "] : " << (*p) << endl;
//   }
//   for (p=EgammaIndex.begin(),i=0; p!=EgammaIndex.end(); ++p,++i) {
//     cout << "Egamma[" << i << "] : " << (*p) << endl;
//   }
}

bool ElectronRoILinks::isSameRoI(const ElectronRoILinks& x) const {
  if (EMClusterIndex.sameIndex(x.EMClusterIndex) && 
      sameIndex(InDetTrackIndex, x.InDetTrackIndex) && 
      sameIndex(TrigElectronIndex, x.TrigElectronIndex)) {
    return true;
  } else {
    return false;
  }
}

bool ElectronRoILinks::operator==(const ElectronRoILinks& x) const {
  if (EMClusterIndex == x.EMClusterIndex && 
      InDetTrackIndex == x.InDetTrackIndex && 
      TrigElectronIndex == x.TrigElectronIndex &&
      EgammaIndex == x.EgammaIndex) {
    return true;
  } else {
    return false;
  }
}


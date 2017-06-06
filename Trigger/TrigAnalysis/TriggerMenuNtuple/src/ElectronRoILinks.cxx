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
  m_EMClusterIndex = index_EMCluster;
  m_InDetTrackIndex = index_InDetTracks;
  m_TrigElectronIndex = index_TrigElectrons;
}

void ElectronRoILinks::setElectronRoILinks(int index_EMCluster, 
					   const std::vector<int>& index_InDetTracks, 
					   const std::vector<int>& index_TrigElectron,
					   const std::vector<int>& index_Egamma,
					   int status_EMCluster, 
					   int status_InDetTracks, 
					   int status_TrigElectron,
					   int status_Egamma) {
  m_EMClusterIndex.set(index_EMCluster, status_EMCluster);

  for(int i=0; i < static_cast<int>(index_InDetTracks.size()); i++){
    FeatureIndex x;
    x.set(index_InDetTracks[i], status_InDetTracks);
    m_InDetTrackIndex.push_back(x);
  }

  for(int i=0; i < static_cast<int>(index_TrigElectron.size()); i++){
    FeatureIndex x;
    x.set(index_TrigElectron[i],status_TrigElectron);
    m_TrigElectronIndex.push_back(x);
  }

  for(int i=0; i < static_cast<int>(index_Egamma.size()); i++){
    FeatureIndex x;
    x.set(index_Egamma[i],status_Egamma);
    m_EgammaIndex.push_back(x);
  }

}


void ElectronRoILinks::dump() const {
  m_EMClusterIndex.dump("L2 EMCluster");
  FeatureIndex::dump(m_InDetTrackIndex.begin(), m_InDetTrackIndex.end(), 
		     "InDetTrack");
  FeatureIndex::dump(m_TrigElectronIndex.begin(), m_TrigElectronIndex.end(), 
		     "TrigElectron");
//   std::vector<FeatureIndex>::const_iterator p;
//   cout << "L2 EMCluster : " << m_EMClusterIndex << endl;
//   int i=0;
//   for (p=m_InDetTrackIndex.begin(); p!=m_InDetTrackIndex.end(); ++p, ++i) {
//     cout << "InDetTrack[" << i << "] : " << (*p) << endl;
//   }
//   for (p=m_TrigElectronIndex.begin(),i=0; p!=m_TrigElectronIndex.end(); ++p,++i) {
//     cout << "TrigElectron[" << i << "] : " << (*p) << endl;
//   }
//   for (p=m_EgammaIndex.begin(),i=0; p!=m_EgammaIndex.end(); ++p,++i) {
//     cout << "Egamma[" << i << "] : " << (*p) << endl;
//   }
}

bool ElectronRoILinks::isSameRoI(const ElectronRoILinks& x) const {
  if (m_EMClusterIndex.sameIndex(x.m_EMClusterIndex) && 
      sameIndex(m_InDetTrackIndex, x.m_InDetTrackIndex) && 
      sameIndex(m_TrigElectronIndex, x.m_TrigElectronIndex)) {
    return true;
  } else {
    return false;
  }
}

bool ElectronRoILinks::operator==(const ElectronRoILinks& x) const {
  if (m_EMClusterIndex == x.m_EMClusterIndex && 
      m_InDetTrackIndex == x.m_InDetTrackIndex && 
      m_TrigElectronIndex == x.m_TrigElectronIndex &&
      m_EgammaIndex == x.m_EgammaIndex) {
    return true;
  } else {
    return false;
  }
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonLayerROTs.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

namespace Muon {

  MuonLayerROTs::MuonLayerROTs(){
    m_clustersPerTechnology.resize(MuonStationIndex::TechnologyIndexMax);
  }

  MuonLayerROTs::~MuonLayerROTs(){
    // clean-up memory
    for( auto mdt : m_mdts ) delete mdt;
    for( auto& clvec : m_clustersPerTechnology ){
      for( auto cl : clvec ) delete cl; 
    }
  }

  void MuonLayerROTs::addMdts( const std::vector<const MdtDriftCircleOnTrack*>& mdts ) {
    m_mdts.insert(m_mdts.end(),mdts.begin(),mdts.end());
  }

  void MuonLayerROTs::addClusters( const std::vector<const MuonClusterOnTrack*>& clusters, MuonStationIndex::TechnologyIndex tech ) {
    m_clustersPerTechnology[tech].insert(m_clustersPerTechnology[tech].end(),clusters.begin(),clusters.end());
  }

  void MuonLayerROTs::replaceMdts( const std::vector<const MdtDriftCircleOnTrack*>& mdts ) {
    // clean-up memory and add
    for( auto mdt : m_mdts ) delete mdt;
    m_mdts.clear();
    addMdts(mdts);
  }

  void MuonLayerROTs::replaceClusters( const std::vector<const MuonClusterOnTrack*>& clusters, MuonStationIndex::TechnologyIndex tech ) {
    for( auto cl : m_clustersPerTechnology[tech] ) delete cl;
    m_clustersPerTechnology[tech].clear();
    addClusters(clusters,tech);
  }


}

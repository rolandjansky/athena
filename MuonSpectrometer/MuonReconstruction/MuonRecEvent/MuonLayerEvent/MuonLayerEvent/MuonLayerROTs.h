/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERROTS_H
#define MUON_MUONLAYERROTS_H

#include <vector>
#include "MuonIdHelpers/MuonStationIndex.h"

namespace Muon {

  class MdtDriftCircleOnTrack;
  class MuonClusterOnTrack;

  /** struct holding RIO_OnTracks for a given layer */
  class MuonLayerROTs {
  public:
    /** constructor */
    MuonLayerROTs();

    /** destructor */
    ~MuonLayerROTs();

    /** add MDTs, will merge them with existing MDT's. Takes ownership of all pointers  */
    void addMdts( const std::vector<const MdtDriftCircleOnTrack*>& mdts );

    /** add MDTs, will remove any existing MDT's. Takes ownership of all pointers  */
    void replaceMdts( const std::vector<const MdtDriftCircleOnTrack*>& mdts );

    /** add MuonClusters of a given technology, will merge them with existing clusters. Takes ownership of all pointers  */
    void addClusters( const std::vector<const MuonClusterOnTrack*>& clusters, MuonStationIndex::TechnologyIndex tech );

    /** add MuonClusters of a given technology, will remove any existing clusters. Takes ownership of all pointers  */
    void replaceClusters( const std::vector<const MuonClusterOnTrack*>& clusters, MuonStationIndex::TechnologyIndex tech );

    /** access calibrated MDT's */
    const std::vector<const MdtDriftCircleOnTrack*>& getMdts() const;

    /** access calibrated MuonClusters for a given technolgy */
    const std::vector<const MuonClusterOnTrack*>&    getClusters( MuonStationIndex::TechnologyIndex tech ) const;
    
  private:
    /** no copying nor assignment allow, use shared pointers instead */
    MuonLayerROTs& operator=(const MuonLayerROTs&) = delete;
    MuonLayerROTs(const MuonLayerROTs&) = delete;

    /** payload */
    std::vector<const MdtDriftCircleOnTrack*>             m_mdts;
    std::vector< std::vector<const MuonClusterOnTrack*> > m_clustersPerTechnology;
  };

  inline const std::vector<const MdtDriftCircleOnTrack*>& MuonLayerROTs::getMdts() const {
    return m_mdts;
  }

  inline const std::vector<const MuonClusterOnTrack*>& MuonLayerROTs::getClusters( MuonStationIndex::TechnologyIndex tech ) const {
    return m_clustersPerTechnology[tech];
  }
}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>

#include "eflowRec/EtaPhiLUT.h"

namespace eflowRec {

  /// define 2*Pi
  static constexpr float TWOPI = 2*M_PI;

  /// hepler function to ensure phi is within +-Pi
  float phiInRange(float phi) {
    while (phi >= M_PI) phi -= TWOPI;
    while (phi < -M_PI) phi += TWOPI;
    return phi;
  }

  /// calculate phi index for a given phi
  unsigned int phiIndex(float phi, float binsize) { return (phi + M_PI)/binsize; }

  EtaPhiLUT::EtaPhiLUT( unsigned int nbins ) :
    m_nphiBins(nbins),
    m_phiBinSize(TWOPI/m_nphiBins),
    m_phiBinnedLookUpTable(m_nphiBins)
    {}

  void EtaPhiLUT::fill(eflowRecClusterContainer& clustersin)
  {
    for(eflowRecCluster* cluster : clustersin) {
      int index = phiIndex(cluster->getCluster()->phi(), m_phiBinSize);
      m_phiBinnedLookUpTable[index].push_back(cluster);
    }

    for( auto& vec : m_phiBinnedLookUpTable ) {
      std::stable_sort(vec.begin(),vec.end(),[](const eflowRecCluster* c1, const eflowRecCluster* c2) { return c1->getCluster()->eta() < c2->getCluster()->eta(); });
    }
  }

  std::vector<eflowRecCluster*> EtaPhiLUT::clustersInCone( float eta, float phi, float dr ) const {
    std::vector<eflowRecCluster*> result;

    // Indices corresponding to phi range
    unsigned int iPhiMin = phiIndex( phiInRange(phi-dr), m_phiBinSize );
    unsigned int iPhiMax = phiIndex( phiInRange(phi+dr), m_phiBinSize );

    // Extract index ranges to iterate over
    std::vector< std::pair<int,int> > iPhiRanges;
    if( iPhiMin < iPhiMax ) {
      iPhiRanges.push_back( std::make_pair(iPhiMin,iPhiMax) );
    } else { // special treatment for phi-wrapping
      iPhiRanges.push_back( std::make_pair(0,iPhiMax) );
      iPhiRanges.push_back( std::make_pair(iPhiMin,m_nphiBins-1) );
    }

    float dr2Cut = dr*dr;

    // loop over ranges
    for( auto& range : iPhiRanges ){
      unsigned int indexMin = range.first;
      unsigned int indexMax = range.second;
      for( ; indexMin <= indexMax; ++indexMin ){
        const std::vector<eflowRecCluster*>& phiClusters = m_phiBinnedLookUpTable[indexMin];

        // Get iterators for clusters in relevant eta range
        auto it_min = std::lower_bound (phiClusters.begin(), phiClusters.end(), eta-dr, [] (const eflowRecCluster* cl, float etaval) {return cl->getCluster()->eta()<etaval;} );
        auto it_max = std::upper_bound (it_min, phiClusters.end(), eta+dr, [] (float etaval, const eflowRecCluster* cl) {return etaval<cl->getCluster()->eta();} );

        // Apply deltaR cut
        for( ;it_min!=it_max;++it_min ){
          const xAOD::CaloCluster& xcluster = *(*it_min)->getCluster();
          float deta = eta - xcluster.eta();
          float dphi = phiInRange(phi - xcluster.phi());
          float dr2 = deta*deta + dphi*dphi;
          if( dr2 < dr2Cut ) result.push_back(*it_min);
        }
      }
    }
    return result;
  }

} // namespace

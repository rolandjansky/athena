/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EtaPhi Look-up table for eflowRecClusters
//     author: Teng Jian Khoo
//     date:   Jun 2020
//
// Inspired by IParticlesLookUpTable from ParticlesInConeTools
// By binning in phi and sorting in eta, extraction of objects in
// a small eta/phi region can be accelerated.
// Useful to reduce the number of distance computations needed
// for e.g. track-cluster matching

#ifndef EFLOWREC_ETAPHILOOKUPTABLE_H
#define EFLOWREC_ETAPHILOOKUPTABLE_H

#include <vector>

#include "xAODCaloEvent/CaloCluster.h"
#include "eflowRec/eflowRecCluster.h"

namespace eflowRec {

  /** 2D look up table for eflowRecClusters */
  class EtaPhiLUT {
  public:
    /** constructor taking the desired binsize */
  EtaPhiLUT( unsigned int nbins = 50 );

  void fill(eflowRecClusterContainer& clustersin);

  /** collect eflowRecClusters in a given cone */
  std::vector<eflowRecCluster*> clustersInCone( float eta, float phi, float dr ) const;

  private:

    unsigned int m_nphiBins;    /// number of bins
    float m_phiBinSize;  /// bin size
    std::vector< std::vector<eflowRecCluster*> > m_phiBinnedLookUpTable; /// the look-up table
  };

}	// end of namespace

#endif

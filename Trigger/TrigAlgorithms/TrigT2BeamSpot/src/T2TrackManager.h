/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
///
/// $Id: T2TrackManager.h 793164 2017-01-20 03:59:26Z ssnyder $
///
/// T2TrackManager.h, (c) ATLAS Detector software
/// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
///
/// Sorting/Splitting class for tracks to be passed to the 
/// online beam spot vertex fitter
///
/// Authors : Emanuel Strauss
///
/// Date : 10 March, 2010
///
///============================================================

#ifndef TRIGT2BEAMSPOT_T2TRACKMANAGER_H
#define TRIGT2BEAMSPOT_T2TRACKMANAGER_H

#include "TrkTrack/TrackCollection.h"
#include "AthContainers/ConstDataVector.h"

#include <vector>
#include <cmath>

namespace PESA
{
  
  class T2TrackManager
  {
  public:

    // List of the splitter algorithms
    // TODO: Only Alternating split works, Pt does things that look strange
    //   and it should be validated before it can be used.
    enum Algorithm { Alternating, Pt /*, Phi, NorthSouth, Charge */ };

    explicit T2TrackManager(int nSplit=2, Algorithm alg=Alternating) : m_nSplit(nSplit), m_alg(alg) {}

    // Return a vector of sub-clusters
    std::vector< ConstDataVector<TrackCollection> > split(const TrackCollection& cluster, const EventContext& ctx) const;

  private:
    
    // The splitter algorithms
    int alternatingSplit(int& key) const;         // 1 for me, 1 for you, 2 for me, ...
    int orderedSplit(int& key, int nEntries) const; // 1, 2, 3 for me, 1, 2, 3 for you ...
    
    /// Data members
    const int m_nSplit = 2;      // How many output clusters
    const int m_alg = Alternating;         // Which algorithm to use
  };

} //PESA

#endif

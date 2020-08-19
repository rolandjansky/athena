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
    enum Algorithm { Alternating, Pt, Phi, NorthSouth, Charge };

    // Return a vector of sub-clusters
    std::vector< ConstDataVector<TrackCollection> > split(const TrackCollection& cluster, const EventContext& ctx) const;

  private:
    
    // The splitter algorithms
    constexpr int alternatingSplit(int key) const;         // 1 for me, 1 for you, 2 for me, ...
    constexpr int orderedSplit(int key, int nEntries) const; // 1, 2, 3 for me, 1, 2, 3 for you ...
    
    /// Data members
    int m_nSplit = 2;      // How many output clusters
    int m_alg = Alternating;         // Which algorithm to use
  };

} //PESA

#endif

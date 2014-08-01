/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
///
/// $Id: T2TrackManager.h 361213 2011-04-23 18:26:44Z bartoldu $
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

#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <vector>
#include <cmath>

namespace PESA
{
  
  class T2TrackManager
  {
  public:

    // List of the splitter algorithms
    enum Algorithm { Alternating, Pt, Phi, NorthSouth, Charge };

    // Constructor/Destructor
    T2TrackManager( int nSplit = 2, Algorithm alg = Alternating );

    ~T2TrackManager();

    // Later, should have a function to clean a track collection of
    // unwanted, poor quality, tracks:
    // TrigInDetTrackCollection * cleanTracks(TrigInDetTrackCollection * cluster);
    
    // Retrun a vector of sub-clusters
    std::vector< TrigInDetTrackCollection > split( const TrigInDetTrackCollection& cluster);

    // Get and Set parameters
    //    int GetNSplit() {return m_nSplit;}
    //    int GetAlg() {return m_alg;}
    //    void SetNSplit(int nSplit) {m_nSplit = nSplit;}
    //    void SetAlg(int alg) {m_alg = alg;}
    void ResetKey(int key = -1) { m_altKey = key;}
    
  private:
    
    // The splitter algorithms
    int alternatingSplit();         // 1 for me, 1 for you, 2 for me, ...
    int orderedSplit(int nEntries); // 1, 2, 3 for me, 1, 2, 3 for you ...
    
    /// Data members
    int m_nSplit;      // How many output clusters
    int m_alg;         // Which algorithm to use
    
    /// Algorithm helper objects
    int m_altKey;     // Alternating split key
    int m_ordKey;     // Ordered split key
  };

} //PESA

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @version: $Id: T2TrackClusterer.h 702277 2015-10-22 10:33:51Z smh $
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2TrackClusterer
 *
 * @brief Auxiliary class that clusters tracks in an TrigInDetTrackCollection
 *        in Z around a seed track
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 * @author David W. Miller, SLAC/Stanford University, <David.W.Miller@cern.ch>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2TRACKCLUSTERER_H
#define TRIGT2BEAMSPOT_T2TRACKCLUSTERER_H

/// Externals
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::GeV;
using Gaudi::Units::mm;

#include <string>
#include <vector>
#include <cmath>

namespace PESA
{

  class T2TrackClusterer
  {
  public:

    // Constructor
    T2TrackClusterer( double deltaZ = 10.*mm, double minPT = 1.*GeV, bool weightedZ = true, unsigned maxSize = 10000. )
      : m_deltaZ    ( deltaZ    )
      , m_minPT     ( minPT     )
      , m_weightedZ ( weightedZ )
      , m_maxSize   ( maxSize   )
      , m_seedZ0    ( 0. )
      , m_totalZ0Err( 0. )
      {}

    // Accessors
    double     seedZ0() const { return m_seedZ0    ; }
    double totalZ0Err() const { return m_totalZ0Err; }

    const TrigInDetTrackCollection&      cluster_TIDT() const { return m_cluster_TIDT;      }
    const TrackCollection&               cluster() const { return m_cluster;      }
    const TrigInDetTrackCollection& unusedTracks_TIDT() const { return m_unusedTracks_TIDT; }
    const TrackCollection&          unusedTracks() const { return m_unusedTracks; }

    // Methods
    double trackWeight( const TrigInDetTrack& track ) const;
    double trackWeight( const Trk::Track& track ) const;

    const TrigInDetTrackCollection& cluster( const TrigInDetTrackCollection& tracks );
    const TrackCollection& cluster( const TrackCollection& tracks );

  private:
    // Data members
    const double   m_deltaZ;
    const double   m_minPT;
    const bool     m_weightedZ;
    const unsigned m_maxSize;

    double m_seedZ0;
    double m_totalZ0Err;

    TrigInDetTrackCollection m_cluster_TIDT;
    TrackCollection m_cluster;
    TrigInDetTrackCollection m_unusedTracks_TIDT;
    TrackCollection m_unusedTracks;
  };

} // end namespace

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @version: $Id: T2TrackClusterer.h 793164 2017-01-20 03:59:26Z ssnyder $
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2TrackClusterer
 *
 * @brief Auxiliary class that clusters tracks in a TrackCollection
 *        in Z around a seed track
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 * @author David W. Miller, SLAC/Stanford University, <David.W.Miller@cern.ch>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2TRACKCLUSTERER_H
#define TRIGT2BEAMSPOT_T2TRACKCLUSTERER_H

#include <string>
#include <vector>
#include <utility>

/// Externals
#include "AthContainers/ConstDataVector.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/SystemOfUnits.h"
//using Gaudi::Units::GeV;
//using Gaudi::Units::mm;

namespace InDet { class BeamSpotData; }

namespace PESA
{

  class T2TrackClusterer
  {
  public:

    using TrackVector = std::vector<const Trk::Track*>;

    // option for calculating track perigee parameters
    enum TrackPerigee {
      perigee_original,   // Use original track perigee parameters, usually means
                          // perigee w.r.t. coordinate system origin x,y=0,0
      perigee_beamspot,   // Perigee to a beamspot position, ignoring tilt
      perigee_beamline,   // Perigee to a beam line, including tilt
    };

    // convert string to above enum value
    static TrackPerigee trackPerigeeFromString(const std::string& perigeeStr);

    // Constructor
    T2TrackClusterer(double deltaZ = 10.*Gaudi::Units::mm, double minPT = 1.*Gaudi::Units::GeV,
                     bool weightedZ = true, unsigned maxSize = 10000.,
                     TrackPerigee trackPerigee = perigee_original)
      : m_deltaZ    ( deltaZ    )
      , m_minPT     ( minPT     )
      , m_weightedZ ( weightedZ )
      , m_maxSize   ( maxSize   )
      , m_trackPerigee( trackPerigee )
      {}

    /**
     * Find one cluster in a set of tracks.
     *
     * If beamspot is nullptr then clustering is done with "perigee_original"
     * option, otherwise perigee option provided in a constructor is used.
     */
    const TrackVector& cluster(const TrackVector& tracks, const InDet::BeamSpotData* beamspot = nullptr);

    /// Z0 position of a seed track that was used for clustering.
    double     seedZ0() const { return m_seedZ0    ; }

    /// Uncertainty of Z0 position of a cluster.
    double totalZ0Err() const { return m_totalZ0Err; }

    /// This is the same vector as returned from clustering method,
    /// have to be called after return from a clustering method.
    const TrackVector& clusterTracks() const { return m_cluster; }

    /// Tracks that were not included into cluster,
    /// have to be called after return from a clustering method.
    const TrackVector& unusedTracks() const { return m_unusedTracks; }

  private:

    double trackWeight(const Trk::Track& track) const;

    // return perigee z0 for a track
    double trackPerigeeZ0(const Trk::Track& track, const InDet::BeamSpotData* beamspot) const;

    // Data members
    const double   m_deltaZ;
    const double   m_minPT;
    const bool     m_weightedZ;
    const unsigned m_maxSize;
    const TrackPerigee m_trackPerigee = perigee_original;

    double m_seedZ0 = 0.;
    double m_totalZ0Err = 0.;

    TrackVector m_cluster;
    TrackVector m_unusedTracks;
  };

} // end namespace

#endif

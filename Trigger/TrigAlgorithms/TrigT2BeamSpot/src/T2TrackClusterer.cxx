/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2TrackClusterer.cxx 793164 2017-01-20 03:59:26Z ssnyder $
//

#include "T2TrackClusterer.h"

#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include <cmath>
#include <stdexcept>

using std::abs;
using std::sqrt;
using std::sin;
using std::cos;
using std::tan;

using namespace PESA;


T2TrackClusterer::TrackPerigee
T2TrackClusterer::trackPerigeeFromString(const std::string& perigeeStr)
{
   if (perigeeStr == "original") {
      return perigee_original;
   } else if (perigeeStr == "beamspot") {
      return perigee_beamspot;
   } else if (perigeeStr == "beamline") {
      return perigee_beamline;
   } else {
     throw std::runtime_error("Invalid value of ClusterPerigee parameter: " + perigeeStr);
   }
}

double
T2TrackClusterer::trackWeight( const Trk::Track& track ) const
{
  const double trackZ0Err = Amg::error(*(track.perigeeParameters()->covariance()),Trk::z0);
  const double weight = ( m_weightedZ ) ? 1. / ( trackZ0Err*trackZ0Err ) : 1.;
  return weight;
}

const T2TrackClusterer::TrackVector&
T2TrackClusterer::cluster( const TrackVector& tracks, const InDet::BeamSpotData* beamspot )
{
  m_seedZ0      = 0.;
  m_totalZ0Err  = 0.;
  m_cluster.clear();
  m_unusedTracks.clear();

  if ( tracks.empty() ) {
    return m_cluster;
  }

  const Trk::Track* seedTrack = *tracks.begin();

  auto& params = seedTrack->perigeeParameters()->parameters();
  const double seedPT = std::abs(sin(params[Trk::theta])/params[Trk::qOverP]);

  if ( seedPT < m_minPT ) {
    m_unusedTracks.assign(tracks.begin(), tracks.end());
    return m_cluster;
  }

  double sumWeight = trackWeight(*seedTrack);
  m_seedZ0 = trackPerigeeZ0(*seedTrack, beamspot);

  m_cluster.push_back(seedTrack);

  for (auto track_itr = tracks.begin() + 1; track_itr != tracks.end(); ++track_itr) {
    const double trackZ0 = trackPerigeeZ0(**track_itr, beamspot);
    const double deltaZ  = trackZ0 - m_seedZ0;
    const double weight  = trackWeight( **track_itr );

    if (abs(deltaZ) <= m_deltaZ && m_cluster.size() < m_maxSize) {
      m_cluster.push_back(*track_itr);

      m_seedZ0 = ( m_seedZ0 * sumWeight + trackZ0 * weight ) / ( sumWeight + weight );
      sumWeight += weight;
    } else {
      m_unusedTracks.push_back(*track_itr);
    }
  }

  m_totalZ0Err = sqrt( 1. / sumWeight );

  return m_cluster;
}

double
T2TrackClusterer::trackPerigeeZ0(const Trk::Track& track, const InDet::BeamSpotData* beamspot) const
{
  auto& params0 = track.perigeeParameters()->parameters();
  if (m_trackPerigee == perigee_original or beamspot == nullptr) {
    return params0[Trk::z0];
  } else if (m_trackPerigee == perigee_beamspot or m_trackPerigee == perigee_beamline) {
    // TODO: beamline is doing the same as beamspot for now, I think it should be OK
    // with our track resolution, somethig to check in the future (when I retire)

    // This assumes that track perigee is defined at 0,0, transform is:
    //     z = z_0 + (B_x*cos(phi) + B_y*sin(phi)) / tan(theta)

    double z_0 = params0[Trk::z0];
    double phi = params0[Trk::phi];
    double theta = params0[Trk::theta];
    auto& beamPos = beamspot->beamPos();
    double B_x = beamPos[0];
    double B_y = beamPos[1];

    double z = z_0 + (B_x*cos(phi) + B_y*sin(phi)) / tan(theta);
    return z;

  } else {
    // fallback to "original"
    return params0[Trk::z0];
  }
}
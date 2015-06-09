/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2TrackClusterer.cxx 361213 2011-04-23 18:26:44Z bartoldu $
//

#include "T2TrackClusterer.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

#include <cmath>
using std::abs;
using std::sqrt;

using namespace PESA;


double
T2TrackClusterer::trackWeight( const TrigInDetTrack& track ) const
{
  const double trackZ0Err = track.param()->ez0();
  const double weight = ( m_weightedZ ) ? 1. / ( trackZ0Err*trackZ0Err ) : 1.;
  return weight;
}


const TrigInDetTrackCollection&
T2TrackClusterer::cluster( const TrigInDetTrackCollection& tracks )
{
  m_seedZ0      = 0.;
  m_totalZ0Err  = 0.;
  m_cluster     .clear( SG::VIEW_ELEMENTS );
  m_unusedTracks.clear( SG::VIEW_ELEMENTS );

  if ( tracks.empty() )
    {
      // FIXME: unusedTracks = tracks;
      return m_cluster;
    }

  TrigInDetTrack* seedTrack = *tracks.begin();
  const double seedPT = abs( seedTrack->param()->pT() );

  if ( seedPT < m_minPT )
    {
      m_unusedTracks = tracks;
      return m_cluster;
    }

  double sumWeight = trackWeight( *seedTrack );
  m_seedZ0 = seedTrack->param()->z0(); // becomes the weighted-average z0 of the cluster

  m_cluster.push_back( seedTrack );

  for ( TrigInDetTrackCollection::const_iterator track = tracks.begin() + 1;
        track != tracks.end(); ++track )
    {
      const double trackZ0 = (*track)->param()->z0();
      const double deltaZ  = trackZ0 - m_seedZ0;
      const double weight  = trackWeight( **track );

      if ( abs(deltaZ) <= m_deltaZ && m_cluster.size() < m_maxSize )
        {
          m_cluster.push_back( *track );

          m_seedZ0 = ( m_seedZ0 * sumWeight + trackZ0 * weight ) / ( sumWeight + weight );
          sumWeight += weight;
        }
      else
        {
          m_unusedTracks.push_back( *track );
        }
    }

  m_totalZ0Err = sqrt( 1. / sumWeight );

  return m_cluster;
}

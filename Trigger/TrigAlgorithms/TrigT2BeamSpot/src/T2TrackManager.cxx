/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
///
/// $Id: T2TrackManager.cxx 702277 2015-10-22 10:33:51Z smh $
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

// This class
#include "T2TrackManager.h"

//#include <iostream>
#include <cmath>

using std::abs;

using namespace PESA;


// Not much to do here now, will eventually init the config parameters
T2TrackManager::T2TrackManager( int nSplit, Algorithm alg )
  // Fill the defaults
  : m_nSplit      ( nSplit )
  , m_alg         ( alg    )
  , m_altKey      ( -1     )
  , m_ordKey      ( 0      )
{
}


// Will have to clean up after myself here
T2TrackManager::~T2TrackManager()
{
}


// The meat of the class, return a vector of split clusters
std::vector< TrigInDetTrackCollection >
T2TrackManager::split( const TrigInDetTrackCollection& cluster )
{
  const int nEntries = cluster.size();

  //std::cout << "Reserve space" << std::endl;

  // Set up the output, reserve space, init collections
  TrigInDetTrackCollection splitColl;
  splitColl.clear( SG::VIEW_ELEMENTS );
  splitColl.reserve( nEntries / m_nSplit );
  std::vector< TrigInDetTrackCollection > trackCollections( m_nSplit, splitColl );

  //if (m_alg == Pt)
  //sort(holder.begin(), holder.end(), ptSort);

  // Iterate over all the tracks in the cluster
  for ( TrigInDetTrackCollection::const_iterator c_itr = cluster.begin();
        c_itr != cluster.end(); ++c_itr ) {
    // By default (if the splitting algorithm doesn't exist) store
    // tracks in the 1st collection
    int nPos = 0;
    if (m_alg == Alternating)
      nPos = alternatingSplit();
    else if (m_alg == Pt)
      nPos = orderedSplit(nEntries);

    // Add the track to the appropriate collection
    trackCollections[nPos].push_back(*c_itr);
  }

  //  if (!m_doCluster) {
  return trackCollections;
}

// The meat of the class, return a vector of split clusters
std::vector< TrackCollection >
T2TrackManager::split( const TrackCollection& cluster )
{
  const int nEntries = cluster.size();

  //std::cout << "Reserve space" << std::endl;

  // Set up the output, reserve space, init collections
  TrackCollection splitColl;
  splitColl.clear( SG::VIEW_ELEMENTS );
  splitColl.reserve( nEntries / m_nSplit );
  std::vector< TrackCollection > trackCollections( m_nSplit, splitColl );

  //if (m_alg == Pt)
  //sort(holder.begin(), holder.end(), ptSort);

  // Iterate over all the tracks in the cluster
  for ( TrackCollection::const_iterator c_itr = cluster.begin();
        c_itr != cluster.end(); ++c_itr ) {
    // By default (if the splitting algorithm doesn't exist) store
    // tracks in the 1st collection
    int nPos = 0;
    if (m_alg == Alternating)
      nPos = alternatingSplit();
    else if (m_alg == Pt)
      nPos = orderedSplit(nEntries);

    // Add the track to the appropriate collection
    trackCollections[nPos].push_back(*c_itr);
  }

  //  if (!m_doCluster) {
  return trackCollections;
}


int
T2TrackManager::alternatingSplit()
{
  ++m_altKey;
  m_altKey %= m_nSplit;
  return m_altKey;
}


int
T2TrackManager::orderedSplit(const int nEntries)
{
  ++m_altKey;
  return m_altKey * m_nSplit / nEntries;
}

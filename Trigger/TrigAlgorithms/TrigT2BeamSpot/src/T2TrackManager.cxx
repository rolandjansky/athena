/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
///
/// $Id: T2TrackManager.cxx 793164 2017-01-20 03:59:26Z ssnyder $
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


// The meat of the class, return a vector of split clusters
std::vector<T2TrackManager::TrackVector>
T2TrackManager::split( const TrackVector& cluster, const EventContext& ctx ) const
{
  int key = ctx.eventID().event_number() %2 -1;
  const int nEntries = cluster.size();

  //std::cout << "Reserve space" << std::endl;

  // Set up the output, reserve space, init collections
  std::vector<TrackVector> trackCollections(m_nSplit);

  //if (m_alg == Pt)
  //sort(holder.begin(), holder.end(), ptSort);

  // Iterate over all the tracks in the cluster
  for (auto&& track: cluster) {
    // By default (if the splitting algorithm doesn't exist) store
    // tracks in the 1st collection
    int nPos = 0;
    if (m_alg == Alternating)
      nPos = alternatingSplit(key);
    else if (m_alg == Pt)
      nPos = orderedSplit(key, nEntries);

    // Add the track to the appropriate collection
    trackCollections[nPos].push_back(track);
  }

  //  if (!m_doCluster) {
  return trackCollections;
}


int T2TrackManager::alternatingSplit(int& key) const
{
  ++key;
  key %= m_nSplit;
  return key;
}


int T2TrackManager::orderedSplit(int& key, const int nEntries) const
{
  ++key;
  return key * m_nSplit / nEntries;
}

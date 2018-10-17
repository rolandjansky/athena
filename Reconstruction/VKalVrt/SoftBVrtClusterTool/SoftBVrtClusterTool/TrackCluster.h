/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_CLUSTER_H
#define TRACK_CLUSTER_H

#include "xAODTracking/TrackParticleContainer.h"
#include <iostream>
#include <vector>

namespace SoftBVrt {

  class TrackCluster {
  
  public:

    TLorentzVector m_sumTrack;  
    const xAOD::IParticle* m_seed = nullptr;
    std::vector<const xAOD::IParticle*> m_cluster_tracks;  
    std::vector<const xAOD::IParticle*> m_cluster_matched_tracks;  

    static bool trackptsorter(const xAOD::IParticle *p1, const xAOD::IParticle *p2);    

    size_t getNTracks() const;
   
    void addSeed(const xAOD::IParticle *track);
    void addTrack(const xAOD::IParticle *track);
    void replaceTracks(std::vector<const xAOD::IParticle*> newTracks);
  
    float getSharedTrackFraction(TrackCluster cluster) const;

    std::vector<const xAOD::IParticle*> getTracks() const;
    std::vector<const xAOD::IParticle*> getTrackUnion(TrackCluster cluster) const;
    std::vector<const xAOD::IParticle*> getTrackIntersection(TrackCluster cluster) const;
  
  };

}

#endif //> !TRACK_CLUSTER_H

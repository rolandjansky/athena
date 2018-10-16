#ifndef TRACK_CLUSTER_H
#define TRACK_CLUSTER_H

#include "xAODTracking/TrackParticleContainer.h"
#include <iostream>
#include <vector>

namespace SoftBVrt {

  class TrackCluster {
  
  public:

    TLorentzVector m_sumTrack;  
    const xAOD::IParticle* m_seed = 0;
    std::vector<const xAOD::IParticle*> m_cluster_tracks;  
    std::vector<const xAOD::IParticle*> m_cluster_matched_tracks;  

    static bool trackptsorter(const xAOD::IParticle *p1, const xAOD::IParticle *p2);    

    size_t getNTracks();
   
    void addSeed(const xAOD::IParticle *track);
    void addTrack(const xAOD::IParticle *track);
    void replaceTracks(std::vector<const xAOD::IParticle*> newTracks);
  
    float getSharedTrackFraction(TrackCluster cluster);

    std::vector<const xAOD::IParticle*> getTracks();
    std::vector<const xAOD::IParticle*> getTrackUnion(TrackCluster cluster);
    std::vector<const xAOD::IParticle*> getTrackIntersection(TrackCluster cluster);
  
  };

}

#endif //> !TRACK_CLUSTER_H

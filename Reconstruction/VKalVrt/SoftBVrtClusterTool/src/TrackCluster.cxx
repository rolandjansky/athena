/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SoftBVrtClusterTool/TrackCluster.h"

namespace SoftBVrt {

  bool TrackCluster::trackptsorter (const xAOD::IParticle *p1, const xAOD::IParticle *p2){
    return p1->pt() > p2->pt();
  }
  
  size_t TrackCluster::getNTracks() const {return m_cluster_tracks.size();}
   
  void TrackCluster::addSeed(const xAOD::IParticle *track)  {
    
    m_seed = track;
    
  }

  void TrackCluster::addTrack(const xAOD::IParticle *track) {

    m_cluster_tracks.push_back(track);
    m_sumTrack+=track->p4();

    std::sort (m_cluster_tracks.begin(), m_cluster_tracks.end(), TrackCluster::trackptsorter);

  }

  void TrackCluster::replaceTracks(std::vector<const xAOD::IParticle*> newTracks) {

    m_cluster_tracks.clear();
    m_cluster_tracks = newTracks;

  }

  std::vector<const xAOD::IParticle*> TrackCluster::getTracks() const {

    return m_cluster_tracks;

  }  
  
  float TrackCluster::getSharedTrackFraction(TrackCluster cluster) const {    
    
    int nshared = 0;
    
    for(const xAOD::IParticle* trk1 : m_cluster_tracks){

      for(const xAOD::IParticle* trk2 : cluster.getTracks() ){       

	if (abs(trk1->pt() - trk2->pt()) < 1) {

	  nshared += 1;
	  break;
	  
	}
      }      
    }
    
    return (double)nshared / m_cluster_tracks.size();

  }

  std::vector<const xAOD::IParticle*> TrackCluster::getTrackUnion(TrackCluster cluster) const {    
  
    std::vector<const xAOD::IParticle*> trackUnion;
    std::vector<const xAOD::IParticle*> clusterTracks = cluster.getTracks();

    std::set_union(m_cluster_tracks.begin(), m_cluster_tracks.end(), clusterTracks.begin(), clusterTracks.end(), std::back_inserter(trackUnion) );
    
    return trackUnion;

  }

  std::vector<const xAOD::IParticle*> TrackCluster::getTrackIntersection(TrackCluster cluster) const {    
  
    std::vector<const xAOD::IParticle*> trackIntersection;
    std::vector<const xAOD::IParticle*> clusterTracks = cluster.getTracks();

    std::set_intersection(m_cluster_tracks.begin(), m_cluster_tracks.end(), clusterTracks.begin(), clusterTracks.end(), std::back_inserter(trackIntersection) );
    
    return trackIntersection;

  }
  
}

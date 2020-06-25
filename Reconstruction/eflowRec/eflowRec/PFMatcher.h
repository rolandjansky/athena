/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatcher.h
 *
 *  Created on: 03.04.2014
 *      Author: tlodd
 */

#ifndef PFMATCHER_H_
#define PFMATCHER_H_

#include "eflowRec/PFMatchInterfaces.h"
#include <algorithm>
#include <utility>
#include <cmath>

class DistanceProvider;

namespace PFMatch {

  typedef std::pair<const ICluster*,double> MatchDistance;

/**
Checks if a cluster should be matched to a track or not, and has methods to return list of best matches.
*/
class TrackClusterMatcher {
public:
  TrackClusterMatcher( std::unique_ptr<IDistanceProvider> distanceProvider, double matchCut):
    m_distanceProvider(std::move(distanceProvider)), m_matchCut(matchCut) { setDRParameters(); }

  virtual ~TrackClusterMatcher() {};

  MatchDistance match(const ITrack* track, const ICluster* cluster) const;

  template<class ClusterType>
  MatchDistance bestMatchDRparametrized(ITrack* track, const std::vector<ClusterType*>& clusters) const;
  template<class ClusterType>
  std::vector<MatchDistance> bestMatches(ITrack* track, const std::vector<ClusterType*>& clusters, int nMatches, double energyThreshold) const;
  template<class ClusterType>
  double getDRCutSquared(ClusterType* theCluster) const;

private:

  std::unique_ptr<IDistanceProvider> m_distanceProvider;
  double m_matchCut;
   /** This stores the parameters to vary DR cut with Pt */
  float m_drcut_par[9][3];

  /** This sets the parameters in the above array */
  void setDRParameters();


};

template<class ClusterType>
   MatchDistance TrackClusterMatcher::bestMatchDRparametrized(ITrack* track, const std::vector<ClusterType*>& clusters) const {
   ClusterType* bestCluster = nullptr;
   double bestDistance(m_matchCut);
   unsigned int nClusters(clusters.size());
   for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
     
     ClusterType* thisCluster = clusters[iCluster];
     double thisDistance(m_distanceProvider->distanceBetween(track, clusters[iCluster]));
     double  mybestdistance = getDRCutSquared(thisCluster);
     
     if (thisDistance < mybestdistance){
       if (thisDistance < bestDistance) {
	 bestDistance = thisDistance;
	 bestCluster  = thisCluster;
       }
     } 	
   }
   
   return MatchDistance(bestCluster, bestDistance);
 }

template<class ClusterType>
std::vector<MatchDistance> TrackClusterMatcher::bestMatches(ITrack* track, const std::vector<ClusterType*>& clusters, int nMatches, double energyThreshold) const {

  std::vector<MatchDistance> result;
  unsigned const nClusters(clusters.size());

  if (nMatches == -1) {
    for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
      ClusterType* thisCluster = clusters[iCluster];
      double thisDistance(m_distanceProvider->distanceBetween(track, thisCluster));
      if (thisDistance < m_matchCut) {
        result.push_back(MatchDistance(thisCluster, thisDistance));
      }
    }
  }

  else{
    int nLoops = nMatches;
    std::vector<unsigned int> masked;
    std::vector<int> maskedType;
    for (int imatch = 0; imatch < nLoops; ++imatch) {
      ClusterType* bestCluster = nullptr;
      double bestDistance(m_matchCut);
      int iMasked = -1;
      for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
        /* Do not consider matched ones. */
        if (masked.size() != 0 && find(masked.begin(), masked.end(), iCluster) != masked.end()) {
          continue;
        }

        ClusterType* thisCluster = clusters[iCluster];

        /* Require that first matched cluster energy is above energyThreshold (10% of track energy). */
        if(imatch == 0 && thisCluster->e() <= energyThreshold) {
          continue;
        }

        /* Do not consider the same type (ECAL, HCAL or FCAL) ones. */
	/* First we check if another ECAL, HCAL or FCAL cluster is found */ 
        if ((maskedType.size() != 0 && find(maskedType.begin(), maskedType.end(),
					    thisCluster->getEfRecCluster()->getClusterType()) != maskedType.end())
            || (thisCluster->getEfRecCluster()->getClusterType() == 4)) { /* then also veto if it is type 4, which means "unknown" type */
          continue;
        }

        double thisDistance(m_distanceProvider->distanceBetween(track, thisCluster));

        if (thisDistance < bestDistance) {
          iMasked = iCluster;
          bestDistance = thisDistance;
          bestCluster = thisCluster;
        }
      }
      if (iMasked == -1 || nullptr == bestCluster) break;

      masked.push_back(iMasked);
      maskedType.push_back(bestCluster->getEfRecCluster()->getClusterType());

      result.push_back(MatchDistance(bestCluster, bestDistance));
    }
    assert(maskedType.size() == masked.size());
  }

  return result;

 }

template<class ClusterType>
 double TrackClusterMatcher::getDRCutSquared( ClusterType* theCluster) const {
    
    double m_coneRSq = 1.64*1.64;
    double coneRSq = m_coneRSq;
  int ieta = -1;
  double clusEta;
  
  clusEta = theCluster->eta();

  if (std::abs(clusEta)<0.6) ieta=0;
  if (std::abs(clusEta)>=0.6 && std::abs(clusEta)<1.6) ieta = 1 + int((std::abs(clusEta) - 0.6)/0.2) ;
  if (std::abs(clusEta)>=1.6 && std::abs(clusEta)<2.0) ieta = 6 ;
  if (std::abs(clusEta)>=2.0 && std::abs(clusEta)<2.5) ieta = 7 ;
  if (std::abs(clusEta)>=2.5) ieta = 8 ; 

  double clusterEnergy = theCluster->e()/1000.0;

  double drcut = m_drcut_par[ieta][0]+m_drcut_par[ieta][1]*exp(m_drcut_par[ieta][2]*clusterEnergy);
  coneRSq = drcut*drcut;

 if (coneRSq>m_coneRSq) coneRSq = m_coneRSq;
  double conemin = m_drcut_par[ieta][0]+m_drcut_par[ieta][1]*exp(m_drcut_par[ieta][2]*10);
  
  if (coneRSq<conemin*conemin) coneRSq = conemin*conemin; 
  return coneRSq;

  }

}

#endif /* PFMATCHER_H_ */

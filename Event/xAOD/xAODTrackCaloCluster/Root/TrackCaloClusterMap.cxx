// $Id: TrackCaloCluster_v1.cxx $

#include "xAODTrackCaloCluster/TrackCaloClusterMap.h"

namespace xAOD {
  
  /// Get all the particles
  const TrackParticleLinks_t& TrackCaloClusterMap::trackParticleLinks() const {
    return m_trackparticle;
  }
  
  /// Set all track particle links at once
  void TrackCaloClusterMap::setTrackParticleLinks( const TrackParticleLinks_t& trackParticles ) {
    m_trackparticle = trackParticles;
  }
  
  /// Get all the clusters associated with the TrackParticle
  const CaloClusterLinks_t& TrackCaloClusterMap::caloClusterLinks() const {
    return m_caloclusters;
  }
  
  /// Get all the clusters associated with the TrackParticle at position pos
  const ElementLink< xAOD::CaloClusterContainer >& TrackCaloClusterMap::caloClusterLink(int pos) const {
    return m_caloclusters.at(pos);
  }
  
  /// Set all calo cluster links at once
  void TrackCaloClusterMap::setCaloClustersLinks( const CaloClusterLinks_t& caloClusters ) {
    m_caloclusters = caloClusters;
  }
  
  /// set calo cluster at position pos. If pos not defined then it is pushed back
  void TrackCaloClusterMap::setCaloClusterLink(ElementLink< xAOD::CaloClusterContainer >& caloCluster, int pos) {
    if (pos<0 or pos>int(m_caloclusters.size()))
      m_caloclusters.push_back(caloCluster);
    else 
      m_caloclusters.at(pos) = caloCluster;
  }
  
  /// Get all the cluster weights  
  const std::vector< float >& TrackCaloClusterMap::clusterWeights() const {
    return m_weigths;
  }
  
  /// Get cluster weight at position position
  float TrackCaloClusterMap::clusterWeight(int pos) const {
    return m_weigths.at(pos); 
  }
  
  /// Set all the cluster weights  
  void TrackCaloClusterMap::setClusterWeights( const std::vector< float >& weights ) {
    m_weigths = weights; 
  }
  
  /// set cluster weigth at position pos
  void TrackCaloClusterMap::setClusterWeight( float& weight, int pos) {
    if (pos<0 or pos>int(m_weigths.size()))
      m_weigths.push_back(weight);
    else 
      m_weigths.at(pos) = weight;
  }
  
  /// Get cluster and weight. If pos not defined then it is pished back
  void TrackCaloClusterMap::getClusterAndWeight(ElementLink< xAOD::CaloClusterContainer >& caloCluster, float& weigth, int pos) const {
    caloCluster = caloClusterLink(pos);
    weigth      = clusterWeight(pos);
  }
  
  /// Set cluster and weight. If pos not defined then it is pished back
  void TrackCaloClusterMap::setClusterAndWeight(ElementLink< xAOD::CaloClusterContainer >& caloCluster, float& weigth, int pos) {
    setCaloClusterLink(caloCluster,pos);
    setClusterWeight(weigth,pos);
  }
}
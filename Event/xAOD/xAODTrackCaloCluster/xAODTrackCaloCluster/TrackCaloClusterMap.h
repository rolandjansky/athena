// Dear emacs, this is -*- c++ -*-
// $Id: TrackCaloClusterMap.h $
#ifndef XAODTRACKCALOCLUSTER_TRACKCALOCLUSTERMAP_H
#define XAODTRACKCALOCLUSTER_TRACKCALOCLUSTERMAP_H

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"
#include "xAODCore/AuxContainerBase.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


namespace xAOD {
  
  /// Type for the associated track particles
  typedef ElementLink< xAOD::TrackParticleContainer > TrackParticleLinks_t;
  /// Type for the associated track particles
  typedef std::vector< ElementLink< xAOD::CaloClusterContainer > > CaloClusterLinks_t;
      
  class TrackCaloClusterMap {
    
    /** Class describing a TrackCaloClusterMap 
     * 
     * @author Noemi Calace <Noemi.Calace@cern.ch>
     * 
     **/
  
    public:  
      TrackCaloClusterMap() {};
      ~TrackCaloClusterMap() {};
                  
      /// Get all the particles
      const TrackParticleLinks_t& trackParticleLinks() const;  
      /// Set all track particle links at once
      void setTrackParticleLinks( const TrackParticleLinks_t& trackParticles );
          
      /// Get all the clusters associated with the TrackParticle
      const CaloClusterLinks_t& caloClusterLinks() const;  
      /// Get all the clusters associated with the TrackParticle at position pos
      const ElementLink< xAOD::CaloClusterContainer >& caloClusterLink(int pos) const;  
      
      /// Set all calo cluster links at once
      void setCaloClustersLinks( const CaloClusterLinks_t& caloClusters );
      /// set calo cluster at position pos. If pos not defined then it is pushed back
      void setCaloClusterLink(ElementLink< xAOD::CaloClusterContainer >& caloCluster, int pos = -1);
      
      /// Get all the cluster weights  
      const std::vector< float >& clusterWeights() const;
      /// Get cluster weight at position position
      float clusterWeight(int pos) const;
      /// Set all the cluster weights  
      void setClusterWeights( const std::vector< float >& weights );
      /// set cluster weigth at position pos
      void setClusterWeight( float& weights, int pos = -1 );
      
      /// Get cluster and weight. If pos not defined then it is pished back
      void getClusterAndWeight(ElementLink< xAOD::CaloClusterContainer >& caloCluster, float& weigth, int pos = -1) const;
        
      /// Set cluster and weight. If pos not defined then it is pished back
      void setClusterAndWeight(ElementLink< xAOD::CaloClusterContainer >& caloCluster, float& weigth, int pos = -1);
  
    private:
      
      TrackParticleLinks_t         m_trackparticle;
      CaloClusterLinks_t           m_caloclusters;
      std::vector< float >         m_weigths;
      
  };
}

#endif //XAODTRACKCALOCLUSTER_TRACKCALOCLUSTERMAP_H
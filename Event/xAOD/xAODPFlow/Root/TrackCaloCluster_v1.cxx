/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// $Id: TrackCaloCluster_v1.cxx $

// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODPFlow/versions/TrackCaloCluster_v1.h"

namespace xAOD {

  TrackCaloCluster_v1::TrackCaloCluster_v1() : IParticle() {}
  
  TrackCaloCluster_v1::~TrackCaloCluster_v1() {}
  
  double TrackCaloCluster_v1::e() const {
    return genvecP4().E();
  }
 
  double TrackCaloCluster_v1::rapidity() const {
    return genvecP4().Rapidity();
  }
   
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, m)
  AUXSTORE_PRIMITIVE_GETTER(TrackCaloCluster_v1,    int, taste)
  
  TrackCaloCluster_v1::FourMom_t TrackCaloCluster_v1::p4() const {
    FourMom_t p4;
    p4.SetPtEtaPhiM( pt(), eta(), phi(), m() );       
    return p4;
  }

  TrackCaloCluster_v1::GenVecFourMom_t TrackCaloCluster_v1::genvecP4() const {
    return GenVecFourMom_t( pt(), eta(), phi(),m()); 
  }

  Type::ObjectType TrackCaloCluster_v1::type() const { 
     return Type::TrackCaloCluster;
  }
    
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackCaloCluster_v1,
				     ElementLink< xAOD::TrackParticleContainer >,
				     trackParticleLink, 
				     setTrackParticleLink)
  
  const xAOD::TrackParticle* TrackCaloCluster_v1::trackParticle() const {
    // The accessor:       
    static const SG::AuxElement::Accessor< ElementLink< xAOD::TrackParticleContainer > >  acc( "trackPartcleLink" );
    if( !acc.isAvailable( *this ) ) {
      return 0;       
    }
    if( !acc( *this ).isValid() ) {
      return 0;
    }
    return *(acc( *this ));
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackCaloCluster_v1, 
				     std::vector< ElementLink< xAOD::CaloClusterContainer > >, 
				     caloClusterLinks, 
				     setCaloClusterLinks)
  
  void TrackCaloCluster_v1::setParameters(float pt, float eta, float phi, float m, TrackCaloCluster_v1::Taste taste,
					  const ElementLink<xAOD::TrackParticleContainer> particle, 
					  const std::vector< ElementLink< xAOD::CaloClusterContainer > > clusters) {

    static const Accessor< float > acc1( "pt" );     
    acc1( *this ) = pt;     
    static const  Accessor< float > acc2( "eta" );     
    acc2( *this ) = eta;     
    static const  Accessor< float > acc3( "phi" );     
    acc3( *this ) = phi;     
    static const  Accessor< float > acc4( "m" );     
    acc4( *this ) = m;     
        
    static const Accessor< int > acc( "taste" );     
    acc( *this ) = (int)taste;
    
    setTrackParticleLink(particle);
    setCaloClusterLinks(clusters);
    
  }
  


} // namespace xAOD

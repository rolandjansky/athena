// $Id: TrackCaloCluster_v1.cxx $

// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrackCaloCluster/versions/TrackCaloCluster_v1.h"

namespace xAOD {

  TrackCaloCluster_v1::TrackCaloCluster_v1() : IParticle(), m_p4(), m_taste() {}
  
  TrackCaloCluster_v1::~TrackCaloCluster_v1(){}
  
  double TrackCaloCluster_v1::pt() const {
    return p4().Pt();
  }

  double TrackCaloCluster_v1::eta() const {
    return p4().Eta();
  }
  
  double TrackCaloCluster_v1::phi() const {
    return p4().Phi();
  }
  
  double TrackCaloCluster_v1::m() const {
    return p4().M();
  }
 
  double TrackCaloCluster_v1::e() const {
    return p4().E();
  }
  double TrackCaloCluster_v1::rapidity() const {
    return p4().Rapidity();
  }

  const TrackCaloCluster_v1::FourMom_t& TrackCaloCluster_v1::p4() const {
    return m_p4;
  }

  Type::ObjectType TrackCaloCluster_v1::type() const { // 
     return Type::TrackCaloCluster;
  }
  
  float TrackCaloCluster_v1::charge() const {
    // static Accessor< float > acc( "charge" );
    return (qOverP() > 0) ? 1 : ((qOverP() < 0) ? -1 : 0);
  }
  
  float TrackCaloCluster_v1::phi0() const {
    Accessor< float > acc( "phi" );
    return acc( *this );
  }
  
  AUXSTORE_PRIMITIVE_GETTER(TrackCaloCluster_v1, float, theta)
  AUXSTORE_PRIMITIVE_GETTER(TrackCaloCluster_v1, float, qOverP)
  
  void TrackCaloCluster_v1::setTaste(TrackCaloCluster_v1::Taste taste) {
    m_taste = taste; 
  }
  
  /// set the 4-vec
  void TrackCaloCluster_v1::setP4(double pt, double eta, double phi, double m) {
    
    m_p4.SetPtEtaPhiM(pt,eta,phi,m);

  }
    
  TrackCaloCluster_v1::Taste& TrackCaloCluster_v1::getTaste() const {
    return m_taste;
  }

} // namespace xAOD
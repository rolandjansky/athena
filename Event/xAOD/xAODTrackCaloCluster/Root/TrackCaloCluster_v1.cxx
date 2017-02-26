// $Id: TrackCaloCluster_v1.cxx $

// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrackCaloCluster/versions/TrackCaloCluster_v1.h"

namespace xAOD {

  TrackCaloCluster_v1::TrackCaloCluster_v1() : IParticle(), m_p4(), m_p4Cached( false ) {}
  
  TrackCaloCluster_v1::~TrackCaloCluster_v1(){}
  
  double TrackCaloCluster_v1::pt() const {
    return p4().Pt();
  }

  double TrackCaloCluster_v1::eta() const {
    return p4().Eta();
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1,float,double,phi)
 
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
    using namespace std;
  // Check if we need to reset the cached object:
    if( ! m_p4Cached ) {
      float p = 1/fabs(qOverP());
      float thetaT = theta();
      float phiT = phi();
      float sinTheta= sin(thetaT);
      float px = p*sinTheta*cos(phiT);
      float py = p*sinTheta*sin(phiT);
      float pz = p*cos(thetaT);
      float e  =  pow (139.570,2) + /// @todo Get value from somewhere. Also, the TrackParticle took the Pion mass - do we really want to do this? We have ParticleHypo?
        pow( px,2) + pow( py,2) + pow( pz,2);
      m_p4.SetPxPyPzE( px, py, pz, sqrt(e) );
      m_p4Cached = true;
    }
  // Return the cached object:
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
  
  void TrackCaloCluster_v1::setTaste(TrackCaloCluster_v1::Taste& taste) {
    m_taste = taste; 
  }
    
  TrackCaloCluster_v1::Taste& TrackCaloCluster_v1::getTaste() const {
    return m_taste;
  }

} // namespace xAOD
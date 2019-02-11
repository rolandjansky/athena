/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/NeutralParticle_v1.h"
// #include "xAODTracking/VertexContainer.h" FIXME - need to get ELs working to vertices for neutrals - currently causes compilation failure EJWM


namespace xAOD {

  NeutralParticle_v1::NeutralParticle_v1()
  : IParticle(){
  }
  
  NeutralParticle_v1::NeutralParticle_v1(const NeutralParticle_v1& tp ) 
  : IParticle( tp ) {
    makePrivateStore( tp );
  }
  
  NeutralParticle_v1& NeutralParticle_v1::operator=(const NeutralParticle_v1& tp ){
    if(this == &tp) return *this;
    
    if(!hasStore() ) makePrivateStore();
    this->IParticle::operator=( tp );
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters.reset();
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    return *this;
  }

  NeutralParticle_v1::~NeutralParticle_v1(){
  }
  
  double NeutralParticle_v1::pt() const {
    return genvecP4().Pt();
  }

  double NeutralParticle_v1::eta() const {
    return genvecP4().Eta(); 
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(NeutralParticle_v1,float,double,phi)


  double NeutralParticle_v1::m() const {
    return 139.570; /// @todo Get value from somewhere. Also, the TrackParticle took the Pion mass - do we really want to do this? We have ParticleHypo?
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( NeutralParticle_v1, float, double, e)


  double NeutralParticle_v1::rapidity() const {
    return genvecP4().Rapidity();
  }

  NeutralParticle_v1::GenVecFourMom_t NeutralParticle_v1::genvecP4() const {
    using namespace std;
    float p = 1/fabs(oneOverP());
    float thetaT = theta();
    float phiT = phi();
    float sinTheta= sin(thetaT);
    float px = p*sinTheta*cos(phiT);
    float py = p*sinTheta*sin(phiT);
    float pz = p*cos(thetaT);
    return GenVecFourMom_t(px, py, pz, m());
  }

  NeutralParticle_v1::FourMom_t NeutralParticle_v1::p4() const {
    using namespace std;
    FourMom_t p4;
    float p = 1/fabs(oneOverP());
    float thetaT = theta();
    float phiT = phi();
    float sinTheta= sin(thetaT);
    float px = p*sinTheta*cos(phiT);
    float py = p*sinTheta*sin(phiT);
    float pz = p*cos(thetaT);
    float e  =  pow (m(),2) + 
      pow( px,2) + pow( py,2) + pow( pz,2);
    p4.SetPxPyPzE( px, py, pz, sqrt(e) ); 
    return p4;
  }

  Type::ObjectType NeutralParticle_v1::type() const {
     return Type::NeutralParticle;
  }


  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, d0)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, z0)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, phi0)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, theta)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, oneOverP)

  const DefiningParameters_t NeutralParticle_v1::definingParameters() const{
      DefiningParameters_t tmp;
      tmp << d0(),z0(),phi0(),theta(),oneOverP();
      return tmp;
  }

  void NeutralParticle_v1::setDefiningParameters(float d0, float z0, float phi0, float theta, float oneOverP) {
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    if(m_perigeeParameters.isValid()) {
      m_perigeeParameters.reset();
    }
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    static const Accessor< float > acc1( "d0" );
    acc1( *this ) = d0;

    static const Accessor< float > acc2( "z0" );
    acc2( *this ) = z0;

    static const Accessor< float > acc3( "phi" );
    acc3( *this ) = phi0;

    static const Accessor< float > acc4( "theta" );
    acc4( *this ) = theta;

    static const Accessor< float > acc5( "oneOverP" );
    acc5( *this ) = oneOverP;

    return;
  }

  void NeutralParticle_v1::setDefiningParametersCovMatrix(const xAOD::ParametersCovMatrix_t& cov){
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
   if(m_perigeeParameters.isValid()) {
     m_perigeeParameters.reset();
   }
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    
    static const Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    std::vector<float>& v = acc(*this);
    v.reserve(15);
    for (size_t irow = 0; irow<5; ++irow)
        for (size_t icol =0; icol<=irow; ++icol)
            v.push_back(cov(icol,irow));
  }

  const xAOD::ParametersCovMatrix_t NeutralParticle_v1::definingParametersCovMatrix() const {
    static const Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    std::vector<float> v = acc(*this);
    std::vector<float>::const_iterator it = v.begin();
    xAOD::ParametersCovMatrix_t cov; 
    cov.setZero();
    for (size_t irow = 0; irow<5; ++irow){
        for (size_t icol =0; icol<=irow; ++icol){
            cov.fillSymmetric(icol,irow, *it++);
        }
    }
    return cov;
  }

  const std::vector<float>& NeutralParticle_v1::definingParametersCovMatrixVec() const {
    static const Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    return acc(*this);
  }

  void NeutralParticle_v1::setDefiningParametersCovMatrixVec(const std::vector<float>& cov){
    static const Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    acc(*this)=cov;
  }

  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, vx)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, vy)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, vz)

  void NeutralParticle_v1::setParametersOrigin(float x, float y, float z){
    static const Accessor< float > acc1( "vx" );
    acc1( *this ) = x;

    static const Accessor< float > acc2( "vy" );
    acc2( *this ) = y;

    static const Accessor< float > acc3( "vz" );
    acc3( *this ) = z;
  }

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
  const Trk::NeutralPerigee& NeutralParticle_v1::perigeeParameters() const {
   
    // Require the cache to be valid and check if the cached pointer has been set
    if(m_perigeeParameters.isValid()){
          return *(m_perigeeParameters.ptr());
    }
    static const Accessor< float > acc1( "d0" );
    static const Accessor< float > acc2( "z0" );
    static const Accessor< float > acc3( "phi" );
    static const Accessor< float > acc4( "theta" );
    static const Accessor< float > acc5( "oneOverP" );
    static const Accessor< std::vector<float> > acc6( "definingParametersCovMatrix" );
    ParametersCovMatrix_t* cov = new ParametersCovMatrix_t;
    cov->setZero();
    auto it= acc6(*this).begin();
    for (size_t irow = 0; irow<5; ++irow){
      for (size_t icol =0; icol<=irow; ++icol){
          cov->fillSymmetric(irow,icol,*it++) ;
      }
    }
    Trk::NeutralPerigee tmpPerigeeParameters(acc1(*this),acc2(*this),acc3(*this),acc4(*this),acc5(*this),
                                             Trk::PerigeeSurface(Amg::Vector3D(vx(),vy(),vz())),cov); 
    m_perigeeParameters.set(tmpPerigeeParameters);
    return *(m_perigeeParameters.ptr());
  }
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

  void NeutralParticle_v1::resetCache(){
     m_perigeeParameters.reset();
   }
} // namespace xAOD

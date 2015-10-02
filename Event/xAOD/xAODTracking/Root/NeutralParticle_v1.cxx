/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NeutralParticle_v1.cxx 573493 2013-12-03 13:05:51Z emoyse $
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
  : IParticle(), m_p4(), m_p4Cached( false ), m_perigeeCached(false) {
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters=0;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE
  }
  
  NeutralParticle_v1::NeutralParticle_v1(const NeutralParticle_v1& tp ) 
  : IParticle( tp ), m_p4(tp.m_p4), m_p4Cached( tp.m_p4Cached ), m_perigeeCached(tp.m_perigeeCached) {
    makePrivateStore( tp );
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters = tp.m_perigeeParameters;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
  }
  
  NeutralParticle_v1& NeutralParticle_v1::operator=(const NeutralParticle_v1& tp ){
    if(this == &tp) return *this;
    
    if(!hasStore() ) makePrivateStore();
    this->IParticle::operator=( tp );
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    m_perigeeParameters = tp.m_perigeeParameters;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    return *this;
  }

  NeutralParticle_v1::~NeutralParticle_v1(){
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    delete m_perigeeParameters;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
  }
  
  double NeutralParticle_v1::pt() const {
    return p4().Pt();
  }

  double NeutralParticle_v1::eta() const {
    return p4().Eta(); 
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(NeutralParticle_v1,float,double,phi)


  double NeutralParticle_v1::m() const {
    return p4().M();
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( NeutralParticle_v1, float, double, e)


  double NeutralParticle_v1::rapidity() const {
    return p4().Rapidity();
  }

  const NeutralParticle_v1::FourMom_t& NeutralParticle_v1::p4() const {
    using namespace std;
  // Check if we need to reset the cached object:
    if( ! m_p4Cached ) {
      float p = 1/fabs(oneOverP());
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

  Type::ObjectType NeutralParticle_v1::type() const {
     return Type::NeutralParticle;
  }


  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, d0)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, z0)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, phi0)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, theta)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, oneOverP)

  const DefiningParameters_t& NeutralParticle_v1::definingParameters() const{
    static DefiningParameters_t tmp;
    tmp << d0(),z0(),phi0(),theta(),oneOverP();
    return tmp;
  }

  void NeutralParticle_v1::setDefiningParameters(float d0, float z0, float phi0, float theta, float oneOverP) {
    m_perigeeCached=false;
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    delete m_perigeeParameters;
    m_perigeeParameters=0;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    static Accessor< float > acc1( "d0" );
    acc1( *this ) = d0;

    static Accessor< float > acc2( "z0" );
    acc2( *this ) = z0;

    static Accessor< float > acc3( "phi" );
    acc3( *this ) = phi0;

    static Accessor< float > acc4( "theta" );
    acc4( *this ) = theta;

    static Accessor< float > acc5( "oneOverP" );
    acc5( *this ) = oneOverP;

    m_p4Cached = false;
    return;
  }

  void NeutralParticle_v1::setDefiningParametersCovMatrix(const xAOD::ParametersCovMatrix_t& cov){
    m_perigeeCached=false;
    #if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
    delete m_perigeeParameters;
    m_perigeeParameters=0;
    #endif // not XAOD_STANDALONE and not XAOD_MANACORE
    
    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    std::vector<float>& v = acc(*this);
    v.reserve(15);
    for (size_t irow = 0; irow<5; ++irow)
        for (size_t icol =0; icol<=irow; ++icol)
            v.push_back(cov(icol,irow));
  }

  const xAOD::ParametersCovMatrix_t& NeutralParticle_v1::definingParametersCovMatrix() const {
    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    std::vector<float> v = acc(*this);
    std::vector<float>::const_iterator it = v.begin();
    static xAOD::ParametersCovMatrix_t cov; cov.setZero();
    for (size_t irow = 0; irow<5; ++irow)
        for (size_t icol =0; icol<=irow; ++icol)
            cov.fillSymmetric(icol,irow, *it++);
    return cov;
  }

  const std::vector<float>& NeutralParticle_v1::definingParametersCovMatrixVec() const {
    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    return acc(*this);
  }

  void NeutralParticle_v1::setDefiningParametersCovMatrixVec(const std::vector<float>& cov){
    static Accessor< std::vector<float> > acc( "definingParametersCovMatrix" );
    acc(*this)=cov;
  }

  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, vx)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, vy)
  AUXSTORE_PRIMITIVE_GETTER(NeutralParticle_v1, float, vz)

  void NeutralParticle_v1::setParametersOrigin(float x, float y, float z){
    static Accessor< float > acc1( "vx" );
    acc1( *this ) = x;

    static Accessor< float > acc2( "vy" );
    acc2( *this ) = y;

    static Accessor< float > acc3( "vz" );
    acc3( *this ) = z;
  }

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
  const Trk::NeutralPerigee& NeutralParticle_v1::perigeeParameters() const {
    
    static Accessor< float > acc1( "d0" );
    static Accessor< float > acc2( "z0" );
    static Accessor< float > acc3( "phi" );
    static Accessor< float > acc4( "theta" );
    static Accessor< float > acc5( "oneOverP" );
    static Accessor< std::vector<float> > acc6( "definingParametersCovMatrix" );
    ParametersCovMatrix_t* cov = new ParametersCovMatrix_t;
    cov->setZero();
    auto it= acc6(*this).begin();
    for (size_t irow = 0; irow<5; ++irow)
      for (size_t icol =0; icol<=irow; ++icol)
          cov->fillSymmetric(irow,icol,*it++) ;
    m_perigeeParameters = new Trk::NeutralPerigee(acc1(*this),acc2(*this),acc3(*this),acc4(*this),acc5(*this),Trk::PerigeeSurface(Amg::Vector3D(vx(),vy(),vz())),cov);    
    return *m_perigeeParameters;
    
  }
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

  // AUXSTORE_OBJECT_SETTER_AND_GETTER(NeutralParticle_v1, ElementLink< VertexContainer >, vertex, setVertex)

} // namespace xAOD

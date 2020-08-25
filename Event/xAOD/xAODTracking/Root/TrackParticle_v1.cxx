/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Misc includes
#include <bitset>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <atomic>
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/TrackParticle_v1.h"
#include "xAODTracking/TrackSummaryAccessors_v1.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


namespace xAODTrackParticlePrivate {

  /// Function that would be possible to use to debug what client is trying
  /// to access covariance matrix from an @c xAOD::TrackParticle object, before they
  /// it has been set.
  void covarianceUnsetHook() {
    static std::atomic< bool > uninitCovarianceAccessPrinted = false;
    if( ! uninitCovarianceAccessPrinted ) {
      std::cout << "xAOD::TrackParticle WARNING Uninitialised covariance matrix was "
	"accessed.\n"
	"                        Debug it by breaking on "
	"xAODTrackParticlePrivate::covarianceUnsetHook function calls!"
		<< std::endl;
      uninitCovarianceAccessPrinted = true;
    }
    return;
  }

}


namespace xAOD {

  TrackParticle_v1::TrackParticle_v1()
  : IParticle() {
    // perigeeParameters cache initialized to be empty (default constructor)
  }

  TrackParticle_v1::TrackParticle_v1(const TrackParticle_v1& tp )
  : IParticle( tp ) {
    makePrivateStore( tp );
    // perigeeParameters cache initialized to be empty (default constructor)
    // assume that this copy will create new cache as needed
  }

  TrackParticle_v1& TrackParticle_v1::operator=(const TrackParticle_v1& tp ){
    if(this == &tp) return *this;

    if( ( ! hasStore() ) && ( ! container() ) ) {
       makePrivateStore();
    }
    this->IParticle::operator=( tp );
#ifndef XAOD_STANDALONE
    // assume that this copy will create new cache as needed
    m_perigeeParameters.reset();
#endif // not XAOD_STANDALONE
    return *this;
  }

  TrackParticle_v1::~TrackParticle_v1(){}

  double TrackParticle_v1::pt() const {
    return genvecP4().Pt();
  }

  double TrackParticle_v1::eta() const {
    return genvecP4().Eta();
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1,float,double,phi)

  double TrackParticle_v1::m() const {
    return 139.570; /// @todo Get value from somewhere. Also, the TrackParticle took the Pion mass - do we really want to do this? We have ParticleHypo?
  }

  double TrackParticle_v1::e() const {
    return genvecP4().E();
  }
  double TrackParticle_v1::rapidity() const {
    return genvecP4().Rapidity();
  }

  TrackParticle_v1::GenVecFourMom_t TrackParticle_v1::genvecP4() const {
    using namespace std;
    float p = 10.e6; // 10 TeV (default value for very high pt muons, with qOverP==0)
    if (fabs(qOverP())>0.) p = 1/fabs(qOverP());
    float thetaT = theta();
    float phiT = phi();
    float sinTheta= sin(thetaT);
    float px = p*sinTheta*cos(phiT);
    float py = p*sinTheta*sin(phiT);
    float pz = p*cos(thetaT);
    return GenVecFourMom_t(px, py, pz, m());
  }

  TrackParticle_v1::FourMom_t TrackParticle_v1::p4() const {
    TrackParticle_v1::FourMom_t p4;
    using namespace std;
    float p = 10.e6; // 10 TeV (default value for very high pt muons, with qOverP==0)
    if (fabs(qOverP())>0.) p = 1/fabs(qOverP());
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

  Type::ObjectType TrackParticle_v1::type() const {
     return Type::TrackParticle;
  }

  float TrackParticle_v1::charge() const {
    // static Accessor< float > acc( "charge" );
    return (qOverP() > 0) ? 1 : ((qOverP() < 0) ? -1 : 0);
  }

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, d0)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, z0)

  float TrackParticle_v1::phi0() const {

     static  const Accessor< float > acc( "phi" );
     return acc( *this );
  }

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, theta)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, qOverP)

  DefiningParameters_t TrackParticle_v1::definingParameters() const{
    DefiningParameters_t tmp;
    tmp << d0() , z0() , phi0() , theta() , qOverP();
    return tmp;
  }

  void TrackParticle_v1::setDefiningParameters(float d0, float z0, float phi0, float theta, float qOverP) {
#ifndef XAOD_STANDALONE
    // reset perigee cache if existing
    if(m_perigeeParameters.isValid()) {
      m_perigeeParameters.reset();
    }
#endif // not XAOD_STANDALONE
    static const Accessor< float > acc1( "d0" );
    acc1( *this ) = d0;

    static const Accessor< float > acc2( "z0" );
    acc2( *this ) = z0;

    static const Accessor< float > acc3( "phi" );
    acc3( *this ) = phi0;

    static const Accessor< float > acc4( "theta" );
    acc4( *this ) = theta;

    static const Accessor< float > acc5( "qOverP" );
    acc5( *this ) = qOverP;

    return;
  }

  static const SG::AuxElement::Accessor< std::vector< float > >
    accCovMatrixDiag( "definingParametersCovMatrixDiag" );
  static const SG::AuxElement::Accessor< std::vector< float > >
    accCovMatrixOffDiag( "definingParametersCovMatrixOffDiag" );

  void TrackParticle_v1::setDefiningParametersCovMatrix(const xAOD::ParametersCovMatrix_t& cov){

#ifndef XAOD_STANDALONE
    // reset perigee cache if existing
    if(m_perigeeParameters.isValid()) {
      m_perigeeParameters.reset();
    }
#endif // not XAOD_STANDALONE

    // Extract the diagonal elements from the matrix.
    std::vector< float > diagVec;
    diagVec.reserve( cov.rows() );
    for( int i = 0; i < cov.rows(); ++i ) {
      diagVec.push_back( cov( i, i ) );
    }
    // Set the variable.
    setDefiningParametersCovMatrixDiagVec( diagVec );

    // Extract the off-diagonal elements from the matrix.
    std::vector< float > offDiagVec;
    offDiagVec.reserve( ( ( cov.rows() - 1 ) * cov.rows() ) / 2 );
    for( int i = 1; i < cov.rows(); ++i ) {
      for( int j = 0; j < i; ++j ) {
        float offDiagCoeff = (cov( i, i )>0 && cov( j, j )>0) ? cov( i, j )/sqrt(cov( i, i )*cov( j, j )) : 0;
        offDiagVec.push_back( offDiagCoeff );
      }
    }
    // Set the variable.
    setDefiningParametersCovMatrixOffDiagVec( offDiagVec );

    return;
  }

  const xAOD::ParametersCovMatrix_t TrackParticle_v1::definingParametersCovMatrix() const {

        // Set up the result matrix.
    xAOD::ParametersCovMatrix_t cov;
    cov.setZero();

    // Set the diagonal elements of the matrix.
    if( accCovMatrixDiag.isAvailable( *this ) &&
        ( static_cast< int >( accCovMatrixDiag( *this ).size() ) == cov.rows() ) ) {

        // Access the "raw" variable.
        const std::vector< float >& diagVec = accCovMatrixDiag( *this );
        // Set the diagonal elements using the raw variable.
        for( int i = 0; i < cov.rows(); ++i ) {
          cov( i, i ) = diagVec[ i ];
        }
    } else {
      xAODTrackParticlePrivate::covarianceUnsetHook();
      // If the variable is not available/set, set the matrix to identity.
      cov.setIdentity();
    }

    bool offDiagCompr = definingParametersCovMatrixOffDiagCompr();

    // Set the off-diagonal elements of the matrix.
    if(!offDiagCompr){

      if( accCovMatrixOffDiag.isAvailable( *this ) &&
	  ( static_cast< int >( accCovMatrixOffDiag( *this ).size() ) ==
	    ( ( ( cov.rows() - 1 ) * cov.rows() ) / 2 ) ) ) {

	// Access the "raw" variable.
	const std::vector< float >& offDiagVec = accCovMatrixOffDiag( *this );
	// Set the off-diagonal elements using the raw variable.
	std::size_t vecIndex = 0;
	for( int i = 1; i < cov.rows(); ++i ) {
	  for( int j = 0; j < i; ++j, ++vecIndex ) {
	    cov.fillSymmetric( i, j, offDiagVec[ vecIndex ]*sqrt(cov(i,i)*cov(j,j)) );
	  }
	}
      }

      else xAODTrackParticlePrivate::covarianceUnsetHook();

    }

    else{ //Compressed case

      if( accCovMatrixOffDiag.isAvailable( *this ) &&
	  ( static_cast< int >( accCovMatrixOffDiag( *this ).size() ) == COVMATRIX_OFFDIAG_VEC_COMPR_SIZE ) ) {
	// Access the "raw" variable.
	const std::vector< float >& offDiagVec = accCovMatrixOffDiag( *this );
	// Set the off-diagonal elements using the raw variable.
	cov.fillSymmetric( d0_index, phi_index,
			   offDiagVec[d0_phi_index]*sqrt(cov(d0_index,d0_index)*cov(phi_index,phi_index)) );
	cov.fillSymmetric( z0_index, th_index,
			   offDiagVec[z0_th_index]*sqrt(cov(z0_index,z0_index)*cov(th_index,th_index)) );
	cov.fillSymmetric( d0_index, qp_index,
			   offDiagVec[d0_qp_index]*sqrt(cov(d0_index,d0_index)*cov(qp_index,qp_index)) );
	cov.fillSymmetric( z0_index, qp_index,
			   offDiagVec[z0_qp_index]*sqrt(cov(z0_index,z0_index)*cov(qp_index,qp_index)) );
	cov.fillSymmetric( phi_index, qp_index,
			   offDiagVec[phi_qp_index]*sqrt(cov(phi_index,phi_index)*cov(qp_index,qp_index)) );
	cov.fillSymmetric( th_index, qp_index,
			   offDiagVec[th_qp_index]*sqrt(cov(th_index,th_index)*cov(qp_index,qp_index)) );

      }

      else xAODTrackParticlePrivate::covarianceUnsetHook();

    }


    // Return the filled matrix.
    return cov;

  }

  ParametersCovMatrixFilled_t TrackParticle_v1::definingParametersCovMatrixFilled() const {

    // Create the result matrix.
    ParametersCovMatrixFilled_t result;
    result.setZero();

    // Check if the diagonal elements are available.
    if( accCovMatrixDiag.isAvailable( *this ) &&
        ( static_cast< int >( accCovMatrixDiag( *this ).size() ) == result.rows() ) ) {

      result.setIdentity();
    }

    bool offDiagCompr = definingParametersCovMatrixOffDiagCompr();

    if(!offDiagCompr){

      // Check if the off-diagonal elements are available.
      if( accCovMatrixOffDiag.isAvailable( *this ) &&
	  ( static_cast< int >( accCovMatrixOffDiag( *this ).size() ) ==
	    ( ( result.rows() * ( result.rows() - 1 ) ) / 2 ) ) ) {

	for( int i = 1; i < result.rows(); ++i ) {
	  for( int j = 0; j < i; ++j ) {
	    result.fillSymmetric( i, j, true );
	  }
	}
      }

    }

    else{

      if( accCovMatrixOffDiag.isAvailable( *this ) &&
	  ( static_cast< int >( accCovMatrixOffDiag( *this ).size() ) == COVMATRIX_OFFDIAG_VEC_COMPR_SIZE ) ){

	result.fillSymmetric( d0_index, phi_index, true );
	result.fillSymmetric( z0_index, th_index, true );
	result.fillSymmetric( d0_index, qp_index, true );
	result.fillSymmetric( z0_index, qp_index, true );
	result.fillSymmetric( phi_index, qp_index, true );
	result.fillSymmetric( th_index, qp_index, true );

      }

    }

    // Return the object.
    return result;
  }

  const std::vector< float >& TrackParticle_v1::definingParametersCovMatrixDiagVec() const {

    return accCovMatrixDiag( *this );
  }

  const std::vector< float >& TrackParticle_v1::definingParametersCovMatrixOffDiagVec() const {

    return accCovMatrixOffDiag( *this );
  }

  std::vector<float> TrackParticle_v1::definingParametersCovMatrixVec() const {

    std::vector< float > vec;
    AmgSymMatrix(5)* cov = new AmgSymMatrix(5)(definingParametersCovMatrix());
    Amg::compress(*cov,vec);
    return vec;

  }

  void TrackParticle_v1::setDefiningParametersCovMatrixDiagVec( const std::vector< float >& vec ) {

    if( vec.size() != ParametersCovMatrix_t::RowsAtCompileTime ){
      throw std::runtime_error("Setting track definingParametersCovMatrixDiag with vector of size "+std::to_string(vec.size())+" instead of expected "+std::to_string(ParametersCovMatrix_t::RowsAtCompileTime)+" is not supported");
    }

    accCovMatrixDiag( *this ) = vec;
    return;
  }

  void TrackParticle_v1::setDefiningParametersCovMatrixOffDiagVec( const std::vector< float >& vec ) {

    bool offDiagCompr = definingParametersCovMatrixOffDiagCompr();

    unsigned int uncompr_size = ( ( ( ParametersCovMatrix_t::RowsAtCompileTime - 1 ) *
				    ParametersCovMatrix_t::RowsAtCompileTime ) / 2 );
    unsigned int size = offDiagCompr ? COVMATRIX_OFFDIAG_VEC_COMPR_SIZE : uncompr_size;

    if( !(vec.size() == size || vec.size() == uncompr_size) ){ //If off-diagonal elements are already compressed, can either set with uncompressed or compressed vector
      throw std::runtime_error("Setting track definingParametersCovMatrixOffDiag with vector of size "+std::to_string(vec.size())+" instead of expected "+std::to_string(size)+" or "+std::to_string(uncompr_size)+" is not supported");
    }

    accCovMatrixOffDiag( *this ) = vec;
    return;
  }

  bool TrackParticle_v1::definingParametersCovMatrixOffDiagCompr() const {

    bool flag = false;
    if(accCovMatrixOffDiag.isAvailable( *this )) flag = (static_cast< int >(accCovMatrixOffDiag( *this ).size())==COVMATRIX_OFFDIAG_VEC_COMPR_SIZE);
    return flag;
  }

  void TrackParticle_v1::compressDefiningParametersCovMatrixOffDiag() {

    ParametersCovMatrix_t cov = definingParametersCovMatrix();
    std::vector< float > offDiagVecCompr;
    offDiagVecCompr.resize(COVMATRIX_OFFDIAG_VEC_COMPR_SIZE);

    offDiagVecCompr[d0_phi_index] = cov(d0_index, phi_index) / sqrt(cov(d0_index,d0_index)*cov(phi_index,phi_index));
    offDiagVecCompr[z0_th_index] = cov(z0_index, th_index) / sqrt(cov(z0_index,z0_index)*cov(th_index,th_index));
    offDiagVecCompr[d0_qp_index] = cov(d0_index, qp_index) / sqrt(cov(d0_index,d0_index)*cov(qp_index,qp_index));
    offDiagVecCompr[z0_qp_index] = cov(z0_index, qp_index) / sqrt(cov(z0_index,z0_index)*cov(qp_index,qp_index));
    offDiagVecCompr[phi_qp_index] = cov(phi_index, qp_index) / sqrt(cov(phi_index,phi_index)*cov(qp_index,qp_index));
    offDiagVecCompr[th_qp_index] = cov(th_index, qp_index) / sqrt(cov(th_index,th_index)*cov(qp_index,qp_index));

    accCovMatrixOffDiag( *this ) = offDiagVecCompr;
    return;

  }


  //Old schema compatibility

  void TrackParticle_v1::setDefiningParametersCovMatrixVec(const std::vector<float>& cov){

    xAOD::ParametersCovMatrix_t covMatrix;
    if( !cov.empty() ) Amg::expand( cov.begin(), cov.end(),covMatrix );
    else covMatrix.setIdentity();
    setDefiningParametersCovMatrix( covMatrix );

  }

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, vx)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, vy)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, vz)

  void TrackParticle_v1::setParametersOrigin(float x, float y, float z){
    static const Accessor< float > acc1( "vx" );
    acc1( *this ) = x;

    static const Accessor< float > acc2( "vy" );
    acc2( *this ) = y;

    static const Accessor< float > acc3( "vz" );
    acc3( *this ) = z;
  }

#ifndef XAOD_STANDALONE
  const Trk::Perigee& TrackParticle_v1::perigeeParameters() const {

    // Require the cache to be valid and check if the cached pointer has been set
    if(m_perigeeParameters.isValid()){
      return *(m_perigeeParameters.ptr());
    }
    static const Accessor< float > acc1( "d0" );
    static const Accessor< float > acc2( "z0" );
    static const Accessor< float > acc3( "phi" );
    static const Accessor< float > acc4( "theta" );
    static const Accessor< float > acc5( "qOverP" );
    static const Accessor< std::vector<float> > acc6( "definingParametersCovMatrix" );
    ParametersCovMatrix_t* cov = new ParametersCovMatrix_t(definingParametersCovMatrix());
    static const Accessor< float > acc7( "beamlineTiltX" );
    static const Accessor< float > acc8( "beamlineTiltY" );

    if(!acc7.isAvailable( *this ) || !acc8.isAvailable( *this )){
      Trk::Perigee tmpPerigeeParameters(acc1(*this),acc2(*this),acc3(*this),acc4(*this),acc5(*this),Trk::PerigeeSurface(Amg::Vector3D(vx(),vy(),vz())),cov);
      m_perigeeParameters.set(tmpPerigeeParameters);
      return *(m_perigeeParameters.ptr());
    }

    Amg::Transform3D * amgTransf = new Amg::Transform3D();
    Amg::Translation3D amgtranslation(vx(),vy(),vz());
    *amgTransf = amgtranslation * Amg::RotationMatrix3D::Identity();
    *amgTransf *= Amg::AngleAxis3D(acc8(*this), Amg::Vector3D(0.,1.,0.));
    *amgTransf *= Amg::AngleAxis3D(acc7(*this), Amg::Vector3D(1.,0.,0.));
    Trk::Perigee tmpPerigeeParameters(acc1(*this),acc2(*this),acc3(*this),acc4(*this),acc5(*this),Trk::PerigeeSurface(amgTransf),cov);

    m_perigeeParameters.set(tmpPerigeeParameters);
    return *(m_perigeeParameters.ptr());
  }
#endif // not XAOD_STANDALONE

  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, chiSquared)
  AUXSTORE_PRIMITIVE_GETTER(TrackParticle_v1, float, numberDoF)

  void TrackParticle_v1::setFitQuality(float chiSquared, float numberDoF){
    static const Accessor< float > acc1( "chiSquared" );
    acc1( *this ) = chiSquared;
    static const Accessor< float > acc2( "numberDoF" );
    acc2( *this ) = numberDoF;
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, float, radiusOfFirstHit, setRadiusOfFirstHit)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint64_t, identifierOfFirstHit, setIdentifierOfFirstHit)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, float, beamlineTiltX, setBeamlineTiltX)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, float, beamlineTiltY, setBeamlineTiltY)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint32_t, hitPattern, setHitPattern)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint8_t,numberOfUsedHitsdEdx ,setNumberOfUsedHitsdEdx )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackParticle_v1, uint8_t,numberOfIBLOverflowsdEdx , setNumberOfIBLOverflowsdEdx)

  size_t TrackParticle_v1::numberOfParameters() const{
    ///@todo - Can we do this in a better way? Not great to force retrieval of one specific parameter - any would do.
    static const Accessor< std::vector<float>  > acc( "parameterX" );
    if(! acc.isAvailable( *this )) return 0;
    return acc(*this).size();
  }

  const CurvilinearParameters_t TrackParticle_v1::trackParameters(unsigned int index) const{
    CurvilinearParameters_t tmp;
    tmp << parameterX(index),parameterY(index),parameterZ(index),
      parameterPX(index),parameterPY(index),parameterPZ(index);
    return tmp;
  }

  void TrackParticle_v1::setTrackParameters(std::vector<std::vector<float> >& parameters){
    static const Accessor< std::vector < float > > acc1( "parameterX" );
    static const Accessor< std::vector < float > > acc2( "parameterY" );
    static const Accessor< std::vector < float > > acc3( "parameterZ" );
    static const Accessor< std::vector < float > > acc4( "parameterPX" );
    static const Accessor< std::vector < float > > acc5( "parameterPY" );
    static const Accessor< std::vector < float > > acc6( "parameterPZ" );

    static const Accessor< std::vector<uint8_t>  > acc7( "parameterPosition" );

    acc1(*this).resize(parameters.size());
    acc2(*this).resize(parameters.size());
    acc3(*this).resize(parameters.size());
    acc4(*this).resize(parameters.size());
    acc5(*this).resize(parameters.size());
    acc6(*this).resize(parameters.size());

    acc7(*this).resize(parameters.size());

    unsigned int index=0;
    std::vector<std::vector<float> >::const_iterator it=parameters.begin(), itEnd=parameters.end();
    for (;it!=itEnd;++it,++index){
      assert((*it).size()==6);
      acc1(*this).at(index)=(*it).at(0);
      acc2(*this).at(index)=(*it).at(1);
      acc3(*this).at(index)=(*it).at(2);
      acc4(*this).at(index)=(*it).at(3);
      acc5(*this).at(index)=(*it).at(4);
      acc6(*this).at(index)=(*it).at(5);
    }
  }

  float TrackParticle_v1::parameterX(unsigned int index) const  {
    static const Accessor< std::vector<float>  > acc( "parameterX" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterY(unsigned int index) const  {
    static const Accessor< std::vector<float>  > acc( "parameterY" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterZ(unsigned int index) const  {
    static const Accessor< std::vector<float>  > acc( "parameterZ" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterPX(unsigned int index) const {
    static const Accessor< std::vector<float>  > acc( "parameterPX" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterPY(unsigned int index) const {
    static const Accessor< std::vector<float>  > acc( "parameterPY" );
    return acc(*this).at(index);
  }

  float TrackParticle_v1::parameterPZ(unsigned int index) const {
    static const Accessor< std::vector<float>  > acc( "parameterPZ" );
    return acc(*this).at(index);
  }

  xAOD::ParametersCovMatrix_t TrackParticle_v1::trackParameterCovarianceMatrix(unsigned int index) const
  {
    static const Accessor< std::vector<float>  > acc( "trackParameterCovarianceMatrices" );
    unsigned int offset = index*15;
    // copy the correct values into the temp matrix
    xAOD::ParametersCovMatrix_t tmp;
    std::vector<float>::const_iterator it = acc(*this).begin()+offset;
    Amg::expand(it,it+15,tmp);
    return tmp;
  }

  void TrackParticle_v1::setTrackParameterCovarianceMatrix(unsigned int index, std::vector<float>& cov){
    assert(cov.size()==15);
    unsigned int offset = index*15;
    static const Accessor< std::vector < float > > acc( "trackParameterCovarianceMatrices" );
    std::vector<float>& v = acc(*this);
    v.resize(offset+15);
    std::copy(cov.begin(),cov.end(),v.begin()+offset );
  }

  xAOD::ParameterPosition TrackParticle_v1::parameterPosition(unsigned int index) const
  {
    static const Accessor< std::vector<uint8_t>  > acc( "parameterPosition" );
    return static_cast<xAOD::ParameterPosition>(acc(*this).at(index));
  }

  bool TrackParticle_v1::indexOfParameterAtPosition(unsigned int& index, ParameterPosition position) const
  {
    size_t maxParameters = numberOfParameters();
    bool foundParameters=false;
    for (size_t i=0; i<maxParameters; ++i){
      if (parameterPosition(i)==position){
        foundParameters=true;
        index=i;
        break;
      }
    }
    return foundParameters;
  }

  void  TrackParticle_v1::setParameterPosition(unsigned int index, xAOD::ParameterPosition pos){
    static const Accessor< std::vector<uint8_t>  > acc( "parameterPosition" );
    acc( *this ).at(index) = static_cast<uint8_t>(pos);
  }

#ifndef XAOD_STANDALONE
  const Trk::CurvilinearParameters TrackParticle_v1::curvilinearParameters(unsigned int index) const {    

    static const Accessor< std::vector<float>  > acc( "trackParameterCovarianceMatrices" );
    unsigned int offset = index*15;
    // copy the correct values into the temp matrix
    ParametersCovMatrix_t* cov = new ParametersCovMatrix_t();
    auto it = acc(*this).begin()+offset;
    Amg::expand(it,it+15,*cov);
    // retrieve the parameters to build the curvilinear frame
    Amg::Vector3D pos(parameterX(index),parameterY(index),parameterZ(index));
    Amg::Vector3D mom(parameterPX(index),parameterPY(index),parameterPZ(index));
    Trk::CurvilinearParameters param(pos,mom,charge(),cov);

    return param;
  }
#endif // not XAOD_STANDALONE

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackProperties,trackProperties)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackProperties,trackProperties, setTrackProperties)

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackFitter,trackFitter)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::TrackFitter,trackFitter, setTrackFitter)

  std::bitset<xAOD::NumberOfTrackRecoInfo>   TrackParticle_v1::patternRecoInfo() const {
    static const Accessor< uint64_t > acc( "patternRecoInfo" );
    std::bitset<xAOD::NumberOfTrackRecoInfo> tmp(acc(*this));
    return tmp;
  }

  void TrackParticle_v1::setPatternRecognitionInfo(uint64_t patternReco)  {
    static const Accessor< uint64_t > acc( "patternRecoInfo" );
    acc( *this ) = patternReco;
  }

  void TrackParticle_v1::setPatternRecognitionInfo(const std::bitset<xAOD::NumberOfTrackRecoInfo>& patternReco)  {
    static const Accessor< uint64_t > acc( "patternRecoInfo" );
    acc( *this ) = patternReco.to_ullong();
  }

  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::ParticleHypothesis, particleHypothesis, setParticleHypothesis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackParticle_v1, uint8_t, xAOD::ParticleHypothesis, particleHypothesis)

  bool TrackParticle_v1::summaryValue(uint8_t& value, const SummaryType &information)  const {
    const xAOD::TrackParticle_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information );
    if( ( ! acc ) || ( ! acc->isAvailable( *this ) ) ) return false;
  // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  bool TrackParticle_v1::summaryValue(float& value, const SummaryType &information)  const {
    const xAOD::TrackParticle_v1::Accessor< float >* acc = trackSummaryAccessorV1<float>( information );
    if( ( ! acc ) || ( ! acc->isAvailable( *this ) ) ) return false;
  // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  void TrackParticle_v1::setSummaryValue(uint8_t& value, const SummaryType &information){
    const xAOD::TrackParticle_v1::Accessor< uint8_t >* acc = trackSummaryAccessorV1<uint8_t>( information );
  // Set the value:
    ( *acc )( *this ) = value;
  }

  void TrackParticle_v1::setSummaryValue(float& value, const SummaryType &information){
    const xAOD::TrackParticle_v1::Accessor< float >* acc = trackSummaryAccessorV1<float>( information );
  // Set the value:
    ( *acc )( *this ) = value;
  }


#ifndef XAOD_STANDALONE
   /// The function will return an invalid ElementLink in case nothing was set
   /// for it yet. This is to avoid users having to always check both for
   /// the decoration being available, and the link being valid.
   ///
   /// @returns An element link to the parent Trk::Track of this track particle
   ///
   const ElementLink< TrackCollection >& TrackParticle_v1::trackLink() const {

      // The accessor:
      static const ConstAccessor< ElementLink< TrackCollection > > acc( "trackLink" );

      // Check if one of them is available:
      if( acc.isAvailable( *this ) ) {
         return acc( *this );
      }

      // If no Trk::Track link was not set (yet), return a dummy object:
      static const ElementLink< TrackCollection > dummy;
      return dummy;
   }

   void TrackParticle_v1::
   setTrackLink( const ElementLink< TrackCollection >& el ) {

      // The accessor:
      static const Accessor< ElementLink< TrackCollection > > acc( "trackLink" );

      // Do the deed:
      acc( *this ) = el;
      return;
   }

   const Trk::Track* TrackParticle_v1::track() const{

      // The accessor:
      static const ConstAccessor< ElementLink< TrackCollection > > acc( "trackLink" );

      if( ! acc.isAvailable( *this ) ) {
         return nullptr;
      }
      if( ! acc( *this ).isValid() ) {
         return nullptr;
      }

      return *( acc( *this ) );
   } 
#endif // not XAOD_STANDALONE
   
   void TrackParticle_v1::resetCache(){
#ifndef XAOD_STANDALONE
     m_perigeeParameters.reset();
#endif // not XAOD_STANDALONE
   }

} // namespace xAOD

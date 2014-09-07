/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Egamma_v1$

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODEgamma/versions/Egamma_v1.h"
#include "EgammaAccessors_v1.h"

namespace xAOD {

  Egamma_v1::Egamma_v1()
    : IParticle(), m_p4(), m_p4Cached( false ) {
  }
  
  Egamma_v1::Egamma_v1(const Egamma_v1& eg) : IParticle(eg), m_p4(eg.m_p4), m_p4Cached(eg.m_p4Cached) {
  }

 Egamma_v1& Egamma_v1::operator=(const Egamma_v1& eg ){

    if (this != &eg){ // protect against invalid self-assignment      

      if (!this->container() && !this->hasStore() ) {      
	  makePrivateStore();
      }
      this->IParticle::operator=( eg );
      this->m_p4 = eg.m_p4;
      this->m_p4Cached = eg.m_p4Cached;
    }
    // by convention, always return *this
    return *this;
 }


  double Egamma_v1::pt() const {
    static Accessor< float > acc( "pt" );
    return acc( *this );
  }
  
  double Egamma_v1::eta() const {
    static Accessor<float > acc( "eta" );
    return acc( *this );
  }
  
  double Egamma_v1::phi() const {
    static Accessor< float > acc( "phi" );
    return acc( *this );
  }
  
  double Egamma_v1::m() const {
    static Accessor< float> acc( "m" );
    return acc( *this );
  }
  
  double Egamma_v1::e() const{
    return p4().E(); 
  }
  
  double Egamma_v1::rapidity() const {
    return p4().Rapidity();	
  }
  
  const Egamma_v1::FourMom_t& Egamma_v1::p4() const {
    if( ! m_p4Cached ) {
      m_p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
      m_p4Cached=true;
    }
    return m_p4;	
  }
  
  void Egamma_v1::setP4(float pt, float eta, float phi, float m){
    static Accessor< float > acc1( "pt" );
    acc1( *this ) = pt;
    static Accessor< float > acc2( "eta" );
    acc2( *this ) = eta;
    static Accessor< float > acc3( "phi" );
    acc3( *this ) = phi;
    static Accessor< float > acc4( "m" );
    acc4( *this ) = m;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }
  
  void Egamma_v1::setPt(float pt){
    static Accessor< float > acc( "pt" );
    acc( *this ) = pt;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }
  
  void Egamma_v1::setEta(float eta){
    static Accessor< float > acc( "eta" );
    acc( *this ) = eta;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }

  void Egamma_v1::setPhi(float phi){
    static Accessor< float > acc( "phi" );
    acc( *this ) = phi;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }

  void Egamma_v1::setM(float m){
    static Accessor< float > acc( "m" );
    acc( *this ) = m;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }

  Egamma_v1::EgammaCovMatrix_t Egamma_v1::covMatrix() const{
    static Accessor< std::vector<float> > acc( "EgammaCovarianceMatrix" );
    const std::vector<float>& v = acc(*this);
    return Eigen::Map<const EgammaCovMatrix_t> (v.data());
  }

  void Egamma_v1::setCovMatrix(const Egamma_v1::EgammaCovMatrix_t& cov){
    //The internal storage is an std::vector
    static Accessor< std::vector < float > > acc( "EgammaCovarianceMatrix" );
    const std::vector<float> v(cov.data(),cov.data()+16);
    acc(*this)=v;
  }
  
  uint16_t Egamma_v1::author(uint16_t mask) const {
    static Accessor< uint16_t > acc( "author" );
    uint16_t author = acc( *this );
    return author & mask;
  }
  
  void Egamma_v1::addAuthor(uint16_t newAuthor) {
    static Accessor< uint16_t > acc( "author" );
    uint16_t author = acc( *this );
    acc( *this) = author | newAuthor;
  }
  
  void Egamma_v1::setAuthor(uint16_t newAuthor) {
    static Accessor< uint16_t > acc( "author" );
    acc( *this) = newAuthor;
  }
  

  
  ///////////////
  bool Egamma_v1::showerShapeValue(float& value, const EgammaParameters::ShowerShapeType information)  const {
    xAOD::Egamma_v1::Accessor< float >* acc = showerShapeAccessorV1( information );
    if( ! acc ) {
      return false;
    }
    if(! acc->isAvailable( *this) ) { 
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  bool Egamma_v1::setShowerShapeValue(float& value, const EgammaParameters::ShowerShapeType information){
    xAOD::Egamma_v1::Accessor< float >* acc = showerShapeAccessorV1( information );
     if( ! acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;

  }
   
  
  ///////////////
  bool  Egamma_v1::isolationValue(float& value, const EgammaParameters::IsolationType information)  const {

    xAOD::Egamma_v1::Accessor< float >* acc = isolationAccessorV1( information );
    if( ! acc ) {
      return false;
    }
    if(! acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  bool Egamma_v1::setIsolationValue(float& value, const EgammaParameters::IsolationType information) {

     xAOD::Egamma_v1::Accessor< float >* acc = isolationAccessorV1( information );
     if( ! acc ) return false;
     // Set the value:
     ( *acc )( *this ) = value;
     return true;

  }


  ////////////////////////////////////////////////////
  /// Implementation of the CaloCluster accessor functions

  size_t Egamma_v1::nCaloClusters() const {

     static SG::AuxElement::Accessor< Egamma_v1::CLELVec_t >
        clusterAcc( "caloClusterLinks" );

     if( clusterAcc.isAvailable( *this ) ) {
        return clusterAcc( *this ).size();
     }
     return 0;
  }

  const xAOD::CaloCluster* Egamma_v1::caloCluster( size_t index ) const {
    
     if( index >= nCaloClusters() ) {
        return 0;
     }
     const CLELVec_t& cls = caloClusterLinks();
     if( ! cls[ index ].isValid() ) {
        return 0;
     }
     return *( cls[ index ] );
  }

  const ElementLink< CaloClusterContainer >&
  Egamma_v1::caloClusterLink( size_t index ) const {

     if( index >= nCaloClusters() ) {
        static const ElementLink< CaloClusterContainer > dummy;
        return dummy;
     }
     return caloClusterLinks()[ index ];
  }

  AUXSTORE_OBJECT_SETTER_AND_GETTER( Egamma_v1, Egamma_v1::CLELVec_t,
                                     caloClusterLinks, setCaloClusterLinks )

  ///////////

  bool Egamma_v1::isGoodOQ(uint32_t mask) const {
    static Accessor< uint32_t > acc( "OQ" );
    uint32_t OQ = acc( *this );
    return OQ & mask;
  }
  
  void Egamma_v1::setOQ(uint32_t newOQ) {
    static Accessor< uint32_t > acc( "OQ" );
    acc( *this) = newOQ;
  }

  ///////////
  bool Egamma_v1::passSelection(bool&  value, const std::string& menu ) const {
    SG::AuxElement::Accessor< char > acc( menu );
    if(! acc.isAvailable( *this) ) { return  false;}
    value= (acc)( *this );
    return true;
  }

  void Egamma_v1::setPassSelection(bool value, const std::string& menu){
    SG::AuxElement::Accessor< char > acc( menu );
    ( acc )( *this )=value;
    return;
  }
  

} // namespace xAOD

//  LocalWords:  const el hasStore makePrivateStore

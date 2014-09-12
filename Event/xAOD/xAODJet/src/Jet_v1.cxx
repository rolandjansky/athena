/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Jet_v1.cxx 582440 2014-02-10 15:54:30Z delsart $

// EDM include(s):
#include "AthLinks/ElementLinkVector.h"

// Local include(s):
#include "xAODJet/versions/Jet_v1.h"

namespace xAOD {

  Jet_v1::Jet_v1()
    : IParticle() 
    , m_pseudoJet(NULL) 
  {
    makePrivateStore();
  }

  static SG::AuxElement::Accessor< JetFourMom_t > momentum( "momentum" );

   Jet_v1::Jet_v1( float pt, float eta, float phi, float m )
     : IParticle() 
     , m_pseudoJet(NULL)
   {
     makePrivateStore();
     momentum( *this ).SetCoordinates( pt, eta, phi, m );
   }

   double Jet_v1::pt() const {

      return momentum( *this ).Pt();
   }

   double Jet_v1::eta() const {

      return momentum( *this ).Eta();
   }

   double Jet_v1::phi() const {

      return momentum( *this ).Phi();
   }

   double Jet_v1::m() const {

      return momentum( *this ).M();
   }

   double Jet_v1::e() const {

      return momentum( *this ).E();
   }

   double Jet_v1::rapidity() const {

      return momentum( *this ).Rapidity();
   }

   const Jet_v1::FourMom_t& Jet_v1::p4() const {
     if(m_pxpypzeCached) return m_pxpypze;
     m_pxpypze.SetPxPyPzE(px(), py(), pz(), e() );
     m_pxpypzeCached = true;
     return m_pxpypze;
   }

   const JetFourMom_t& Jet_v1::jetP4() const {
     return momentum( *this );
   }

  Type::ObjectType Jet_v1::type() const {    
      return Type::Jet;
   }

   float Jet_v1::px() const {

      return momentum( *this ).Px();
   }

   float Jet_v1::py() const {

      return momentum( *this ).Py();     
   }

   float Jet_v1::pz() const {

      return momentum( *this ).Pz();
   }

   // void Jet_v1::setPt( float pt ) {

   //    momentum( *this ).SetPt(pt);
   //    return;
   // }

   // void Jet_v1::setEta( float eta ) {

   //    momentum( *this ).SetEta( eta );
   //    return;
   // }

   // void Jet_v1::setPhi( float phi ) {

   //    momentum( *this ).SetPhi( phi );
   //    return;
   // }

   // void Jet_v1::setM( float m ) {

   //    momentum( *this ).SetM( m );
   //    return;
   // }

   // void Jet_v1::setPx( float px ) {

   //    momentum( *this ).SetPxPyPzE( px, py(), pz(), e() );
   //    return;
   // }

   // void Jet_v1::setPy( float py ) {

   //    momentum( *this ).SetPxPyPzE( px(), py, pz(), e() );
   //    return;
   // }

   // void Jet_v1::setPz( float pz ) {

   //    momentum( *this ).SetPxPyPzE( px(), py(), pz, e() );
   //    return;
   // }

   // void Jet_v1::setE( float e ) {

   //    momentum( *this ).SetPxPyPzE( px(), py(), pz(), e );
   //    return;
   // }

   void Jet_v1::setP4( const JetFourMom_t& p4 ) {

      momentum( *this ) = p4;
      return;
   }

   void Jet_v1::setP4( const std::string& stateName, const JetFourMom_t& p4 ) {

      setAttribute< JetFourMom_t >( stateName, p4 );
      return;
   }

  /// The accessor for the cluster element links
  static SG::AuxElement::Accessor< std::vector< ElementLink< IParticleContainer > > >
      constituentAcc( "constituents" );
  static SG::AuxElement::Accessor< std::vector< float> >
      constituentWeightAcc( "constituentWeights" );

  void Jet_v1::addConstituent( const ElementLink< IParticleContainer >& link,
                               float w ) {

    constituentAcc( *this ).push_back( link );
    constituentWeightAcc( *this ).push_back( w );
    return;
  }

  void Jet_v1::addConstituent( const IParticle* c, float w ) {

    ElementLink< IParticleContainer > el;
    el.toIndexedElement( *( dynamic_cast< const IParticleContainer* >( c->container() ) ), c->index() );

    constituentAcc( *this ).push_back( el );
    constituentWeightAcc( *this ).push_back( w );
    return;
  }


  static SG::AuxElement::Accessor< int > constitSigStateAcc( "constituentSignalState" );

  SignalStateType Jet_v1::getConstituentsSignalState() const {
    if( constitSigStateAcc.isAvailable(*this) ) return (SignalStateType)constitSigStateAcc(*this) ;
    return P4SignalState::CALIBRATED; // default value
  }

  void Jet_v1::setConstituentsSignalState( SignalStateType t){
    constitSigStateAcc(*this) = t;
  }



  JetConstituentVector Jet_v1::getConstituents() const {
    
    JetConstituentVector vec( & constituentAcc(*this) , getConstituentsSignalState() );
    return vec;
  }

  size_t Jet_v1::numConstituents() const {

     return constituentAcc( *this ).size();
  }

  const IParticle* Jet_v1::rawConstituent(size_t i) const {
    return * ( constituentAcc(*this)[i]);
  }


  ////////////////////////////////////////////////////////


  static SG::AuxElement::Accessor<JetFourMom_t> jetConstituentScaleAcc("JETCONSTITUENTSCALE");
  static SG::AuxElement::Accessor<JetFourMom_t> jetEMScaleAcc("JETEMSCALE");

  /// Access by the enum to the historical states
  JetFourMom_t Jet_v1::jetP4(P4SignalState::State s) const {
    switch( s ){ 
    case P4SignalState::JETEMSCALE: return jetEMScaleAcc( *this );
    case P4SignalState::JETCONSTITUENTSCALE: return jetConstituentScaleAcc( *this );
    default :
      std::cerr << " Error p4(P4SignalState) unknown state "<< s << std::endl;
      break;
    }
    return JetFourMom_t();
  }

  /// Access by the enum to the historical states
  void Jet_v1::setJetP4(P4SignalState::State s, const JetFourMom_t &p4)  {
    switch( s ){ 
    case P4SignalState::JETEMSCALE: 
      jetEMScaleAcc( *this ) = p4;
      break;
    case P4SignalState::JETCONSTITUENTSCALE: 
      jetConstituentScaleAcc( *this ) = p4;
      break;
    default :
      std::cerr << " Error p4(P4SignalState) unknown state "<< s << std::endl;
      break;
    }
  }


  /// Generic access to states
  JetFourMom_t Jet_v1::jetP4(const std::string& statename) const {
    return getAttribute<JetFourMom_t>(statename);
  }

  void Jet_v1::setJetP4(const std::string& statename, const JetFourMom_t &p4)  {
    return setAttribute<JetFourMom_t>(statename, p4);
  }

  void Jet_v1::toPersistent() {

    if( constituentAcc.isAvailableWritable( *this ) ) { 
       std::vector< ElementLink< IParticleContainer > >::iterator itr =
          constituentAcc( *this ).begin();
       std::vector< ElementLink< IParticleContainer > >::iterator end =
          constituentAcc( *this ).end();
       for( ; itr != end; ++itr ) {
          itr->toPersistent();
       }
    }
    
    return;
  }

  void Jet_v1::reset() {
    m_assoParticleCache.clear();
    m_pseudoJet = 0;
  }



  /// //////////////////////////////
  /// getAttribute specialization for double (convert into float)
  template<>
  bool Jet_v1::getAttribute<double>( AttributeID type, double & value ) const {
    float fv;
    bool floatres = getAttribute(type, fv);
    if( floatres) value = fv;
    return floatres;
  }

  template<>
  bool Jet_v1::getAttribute<std::vector<double> >( AttributeID type, std::vector<double>& value ) const {
    std::vector<float> fv;
    bool floatres = getAttribute(type, fv);
    if(floatres) {
      value.assign( fv.begin(), fv.end() );
    }
    return floatres;
  }

  template<>
  bool Jet_v1::getAttribute<double>( const std::string & name, double & value ) const {
    float fv;
    bool floatres = getAttribute(name, fv);
    if( floatres) value = fv;
    return floatres;
  }

  template<>
  bool Jet_v1::getAttribute<std::vector<double> >( const std::string & name, std::vector<double>& value ) const {
    std::vector<float> fv;
    bool floatres = getAttribute(name, fv);
    if(floatres)  value.assign( fv.begin(), fv.end() );
    return floatres;
  }

  template<>
  double Jet_v1::getAttribute<double>(const std::string &name) const {
    return getAttribute<float>(name);
  }

  template<>
  std::vector<double> Jet_v1::getAttribute<std::vector<double> >(const std::string &name) const {
    const std::vector<float> & vf = this->auxdata< std::vector<float> >(name);
    std::vector<double> r; r.assign(vf.begin(), vf.end());
    return r;
    
  }


  
  /// //////////////////////////////
  /// setAttribute specialization for double (convert into float)

  template<>
  void Jet_v1::setAttribute<double>( AttributeID type, const double& value )  {
    ( *( xAOD::JetAttributeAccessor_v1<float>::accessor( type ) ) )( *this ) = value;
    
  }

  template<>
  void Jet_v1::setAttribute<std::vector<double> >( AttributeID type, const std::vector<double>& value )  {
    std::vector<float> & floatv = ( *( xAOD::JetAttributeAccessor_v1<std::vector<float> >::accessor( type ) ) )( *this ) ;
    floatv.assign(value.begin(), value.end() ) ;
  }

  template<>
  void Jet_v1::setAttribute<double>( const std::string & name, const double& value )  {
    this->auxdata<float>(name) = value;    
  }

  template<>
  void Jet_v1::setAttribute<std::vector<double> >( const std::string & name, const std::vector<double>& value )  {
    std::vector<float> & floatv = this->auxdata<std::vector<float> >(name);
    floatv.assign(value.begin(), value.end() ) ;
  }






  // /// //////////////////////////////
  // /// setAssociatedParticles specialization for IParticle
  // template<>
  // bool Jet_v1::setAssociatedParticles< IParticle >(const std::string &name, const std::vector<const IParticle*> & vec){

  //   std::vector< ElementLink< IParticleContainer > > & elv = this->auxdata< std::vector< ElementLink< IParticleContainer > > >( name );
  //   m_assoParticleCache[name] = vec ;
    
  //   for(size_t i=0; i< vec.size() ; i++) { 
  //     ElementLink<IParticleContainer> el;
  //     el.toIndexedElement( *( dynamic_cast< const IParticleContainer* >( vec[i]->container() ) ), vec[i]->index() );      
  //     elv.push_back(el); 
  //     elv.back().toPersistent(); // The EL will not change anymore since it is purely internal. We can call toPersistent() to be sure it will be saved as expected.
  //   }
    

  //   return true ; /// fixme : return false  if the above lines fail ?
  // }


 ////////////////////////////////////////////////////////
  const std::vector<const IParticle*> &  Jet_v1::getAssociatedParticles(const std::string &name) const {
    VectIPartMap_t::iterator it = m_assoParticleCache.find( name );
    if( it != m_assoParticleCache.end() ) return it->second;
    
    const std::vector< ElementLink< IParticleContainer > > & elv = this->auxdata< std::vector< ElementLink< IParticleContainer > > >( name );    
    //std::vector<const IParticle*>  & ipvec = const_cast<Jet_v1*>(this)->auxdata< std::vector<const IParticle*> >(vec_name);
    std::vector<const IParticle*>  &ipvec = m_assoParticleCache[name];
    ipvec.resize( elv.size() );      
    for(size_t i=0;i<elv.size(); i++) ipvec[i] = *(elv[i]) ;        
    return ipvec;    
  }

 ////////////////////////////////////////////////////////
  const std::vector<const IParticle*> &  Jet_v1::getAssociatedParticles(AssoParticlesID type) const {
    JetAssoParticleAccessors_v1::VectorIParticleAccessor_t *acc = JetAssoParticleAccessors_v1::accessor( type);
    if( acc ) { /// then we'resure the AssoParticlesID is defined
      VectIPartMap_t::iterator it = m_assoParticleCache.find( JetAssoParticleAccessors_v1::assoName(type ) );
      if( it != m_assoParticleCache.end() ) return it->second;

      std::vector< ElementLink< IParticleContainer > > & elv =  (*acc)(*this );
      std::vector<const IParticle*>  &ipvec = m_assoParticleCache[name];
      ipvec.resize( elv.size() );      
      for(size_t i=0;i<elv.size(); i++) ipvec[i] = *(elv[i]) ;        
      return ipvec;    
    }
    // else return a static vector
    static std::vector<const IParticle*>  s_ipvec;
    return s_ipvec;
  }


  static SG::AuxElement::Accessor<int> inputAcc("InputType");
  static SG::AuxElement::Accessor<int> algAcc("AlgorithmType");
  static SG::AuxElement::Accessor<float> spAcc("SizeParameter");

  float Jet_v1::getSizeParameter() const {
    return spAcc(*this);
  }
  
  JetAlgorithmType::ID Jet_v1::getAlgorithmType()const {
    return (JetAlgorithmType::ID) algAcc(*this);
  }


  JetInput::Type Jet_v1::getInputType()const {
    return (JetInput::Type) inputAcc(*this);
  }

  void Jet_v1::setSizeParameter(float p)  { spAcc(*this) = p;}
  void Jet_v1::setAlgorithmType(JetAlgorithmType::ID a)  { algAcc(*this) = a;}
  void Jet_v1::setInputType(JetInput::Type t)  {inputAcc(*this)=t;}
  
  

  // Jet_v1* Jet_v1::clone(const std::string& state) const {
  //   Jet_v1* j = new Jet_v1();
  //   j->setStore( getConstStore() );
  //   j->setIndex( index(), const_cast<SG::AuxVectorData*>( container() ) );
  //   if(state!="") {
  //     j->setP4( j->get<JetFourMom_t>(state) );
  //   }else{
  //     j->setP4(m_momentum);
  //   }
  //   return j;
  // }

} // namespace xAOD

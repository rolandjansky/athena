/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JGTower_v1.h"

namespace xAOD{  

  JGTower_v1::JGTower_v1() :
    IParticle(), 
    m_p4(), 
    m_p4Cached( false )
  {        
  }


  
  JGTower_v1::JGTower_v1(const JGTower_v1& other) :
    IParticle( other ),
    m_p4( other.m_p4 ),
    m_p4Cached( other.m_p4Cached )
  {   
  }
  
  JGTower_v1& JGTower_v1::operator=(const JGTower_v1& other)
  {
    if( this != &other ){
      if( !container() && !hasStore() ){
        makePrivateStore();
      }
      IParticle::operator=( other );
      m_p4 = other.m_p4;
      m_p4Cached = other.m_p4Cached;
    }
    return *this;
  }
//Objects stored in this way will be hidden information   
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JGTower_v1 , std::vector<int> , SCIndex , setSCIndex )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JGTower_v1 , std::vector<int> , TileIndex , setTileIndex )

  /// initialize
  void JGTower_v1::initialize(const int Id, const float Eta,const float Phi)
  {
    setId (Id);
    setEta( Eta );
    setPhi( Phi );    
  }
  
  void JGTower_v1::initialize(const int Id, const float Eta,const float Phi, const float Et)
  {
    setId (Id);
    setEt (Et);
    setEta( Eta );
    setPhi( Phi );
  }
 
  // IParticle interface
  
//  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JGTower_v1, int , id , setId )

  double JGTower_v1::eta() const
  {
    static Accessor< float > acc( "eta" );
    return acc( *this );      
  }

  void JGTower_v1::setEta(float eta)
  {
    static Accessor< float > acc( "eta" );
    acc( *this ) = eta;
  }

  
  double JGTower_v1::phi() const
  {
    static Accessor< float > acc( "phi" );
    return acc( *this );      
  }
  
  void JGTower_v1::setPhi(float phi)
  {
    static Accessor< float > acc( "phi" );
    acc( *this ) = phi;    
  } 

  double JGTower_v1::deta() const
  {
    static Accessor< float > acc( "deta" );
    return acc( *this );
  }

  void JGTower_v1::setdEta(float deta)
  {
    static Accessor< float > acc( "deta" );
    acc( *this ) = deta;
  }

  double JGTower_v1::dphi() const
  {
    static Accessor< float > acc( "dphi" );
    return acc( *this );
  }

  void JGTower_v1::setdPhi(float dphi)
  {
    static Accessor< float > acc( "dphi" );
    acc( *this ) = dphi;
  }

  int JGTower_v1::Id() const
  {
    static Accessor< int > acc( "Id" );
    return acc( *this );
  }

  void JGTower_v1::setId(int Id)
  {
    static Accessor< int > acc( "Id" );
    acc( *this ) = Id;
  }

  int JGTower_v1::sampling() const
  {
    static Accessor< int > acc( "sampling" );
    return acc( *this );
  }

  void JGTower_v1::setSampling(int sampling)
  {
    static Accessor< int > acc( "sampling" );
    acc( *this ) = sampling;
  }


  double JGTower_v1::et() const
  {
    static Accessor< float > acc( "et" );
    return acc( *this );
  }

  void JGTower_v1::setEt(float et)
  {
    static Accessor< float > acc( "et" );
    acc( *this ) = et;
  }

    double JGTower_v1::pt() const
  {
    return static_cast< double >(pt() );
  }
  
  double JGTower_v1::m() const
  {
    return 0.;
  }

  double JGTower_v1::e() const
  {
    return p4().E();
  }

  double JGTower_v1::rapidity() const
  {
    return p4().Rapidity();
  }

  const JGTower_v1::FourMom_t& JGTower_v1::p4() const
  {
    if( ! m_p4Cached ) {
      
      double PT = (pt() > 0) ? 1000. * static_cast< double >( pt() ) : 0.01;
      
      m_p4.SetPtEtaPhiM( PT , eta() , phi() , m() );
      m_p4Cached = true;      
    }
    return m_p4;       
  }

  Type::ObjectType JGTower_v1::type() const
  {
    return Type::Jet;
  }
  
} // namespace xAOD

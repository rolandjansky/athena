/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JGTower_v1.h"

namespace xAOD{  

  JGTower_v1::JGTower_v1() :
    IParticle()
  {        
  }


  
  JGTower_v1::JGTower_v1(const JGTower_v1& other) :
    IParticle( other )
  {   
  }
  
  JGTower_v1& JGTower_v1::operator=(const JGTower_v1& other)
  {
    if( this != &other ){
      if( !container() && !hasStore() ){
        makePrivateStore();
      }
      IParticle::operator=( other );
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
    const static ConstAccessor< float > acc( "eta" );
    return acc( *this );      
  }

  void JGTower_v1::setEta(float eta)
  {
    const static Accessor< float > acc( "eta" );
    acc( *this ) = eta;
  }

  
  double JGTower_v1::phi() const
  {
    const static ConstAccessor< float > acc( "phi" );
    return acc( *this );      
  }
  
  void JGTower_v1::setPhi(float phi)
  {
    const static Accessor< float > acc( "phi" );
    acc( *this ) = phi;    
  } 

  double JGTower_v1::deta() const
  {
    const static ConstAccessor< float > acc( "deta" );
    return acc( *this );
  }

  void JGTower_v1::setdEta(float deta)
  {
    const static Accessor< float > acc( "deta" );
    acc( *this ) = deta;
  }

  double JGTower_v1::dphi() const
  {
    const static ConstAccessor< float > acc( "dphi" );
    return acc( *this );
  }

  void JGTower_v1::setdPhi(float dphi)
  {
    const static Accessor< float > acc( "dphi" );
    acc( *this ) = dphi;
  }

  int JGTower_v1::Id() const
  {
    const static ConstAccessor< int > acc( "Id" );
    return acc( *this );
  }

  void JGTower_v1::setId(int Id)
  {
    const static Accessor< int > acc( "Id" );
    acc( *this ) = Id;
  }

  int JGTower_v1::sampling() const
  {
    const static ConstAccessor< int > acc( "sampling" );
    return acc( *this );
  }

  void JGTower_v1::setSampling(int sampling)
  {
    const static Accessor< int > acc( "sampling" );
    acc( *this ) = sampling;
  }


  double JGTower_v1::et() const
  {
    const static ConstAccessor< float > acc( "et" );
    return acc( *this );
  }

  void JGTower_v1::setEt(float et)
  {
    const static Accessor< float > acc( "et" );
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

  JGTower_v1::FourMom_t JGTower_v1::p4() const
  {
    FourMom_t p4;
    double PT = (pt() > 0) ? 1000. * static_cast< double >( pt() ) : 0.01;
    p4.SetPtEtaPhiM( PT , eta() , phi() , m() );
    return p4;    
  }

  Type::ObjectType JGTower_v1::type() const
  {
    return Type::Jet;
  }
  
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v2.cxx 642659 2015-01-29 12:41:06Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTower_v2.h"

namespace xAOD{  

  TriggerTower_v2::TriggerTower_v2() :
    IParticle(), 
    m_p4(), 
    m_p4Cached( false )
  {        
  }
  
  TriggerTower_v2::TriggerTower_v2(const TriggerTower_v2& other) :
    IParticle( other ),
    m_p4( other.m_p4 ),
    m_p4Cached( other.m_p4Cached )
  {   
  }
  
  TriggerTower_v2& TriggerTower_v2::operator=(const TriggerTower_v2& other)
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
    
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint_least8_t> , lut_cp , setLut_cp )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint_least8_t> , lut_jep , setLut_jep )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<int_least16_t> , correction , setCorrection )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint_least8_t> , correctionEnabled , setCorrectionEnabled )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint_least8_t> , bcidVec , setBcidVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint_least16_t> , adc , setAdc )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint_least8_t> , bcidExt , setBcidExt )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least32_t , coolId , setCoolId )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least8_t , layer , setLayer )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least16_t , error , setError )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least8_t , peak , setPeak )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least8_t , adcPeak , setAdcPeak )  
  
  
  /// Fill tower with all information
  
  void TriggerTower_v2::initialize(const uint_least32_t CoolId,const uint_least8_t Layer,const float Eta,const float Phi,
                                   const std::vector<uint_least8_t>& Lut_cp,const std::vector<uint_least8_t>& Lut_jep,
                                   const std::vector<int_least16_t>& Correction,const std::vector<uint_least8_t>& CorrectionEnabled,
                                   const std::vector<uint_least8_t>& BcidVec,const std::vector<uint_least16_t>& Adc,
                                   const std::vector<uint_least8_t>& BcidExt,
                                   const uint_least16_t Error,
                                   const uint_least8_t Peak,
                                   const uint_least8_t AdcPeak)
  {
    setCoolId( CoolId );
    setLayer( Layer );
    setEta( Eta );
    setPhi( Phi );
    setLut_cp( Lut_cp );
    setLut_jep( Lut_jep );
    setCorrection( Correction );
    setCorrectionEnabled( CorrectionEnabled );
    setBcidVec( BcidVec );
    setAdc( Adc );
    setBcidExt( BcidExt );
    setError( Error );
    setPeak( Peak );
    setAdcPeak( AdcPeak );
  }
    
  /// get cpET from peak of lut_cp
  uint_least8_t TriggerTower_v2::cpET() const
  {
    return lut_cp()[ peak() ];
  }
  
  /// get jepET from peak of lut_jep
  uint_least8_t TriggerTower_v2::jepET() const
  {
    return lut_jep()[ peak() ];
  }
  
  /// Is tower saturated?
    // return true if the TriggerTower is saturated
    // i.e. it has an energy greater than the saturation threshold
    // otherwise, return false;
      
  bool TriggerTower_v2::isCpSaturated() const
  {
    if( cpET() >= m_saturation ){
      return true;
    }
    return false;    
  }
  
  bool TriggerTower_v2::isJepSaturated() const
  {    
    if( jepET() >= m_saturation ){
      return true;
    }
    return false;    
  }  
  
  
  // IParticle interface
  

  double TriggerTower_v2::eta() const
  {
    static Accessor< float > acc( "eta" );
    return acc( *this );      
  }
  
  void TriggerTower_v2::setEta(float eta)
  {
    static Accessor< float > acc( "eta" );
    acc( *this ) = eta;    
  }

  double TriggerTower_v2::phi() const
  {
    static Accessor< float > acc( "phi" );
    return acc( *this );      
  }
  
  void TriggerTower_v2::setPhi(float phi)
  {
    static Accessor< float > acc( "phi" );
    acc( *this ) = phi;    
  } 
  
  double TriggerTower_v2::pt() const
  {
    return static_cast< double >( jepET() );
  }
  
  double TriggerTower_v2::m() const
  {
    return 0.;
  }

  double TriggerTower_v2::e() const
  {
    return p4().E();
  }

  double TriggerTower_v2::rapidity() const
  {
    return p4().Rapidity();
  }

  const TriggerTower_v2::FourMom_t& TriggerTower_v2::p4() const
  {
    if( ! m_p4Cached ) {
      m_p4.SetPtEtaPhiM( pt() , eta() , phi() , m() );
      m_p4Cached = true;      
    }
    return m_p4;       
  }

  Type::ObjectType TriggerTower_v2::type() const
  {
    return Type::Jet;
  }
  
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v2.cxx 793305 2017-01-21 04:50:50Z ssnyder $

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
    
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint8_t> , lut_cp , setLut_cp )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint8_t> , lut_cp , setLut_cp )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint8_t> , lut_jep , setLut_jep )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint8_t> , lut_jep , setLut_jep )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<int16_t> , correction , setCorrection )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<int16_t> , correction , setCorrection )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint8_t> , correctionEnabled , setCorrectionEnabled )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint8_t> , correctionEnabled , setCorrectionEnabled )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint8_t> , bcidVec , setBcidVec )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint8_t> , bcidVec , setBcidVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint16_t> , adc , setAdc )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint16_t> , adc , setAdc )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint8_t> , bcidExt , setBcidExt )  
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint8_t> , bcidExt , setBcidExt )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v2 , std::vector<uint8_t> , sat80Vec , setSat80Vec )
  AUXSTORE_OBJECT_MOVE( TriggerTower_v2 , std::vector<uint8_t> , sat80Vec , setSat80Vec )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint32_t , coolId , setCoolId )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint32_t , errorWord , setErrorWord )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint8_t , peak , setPeak )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint8_t , adcPeak , setAdcPeak )  
  
  
  /// initialize
  void TriggerTower_v2::initialize(const uint32_t CoolId,const float Eta,const float Phi)
  {
    setCoolId( CoolId );
    setEta( Eta );
    setPhi( Phi );    
  }
  
  void TriggerTower_v2::initialize(const uint32_t CoolId,const float Eta,const float Phi,
                                   const std::vector<uint8_t>& Lut_cp,const std::vector<uint8_t>& Lut_jep,
                                   const std::vector<int16_t>& Correction,const std::vector<uint8_t>& CorrectionEnabled,
                                   const std::vector<uint8_t>& BcidVec,const std::vector<uint16_t>& Adc,
                                   const std::vector<uint8_t>& BcidExt,const std::vector<uint8_t>& Sat80Vec,
                                   const uint32_t ErrorWord,
                                   const uint8_t Peak,
                                   const uint8_t AdcPeak)
  {
    setCoolId( CoolId );
    setEta( Eta );
    setPhi( Phi );
    setLut_cp( Lut_cp );
    setLut_jep( Lut_jep );
    setCorrection( Correction );
    setCorrectionEnabled( CorrectionEnabled );
    setBcidVec( BcidVec );
    setAdc( Adc );
    setBcidExt( BcidExt );
    setSat80Vec( Sat80Vec );
    setErrorWord( ErrorWord );
    setPeak( Peak );
    setAdcPeak( AdcPeak );
  }
  
  void TriggerTower_v2::initialize(const uint32_t CoolId,const float Eta,const float Phi,
                                   std::vector<uint8_t>&& Lut_cp,
                                   std::vector<uint8_t>&& Lut_jep,
                                   std::vector<int16_t>&& Correction,
                                   std::vector<uint8_t>&& CorrectionEnabled,
                                   std::vector<uint8_t>&& BcidVec,
                                   std::vector<uint16_t>&& Adc,
                                   std::vector<uint8_t>&& BcidExt,
                                   std::vector<uint8_t>&& Sat80Vec,
                                   const uint32_t ErrorWord,
                                   const uint8_t Peak,
                                   const uint8_t AdcPeak)
  {
    setCoolId( CoolId );
    setEta( Eta );
    setPhi( Phi );
    setLut_cp( std::move(Lut_cp) );
    setLut_jep( std::move(Lut_jep) );
    setCorrection( std::move(Correction) );
    setCorrectionEnabled( std::move(CorrectionEnabled) );
    setBcidVec( std::move(BcidVec) );
    setAdc( std::move(Adc) );
    setBcidExt( std::move(BcidExt) );
    setSat80Vec( std::move(Sat80Vec) );
    setErrorWord( ErrorWord );
    setPeak( Peak );
    setAdcPeak( AdcPeak );
  }
  
  int TriggerTower_v2::layer() const
  {
    unsigned int cnum = ( coolId() >> 24 ) & 0x7;
    unsigned int mnum = ( coolId() >> 16 ) & 0xf;
    // unsigned int chan = coolId() & 0x3; // To be used for FCAL2,FCAL3 soon

    if (cnum < 4) {  // EMB,EMEC
      return 0;
    }
    else if (cnum < 6) {  // HEC,FCAL
      if (mnum == 0) {
        return 0;  // FCAL1
      }
      else if (mnum == 8) {  // FCAL2,FCAL3
        // Detail to be determined (using chan): for now use layer 1
        return 1;
      }
      else {  // HEC
        return 1;
      }
    }
    else {  // Tile
      return 1;
    } 
    // really shouldn't get this far
    return 1;
  }
  
  int TriggerTower_v2::sampling() const
  {
    unsigned int cnum = ( coolId() >> 24 ) & 0x7;
    unsigned int mnum = ( coolId() >> 16 ) & 0xf;
    // unsigned int chan = coolId() & 0x3; // To be used for FCAL2,FCAL3 soon

    if (cnum < 4) {  // EMB,EMEC
      return 0;
    }
    else if (cnum < 6) {  // HEC,FCAL
      if (mnum == 0) {
        return 0;  // FCAL1
      }
      else if (mnum == 8) {  // FCAL2,FCAL3
        // Detail to be determined (using chan): for now use layer 1
        return 1;
      }
      else {  // HEC
        return 1;
      }
    }
    else {  // Tile
      return 1;
    } 
    // really shouldn't get this far
    return 1;    
  }
    
  /// get cpET from peak of lut_cp
  uint8_t TriggerTower_v2::cpET() const
  {
    return peak() < lut_cp().size()? lut_cp()[ peak() ]: 0;
  }
  
  /// get jepET from peak of lut_jep
  uint8_t TriggerTower_v2::jepET() const
  {
    return peak() < lut_jep().size()? lut_jep()[ peak() ]: 0;
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
    static const Accessor< float > acc( "eta" );
    return acc( *this );      
  }
  
  void TriggerTower_v2::setEta(float eta)
  {
    static const Accessor< float > acc( "eta" );
    acc( *this ) = eta;    
  }

  double TriggerTower_v2::phi() const
  {
    static const Accessor< float > acc( "phi" );
    return acc( *this );      
  }
  
  void TriggerTower_v2::setPhi(float phi)
  {
    static const Accessor< float > acc( "phi" );
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
      
      double PT = (jepET() > 0) ? 1000. * static_cast< double >( jepET() ) : 0.01;
      
      m_p4.SetPtEtaPhiM( PT , eta() , phi() , m() );
      m_p4Cached = true;      
    }
    return m_p4;       
  }

  Type::ObjectType TriggerTower_v2::type() const
  {
    return Type::Jet;
  }
  
} // namespace xAOD

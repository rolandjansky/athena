/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v2.cxx 619551 2014-10-02 07:33:22Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTower_v2.h"

namespace xAOD{  

  TriggerTower_v2::TriggerTower_v2() :
    SG::AuxElement()
  {        
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
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, float , eta , setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least16_t , error , setError )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least8_t , peak , setPeak )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v2, uint_least8_t , adcPeak , setAdcPeak )  
  
  
  /// Fill tower with all information
  void TriggerTower_v2::initialize(const uint_least32_t& coolId,const uint_least8_t& layer,const float& eta,const float& phi,
                                   const std::vector<uint_least8_t>& lut_cp,const std::vector<uint_least8_t>& lut_jep,
                                   const std::vector<int_least16_t>& correction,const std::vector<uint_least8_t>& correctionEnabled,
                                   const std::vector<uint_least8_t>& bcidVec,const std::vector<uint_least16_t>& adc,
                                   const std::vector<uint_least8_t>& bcidExt,
                                   const uint_least16_t& error,
                                   const uint_least8_t& peak,
                                   const uint_least8_t& adcPeak)
  {
    this->setCoolId( coolId );
    this->setLayer( layer );
    this->setEta( eta );
    this->setPhi( phi );
    this->setLut_cp( lut_cp );
    this->setLut_jep( lut_jep );
    this->setCorrection( correction );
    this->setCorrectionEnabled( correctionEnabled );
    this->setBcidVec( bcidVec );
    this->setAdc( adc );
    this->setBcidExt( bcidExt );
    this->setError( error );
    this->setPeak( peak );
    this->setAdcPeak( adcPeak );
  }
    
  /// get cpET from peak of lut_cp
  uint_least8_t TriggerTower_v2::cpET() const
  {
    return this->lut_cp()[ this->peak() ];
  }
  
  /// get jepET from peak of lut_jep
  uint_least8_t TriggerTower_v2::jepET() const
  {
    return this->lut_jep()[ this->peak() ];
  }
  
  /// Is tower saturated?
    // return true if the TriggerTower is saturated
    // i.e. it has an energy greater than the saturation threshold
    // otherwise, return false;
      
  bool TriggerTower_v2::isCpSaturated() const
  {
    if( this->cpET() >= m_saturation ){
      return true;
    }
    return false;    
  }
  
  bool TriggerTower_v2::isJepSaturated() const
  {    
    if( this->jepET() >= m_saturation ){
      return true;
    }
    return false;    
  }  

} // namespace xAOD

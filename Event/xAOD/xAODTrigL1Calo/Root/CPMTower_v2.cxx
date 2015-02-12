/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTower_v2.h"

namespace xAOD{  
  
  CPMTower_v2::CPMTower_v2() :
    SG::AuxElement()
  {        
  }
  
  /// initialize
  void CPMTower_v2::initialize(const float eta,const float phi)
  {        
    setEta( eta );
    setPhi( phi );
  }  
  
  /// initialize
  void CPMTower_v2::initialize(const float eta,const float phi,
              const std::vector<uint_least8_t>& emEnergyVec,
              const std::vector<uint_least8_t>& hadEnergyVec,
              const std::vector<uint_least8_t>& emErrorVec,
              const std::vector<uint_least8_t>& hadErrorVec,
              const uint_least8_t peak)
  { 
    setEta( eta );
    setPhi( phi );
    setEmEnergyVec( emEnergyVec );
    setHadEnergyVec( hadEnergyVec );
    setEmErrorVec( emErrorVec );
    setHadErrorVec( hadErrorVec );
    setPeak( peak );
  }  
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint_least8_t> , emEnergyVec , setEmEnergyVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint_least8_t> , hadEnergyVec , setHadEnergyVec )    
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint_least8_t> , emErrorVec , setEmErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint_least8_t> , hadErrorVec , setHadErrorVec )     
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , float , eta , setEta )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , uint_least8_t , peak , setPeak )  
 
  /** return the final ET values using the in algorithms */
  uint_least8_t CPMTower_v2::emEnergy() const
  {
    return emEnergyVec()[ peak() ];
  }
  
  uint_least8_t CPMTower_v2::hadEnergy() const
  {
    return hadEnergyVec()[ peak() ];
  }

  uint_least8_t CPMTower_v2::emSliceEnergy(unsigned int slice) const
  {
    if( slice < emEnergyVec().size() ){
      return emEnergyVec()[ slice ];
    }
    return 0;    
  }
  
  uint_least8_t CPMTower_v2::hadSliceEnergy(unsigned int slice) const
  {
    if( slice < hadEnergyVec().size() ){
      return hadEnergyVec()[ slice ];
    }
    return 0;        
  }
  
  uint_least8_t CPMTower_v2::emError() const
  {
    return emErrorVec()[ peak() ];
  }
  
  uint_least8_t CPMTower_v2::hadError() const
  {
    return hadErrorVec()[ peak() ];
  }
  
  uint_least8_t CPMTower_v2::emSliceError(unsigned int slice) const
  {
    if( slice < emErrorVec().size() ){
      return emErrorVec()[ slice ];
    }
    return 0;      
  }
  
  uint_least8_t CPMTower_v2::hadSliceError(unsigned int slice) const
  {
    if( slice < hadErrorVec().size() ){
      return hadErrorVec()[ slice ];
    }
    return 0;      
  }
   
} // namespace xAOD

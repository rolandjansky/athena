/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v2.cxx 687949 2015-08-06 15:48:49Z amazurov $

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
              const std::vector<uint8_t>& emEnergyVec,
              const std::vector<uint8_t>& hadEnergyVec,
              const std::vector<uint32_t>& emErrorVec,
              const std::vector<uint32_t>& hadErrorVec,
              const uint8_t peak)
  { 
    setEta( eta );
    setPhi( phi );
    setEmEnergyVec( emEnergyVec );
    setHadEnergyVec( hadEnergyVec );
    setEmErrorVec( emErrorVec );
    setHadErrorVec( hadErrorVec );
    setPeak( peak );
  }  
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint8_t> , emEnergyVec , setEmEnergyVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint8_t> , hadEnergyVec , setHadEnergyVec )    
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint32_t> , emErrorVec , setEmErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<uint32_t> , hadErrorVec , setHadErrorVec )     
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , float , eta , setEta )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , uint8_t , peak , setPeak )  
 
  /** return the final ET values using the in algorithms */
  uint8_t CPMTower_v2::emEnergy() const
  {
    return emSliceEnergy(peak());
  }
  
  uint8_t CPMTower_v2::hadEnergy() const
  {
    return hadSliceEnergy(peak());
  }

  uint8_t CPMTower_v2::emSliceEnergy(unsigned int slice) const
  {
    if( slice < emEnergyVec().size() ){
      return emEnergyVec()[ slice ];
    }
    return 0;    
  }
  
  uint8_t CPMTower_v2::hadSliceEnergy(unsigned int slice) const
  {
    if( slice < hadEnergyVec().size() ){
      return hadEnergyVec()[ slice ];
    }
    return 0;        
  }
  
  uint32_t CPMTower_v2::emError() const
  {
    return emSliceError(peak());
  }
  
  uint32_t CPMTower_v2::hadError() const
  {
    return hadSliceError(peak());
  }
  
  uint32_t CPMTower_v2::emSliceError(unsigned int slice) const
  {
    if( slice < emErrorVec().size() ){
      return emErrorVec()[ slice ];
    }
    return 0;      
  }
  
  uint32_t CPMTower_v2::hadSliceError(unsigned int slice) const
  {
    if( slice < hadErrorVec().size() ){
      return hadErrorVec()[ slice ];
    }
    return 0;      
  }
   
} // namespace xAOD

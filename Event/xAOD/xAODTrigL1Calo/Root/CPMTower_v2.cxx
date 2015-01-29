/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v2.cxx 642659 2015-01-29 12:41:06Z morrisj $

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
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<int> , emEnergyVec , setEmEnergyVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<int> , hadEnergyVec , setHadEnergyVec )    
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<int> , emErrorVec , setEmErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v2 , std::vector<int> , hadErrorVec , setHadErrorVec )     
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , float , eta , setEta )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v2 , uint_least8_t , peak , setPeak )  
 
  /** return the final ET values using the in algorithms */
  int CPMTower_v2::emEnergy() const
  {
    return emEnergyVec()[ peak() ];
  }
  
  int CPMTower_v2::hadEnergy() const
  {
    return hadEnergyVec()[ peak() ];
  }

  int CPMTower_v2::emSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)emEnergyVec().size() ){
      return emEnergyVec()[ slice ];
    }
    return 0;    
  }
  
  int CPMTower_v2::hadSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)hadEnergyVec().size() ){
      return hadEnergyVec()[ slice ];
    }
    return 0;        
  }
  
  int CPMTower_v2::emError() const
  {
    return emErrorVec()[ peak() ];
  }
  
  int CPMTower_v2::hadError() const
  {
    return hadErrorVec()[ peak() ];
  }
  
  int CPMTower_v2::emSliceError(int slice) const
  {
    if( slice >= 0 && slice < (int)emErrorVec().size() ){
      return emErrorVec()[ slice ];
    }
    return 0;      
  }
  
  int CPMTower_v2::hadSliceError(int slice) const
  {
    if( slice >= 0 && slice < (int)hadErrorVec().size() ){
      return hadErrorVec()[ slice ];
    }
    return 0;      
  }
   
} // namespace xAOD

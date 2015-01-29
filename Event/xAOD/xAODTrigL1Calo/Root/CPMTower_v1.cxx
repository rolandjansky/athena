/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTower_v1.h"

namespace xAOD{  
  
  CPMTower_v1::CPMTower_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v1 , std::vector<int> , emEnergyVec , setEmEnergyVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v1 , std::vector<int> , hadEnergyVec , setHadEnergyVec )    
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v1 , std::vector<int> , emErrorVec , setEmErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMTower_v1 , std::vector<int> , hadErrorVec , setHadErrorVec )     
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v1 , float , eta , setEta )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v1 , float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTower_v1 , int , peak , setPeak )  
 
  /** return the final ET values using the in algorithms */
  int CPMTower_v1::emEnergy() const
  {
    return emEnergyVec()[ peak() ];
  }
  
  int CPMTower_v1::hadEnergy() const
  {
    return hadEnergyVec()[ peak() ];
  }

  int CPMTower_v1::emSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)emEnergyVec().size() ){
      return emEnergyVec()[ slice ];
    }
    return 0;    
  }
  
  int CPMTower_v1::hadSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)hadEnergyVec().size() ){
      return hadEnergyVec()[ slice ];
    }
    return 0;        
  }
  
  int CPMTower_v1::emError() const
  {
    return emErrorVec()[ peak() ];
  }
  
  int CPMTower_v1::hadError() const
  {
    return hadErrorVec()[ peak() ];
  }
  
  int CPMTower_v1::emSliceError(int slice) const
  {
    if( slice >= 0 && slice < (int)emErrorVec().size() ){
      return emErrorVec()[ slice ];
    }
    return 0;      
  }
  
  int CPMTower_v1::hadSliceError(int slice) const
  {
    if( slice >= 0 && slice < (int)hadErrorVec().size() ){
      return hadErrorVec()[ slice ];
    }
    return 0;      
  }
   
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

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
    return this->emEnergyVec()[ this->peak() ];
  }
  
  int CPMTower_v1::hadEnergy() const
  {
    return this->hadEnergyVec()[ this->peak() ];
  }

  int CPMTower_v1::emSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)this->emEnergyVec().size() ){
      return this->emEnergyVec()[ slice ];
    }
    return 0;    
  }
  
  int CPMTower_v1::hadSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)this->hadEnergyVec().size() ){
      return this->hadEnergyVec()[ slice ];
    }
    return 0;        
  }
  
  int CPMTower_v1::emError() const
  {
    return this->emErrorVec()[ this->peak() ];
  }
  
  int CPMTower_v1::hadError() const
  {
    return this->hadErrorVec()[ this->peak() ];
  }
  
  int CPMTower_v1::emSliceError(int slice) const
  {
    if( slice >= 0 && slice < (int)this->emErrorVec().size() ){
      return this->emErrorVec()[ slice ];
    }
    return 0;      
  }
  
  int CPMTower_v1::hadSliceError(int slice) const
  {
    if( slice >= 0 && slice < (int)this->hadErrorVec().size() ){
      return this->hadErrorVec()[ slice ];
    }
    return 0;      
  }
   
} // namespace xAOD

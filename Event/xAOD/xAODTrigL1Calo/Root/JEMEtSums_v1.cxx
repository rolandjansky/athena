/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSums_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSums_v1.h"

namespace xAOD{  
  
  JEMEtSums_v1::JEMEtSums_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMEtSums_v1 , int , crate , setCrate )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMEtSums_v1 , int , module , setModule )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMEtSums_v1 , int , peak , setPeak )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JEMEtSums_v1 , std::vector<unsigned int> , etVec , setEtVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JEMEtSums_v1 , std::vector<unsigned int> , exVec , setExVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JEMEtSums_v1 , std::vector<unsigned int> , eyVec , setEyVec )  
    
  unsigned int JEMEtSums_v1::forward() const
  {
    unsigned int result(0);
    int mod = this->module();
    if( ((mod%8) == 0)  || ((mod%8) == 7) ){
      result = 1;
    }
    return result;
  }
  
  unsigned int JEMEtSums_v1::et() const
  {
    return this->etVec()[ this->peak() ];  
  }
  
  unsigned int JEMEtSums_v1::ex() const
  {
    return this->exVec()[ this->peak() ]; 
  }
  
  unsigned int JEMEtSums_v1::ey() const
  {
    return this->eyVec()[ this->peak() ]; 
  }
  
} // namespace xAOD

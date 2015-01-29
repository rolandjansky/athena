/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMHits_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMHits_v1.h"

namespace xAOD{  
  
  CPMHits_v1::CPMHits_v1() :
    SG::AuxElement()
  {        
  }
       
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMHits_v1 , int , crate , setCrate )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMHits_v1 , int , module , setModule )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMHits_v1 , int , peak , setPeak )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMHits_v1 , std::vector<unsigned int> , hitsVec0 , setHitsVec0 )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CPMHits_v1 , std::vector<unsigned int> , hitsVec1 , setHitsVec1 )  
  
  void CPMHits_v1::addHits(const std::vector<unsigned int>&  hits0,const std::vector<unsigned int>&  hits1)
  {
    setHitsVec0( hits0 );
    setHitsVec1( hits1 );
  }
          
  unsigned int CPMHits_v1::hitWord0() const
  {
    return hitsVec0()[ peak() ]; 
  }
  
  unsigned int CPMHits_v1::hitWord1() const
  {
    return hitsVec1()[ peak() ];
  }
  
} // namespace xAOD

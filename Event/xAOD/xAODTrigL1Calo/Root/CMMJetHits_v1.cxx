/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMJetHits_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMJetHits_v1.h"

namespace xAOD{  
  
  CMMJetHits_v1::CMMJetHits_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMJetHits_v1 , int , crate , setCrate )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMJetHits_v1 , int , dataID , setDataID )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMJetHits_v1 , int , peak , setPeak )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMJetHits_v1 , std::vector<unsigned int> , hitsVec , setHitsVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMJetHits_v1 , std::vector<int> , errorVec , setErrorVec )  
  
  void CMMJetHits_v1::addHits(const std::vector<unsigned int>& hits,const std::vector<int>& error)
  {
    this->setHitsVec( hits );
    this->setErrorVec( error );

  }
              
  unsigned int CMMJetHits_v1::hits() const
  {
    return this->hitsVec()[ this->peak() ]; 
  }
    
  int CMMJetHits_v1::error() const
  {
    return this->errorVec()[ this->peak() ]; 
  }
    
} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSums_v2.cxx 652807 2015-03-09 21:52:07Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSums_v2.h"

namespace xAOD{  
  
  JEMEtSums_v2::JEMEtSums_v2() :
    SG::AuxElement()
  {        
  }
  
  /// initialize
  void JEMEtSums_v2::initialize(const uint8_t crate,const uint8_t module)
  {       
    setCrate( crate );
    setModule( module );
  }

  /// initialize
  void JEMEtSums_v2::initialize(const uint8_t crate,const uint8_t module,
                                const std::vector<uint16_t>& etVec,
                                const std::vector<uint16_t>& exVec,
                                const std::vector<uint16_t>& eyVec,
                                const uint8_t peak)
  {  
    setCrate( crate );
    setModule( module );
    setEtVec( etVec );
    setExVec( exVec );
    setEyVec( eyVec );
    setPeak( peak );
  }


  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMEtSums_v2 , uint8_t , crate , setCrate )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMEtSums_v2 , uint8_t , module , setModule )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMEtSums_v2 , uint8_t , peak , setPeak )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JEMEtSums_v2 , std::vector<uint16_t> , etVec , setEtVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JEMEtSums_v2 , std::vector<uint16_t> , exVec , setExVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JEMEtSums_v2 , std::vector<uint16_t> , eyVec , setEyVec )  
    
  uint8_t JEMEtSums_v2::forward() const
  {
    uint8_t result(0);
    uint8_t mod = module();
    if( ((mod%8) == 0)  || ((mod%8) == 7) ){
      result = 1;
    }
    return result;
  }
  
  uint16_t JEMEtSums_v2::et() const
  {
    return etVec()[ peak() ];  
  }
  
  uint16_t JEMEtSums_v2::ex() const
  {
    return exVec()[ peak() ]; 
  }
  
  uint16_t JEMEtSums_v2::ey() const
  {
    return eyVec()[ peak() ]; 
  }
  
} // namespace xAOD

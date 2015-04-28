/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoRawData_v1.cxx 652824 2015-03-09 23:24:32Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/L1TopoRawData_v1.h"

namespace xAOD{  
  
  L1TopoRawData_v1::L1TopoRawData_v1() :
    SG::AuxElement()
  {        
  }
  
  /// initialize
  void L1TopoRawData_v1::initialize(const std::vector<uint32_t>& dataWords,
                                    const std::vector<uint32_t>& statusWords,
                                    const uint32_t errorWord,
                                    const uint32_t sourceID)
  { 
    setDataWords( dataWords );
    setStatusWords( statusWords );
    setErrorWord( errorWord );
    setSourceID( sourceID );
  }  
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( L1TopoRawData_v1 , std::vector<uint32_t> , dataWords , setDataWords )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( L1TopoRawData_v1 , std::vector<uint32_t> , statusWords , setStatusWords )     
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoRawData_v1 , uint32_t , errorWord , setErrorWord )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoRawData_v1 , uint32_t , sourceID , setSourceID )  
 
} // namespace xAOD

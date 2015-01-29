/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSums_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMEtSums_v1.h"

namespace xAOD{ 
  
  CMMEtSums_v1::CMMEtSums_v1() :
    SG::AuxElement()
  {        
  }  
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMEtSums_v1 , int , crate , setCrate )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMEtSums_v1 , int , dataID , setDataID )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMEtSums_v1 , int , peak , setPeak )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMEtSums_v1 , std::vector<unsigned int> , etVec , setEtVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMEtSums_v1 , std::vector<unsigned int> , exVec , setExVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMEtSums_v1 , std::vector<unsigned int> , eyVec , setEyVec )    
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMEtSums_v1 , std::vector<int> , etErrorVec , setEtErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMEtSums_v1 , std::vector<int> , exErrorVec , setExErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMEtSums_v1 , std::vector<int> , eyErrorVec , setEyErrorVec )    
    
  void CMMEtSums_v1::addEt(const std::vector<unsigned int>& Et,const std::vector<int>& EtError)
  {
    setEtVec( Et );
    setEtErrorVec( EtError );
  }
  
  void CMMEtSums_v1::addEx(const std::vector<unsigned int>& Ex,const std::vector<int>& ExError)
  {
    setExVec( Ex );
    setExErrorVec( ExError );    
  }
  
  void CMMEtSums_v1::addEy(const std::vector<unsigned int>& Ey,const std::vector<int>& EyError)
  {
    setEyVec( Ey );
    setEyErrorVec( EyError );    
  }

  unsigned int CMMEtSums_v1::et() const
  {
    return etVec()[ peak() ];
  }
  
  unsigned int CMMEtSums_v1::ex() const
  {
    return exVec()[ peak() ];
  }
  
  unsigned int CMMEtSums_v1::ey() const
  {
    return eyVec()[ peak() ];
  }
  
  int CMMEtSums_v1::etError() const
  {
    return etErrorVec()[ peak() ];
  }
  
  int CMMEtSums_v1::exError() const
  {
    return exErrorVec()[ peak() ];
  }
  
  int CMMEtSums_v1::eyError() const
  {
   return eyErrorVec()[ peak() ]; 
  }
  
} // namespace xAOD

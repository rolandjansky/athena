/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMCPHits_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMCPHits_v1.h"

namespace xAOD{  

  CMMCPHits_v1::CMMCPHits_v1() :
    SG::AuxElement()
  {        
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMCPHits_v1, int, crate, setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMCPHits_v1, int, dataID, setDataID )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMCPHits_v1, int, peak, setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMCPHits_v1, std::vector<unsigned int>, hitsVec0, setHitsVec0 )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMCPHits_v1, std::vector<unsigned int>, hitsVec1, setHitsVec1 )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMCPHits_v1, std::vector<int>, errorVec0, setErrorVec0 )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMMCPHits_v1, std::vector<int>, errorVec1, setErrorVec1 )

  void CMMCPHits_v1::addHits(const std::vector<unsigned int>& hits0,
                             const std::vector<unsigned int>& hits1,
                             const std::vector<int>& error0,
                             const std::vector<int>& error1)
  {
    setHitsVec0( hits0 );
    setHitsVec1( hits1 );
    setErrorVec0( error0 );
    setErrorVec1( error1 );
  }

  unsigned int CMMCPHits_v1::hitWord0() const
  {
    return hitsVec0()[ peak() ]; 
  }

  unsigned int CMMCPHits_v1::hitWord1() const
  {
    return hitsVec1()[ peak() ];
  }

  int CMMCPHits_v1::error0() const
  {
    return errorVec0()[ peak() ]; 
  }

  int CMMCPHits_v1::error1() const
  {
    return errorVec1()[ peak() ];
  }  

} // namespace xAOD

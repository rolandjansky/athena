/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPHits_v1.cxx 687949 2015-08-06 15:48:49Z amazurov $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPHits_v1.h"

namespace xAOD{

  CMXCPHits_v1::CMXCPHits_v1() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXCPHits_v1::initialize(const uint8_t crate,const uint8_t cmx,const uint8_t source) 
  {
    setCrate( crate );
    setCmx( cmx );
    setSourceComponent( source );
  }
  
  /// initialize
  void CMXCPHits_v1::initialize(const uint8_t crate,const uint8_t cmx,const uint8_t source,
                                const std::vector<uint32_t>& hitsVec0,
                                const std::vector<uint32_t>& hitsVec1,
                                const std::vector<uint32_t>& errorVec0,
                                const std::vector<uint32_t>& errorVec1,
                                const uint8_t peak)
  {
    setCrate( crate );
    setCmx( cmx );
    setSourceComponent( source );
    setHitsVec0( hitsVec0 );
    setHitsVec1( hitsVec1 );
    setErrorVec0( errorVec0 );
    setErrorVec1( errorVec1 );
    setPeak( peak );
  } 
      
  /// add hits to existing object  
  void CMXCPHits_v1::addHits(const std::vector<uint32_t>& hitsVec0,
                             const std::vector<uint32_t>& hitsVec1,
                             const std::vector<uint32_t>& errorVec0,
                             const std::vector<uint32_t>& errorVec1)
  {
    setHitsVec0( hitsVec0 );
    setHitsVec1( hitsVec1 );
    setErrorVec0( errorVec0 );
    setErrorVec1( errorVec1 );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPHits_v1 , uint8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPHits_v1 , uint8_t , cmx , setCmx )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPHits_v1 , uint8_t , sourceComponent , setSourceComponent )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPHits_v1 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPHits_v1 , std::vector<uint32_t> , hitsVec0 , setHitsVec0 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPHits_v1 , std::vector<uint32_t> , hitsVec1 , setHitsVec1 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPHits_v1 , std::vector<uint32_t> , errorVec0 , setErrorVec0 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPHits_v1 , std::vector<uint32_t> , errorVec1 , setErrorVec1 )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get hitsVec0 at peak bunch crossing
  uint32_t CMXCPHits_v1::hits0() const
  {
    return hitsVec0()[ peak() ];
  }

  /// get hitsVec1 at peak bunch crossing
  uint32_t CMXCPHits_v1::hits1() const
  {
    return hitsVec1()[ peak() ];
  }

  /// get errorVec0 at peak bunch crossing
  uint32_t CMXCPHits_v1::error0() const
  {
    return errorVec0()[ peak() ];
  }

  /// get errorVec1 at peak bunch crossing
  uint32_t CMXCPHits_v1::error1() const
  {
    return errorVec1()[ peak() ];
  }

} // namespace xAOD


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetHits_v2.h"

namespace xAOD{

  CMXJetHits_v2::CMXJetHits_v2() :
    SG::AuxElement()
  {
  }
  
  /// Alternative constructor
  CMXJetHits_v2::CMXJetHits_v2(const int crate,const int source) :
    SG::AuxElement()
  {
    setCrate( crate );
    setSource( source );
  }
  
  /// Alternative constructor
  CMXJetHits_v2::CMXJetHits_v2(const int crate,const int source,
                               const std::vector<unsigned int>& hitsVec0,
                               const std::vector<unsigned int>& hitsVec1,
                               const std::vector<int>& errorVec0,
                               const std::vector<int>& errorVec1,
                               const uint_least8_t peak) :
    SG::AuxElement()
  {
    setCrate( crate );
    setSource( source );
    setHitsVec0( hitsVec0 );
    setHitsVec1( hitsVec1 );
    setErrorVec0( errorVec0 );
    setErrorVec1( errorVec1 );
    setPeak( peak );
  }
  
  
  /// add data to existing object
  void CMXJetHits_v2::addHits(const std::vector<unsigned int>& hitsVec0,
                              const std::vector<unsigned int>& hitsVec1,
                              const std::vector<int>& errorVec0,
                              const std::vector<int>& errorVec1)
  {
    setHitsVec0( hitsVec0 );
    setHitsVec1( hitsVec1 );
    setErrorVec0( errorVec0 );
    setErrorVec1( errorVec1 );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetHits_v2 , int , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetHits_v2 , int , source , setSource )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetHits_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v2 , std::vector<unsigned int> , hitsVec0 , setHitsVec0 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v2 , std::vector<unsigned int> , hitsVec1 , setHitsVec1 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v2 , std::vector<int> , errorVec0 , setErrorVec0 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v2 , std::vector<int> , errorVec1 , setErrorVec1 )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get hitsVec0 at peak bunch crossing
  unsigned int CMXJetHits_v2::hits0() const
  {
    return hitsVec0()[ peak() ];
  }

  /// get hitsVec1 at peak bunch crossing
  unsigned int CMXJetHits_v2::hits1() const
  {
    return hitsVec1()[ peak() ];
  }

  /// get errorVec0 at peak bunch crossing
  int CMXJetHits_v2::error0() const
  {
    return errorVec0()[ peak() ];
  }

  /// get errorVec1 at peak bunch crossing
  int CMXJetHits_v2::error1() const
  {
    return errorVec1()[ peak() ];
  }

} // namespace xAOD


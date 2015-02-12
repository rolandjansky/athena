/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSums_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSums_v2.h"

namespace xAOD{

  CMXEtSums_v2::CMXEtSums_v2() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXEtSums_v2::initialize(const uint_least8_t crate,const uint_least8_t source)
  {
    setCrate( crate );
    setSource( source );
  }

  /// initialize
  void CMXEtSums_v2::initialize(const uint_least8_t crate,const uint_least8_t source,
                                const std::vector<uint_least16_t>& etVec,
                                const std::vector<uint_least16_t>& exVec,
                                const std::vector<uint_least16_t>& eyVec,
                                const std::vector<uint_least8_t>& etErrorVec,
                                const std::vector<uint_least8_t>& exErrorVec,
                                const std::vector<uint_least8_t>& eyErrorVec,
                                const uint_least8_t peak)
  {
    setCrate( crate );
    setSource( source );
    setEtVec( etVec );
    setExVec( exVec );
    setEyVec( eyVec );
    setEtErrorVec( etErrorVec );
    setExErrorVec( exErrorVec );
    setEyErrorVec( eyErrorVec );
    setPeak( peak );
  }

  
  /// add data to existing object
  /// add Et
  void CMXEtSums_v2::addEt(const std::vector<uint_least16_t>& etVec,const std::vector<uint_least8_t>& etErrorVec)
  {
    setEtVec( etVec );
    setEtErrorVec( etErrorVec );
  }
  
  /// add Ex
  void CMXEtSums_v2::addEx(const std::vector<uint_least16_t>& exVec,const std::vector<uint_least8_t>& exErrorVec)
  {
    setExVec( exVec );
    setExErrorVec( exErrorVec );    
  }
  
  /// add Ey
  void CMXEtSums_v2::addEy(const std::vector<uint_least16_t>& eyVec,const std::vector<uint_least8_t>& eyErrorVec)
  {
    setEyVec( eyVec );
    setEyErrorVec( eyErrorVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v2 , uint_least8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v2 , uint_least8_t , source , setSource )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<uint_least16_t> , etVec , setEtVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<uint_least16_t> , exVec , setExVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<uint_least16_t> , eyVec , setEyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<uint_least8_t> , etErrorVec , setEtErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<uint_least8_t> , exErrorVec , setExErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<uint_least8_t> , eyErrorVec , setEyErrorVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get etVec at peak bunch crossing
  uint_least16_t CMXEtSums_v2::et() const
  {
    return etVec()[ peak() ];
  }

  /// get exVec at peak bunch crossing
  uint_least16_t CMXEtSums_v2::ex() const
  {
    return exVec()[ peak() ];
  }

  /// get eyVec at peak bunch crossing
  uint_least16_t CMXEtSums_v2::ey() const
  {
    return eyVec()[ peak() ];
  }

  /// get etErrorVec at peak bunch crossing
  uint_least8_t CMXEtSums_v2::etError() const
  {
    return etErrorVec()[ peak() ];
  }

  /// get exErrorVec at peak bunch crossing
  uint_least8_t CMXEtSums_v2::exError() const
  {
    return exErrorVec()[ peak() ];
  }

  /// get eyErrorVec at peak bunch crossing
  uint_least8_t CMXEtSums_v2::eyError() const
  {
    return eyErrorVec()[ peak() ];
  }

} // namespace xAOD


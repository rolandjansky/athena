/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXEtSums_v2.h"

namespace xAOD{

  CMXEtSums_v2::CMXEtSums_v2() :
    SG::AuxElement()
  {
  }
  
  /// Alternative constructor
  CMXEtSums_v2::CMXEtSums_v2(const int crate,const int source) :
    SG::AuxElement()
  {
    setCrate( crate );
    setSource( source );
  }

  /// Alternative constructor
  CMXEtSums_v2::CMXEtSums_v2(const int crate,const int source,
                             const std::vector<unsigned int>& etVec,
                             const std::vector<unsigned int>& exVec,
                             const std::vector<unsigned int>& eyVec,
                             const std::vector<int>& etErrorVec,
                             const std::vector<int>& exErrorVec,
                             const std::vector<int>& eyErrorVec,
                             const uint_least8_t peak) :
    SG::AuxElement()
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
  void CMXEtSums_v2::addEt(const std::vector<unsigned int>& etVec,const std::vector<int>& etErrorVec)
  {
    setEtVec( etVec );
    setEtErrorVec( etErrorVec );
  }
  
  /// add Ex
  void CMXEtSums_v2::addEx(const std::vector<unsigned int>& exVec,const std::vector<int>& exErrorVec)
  {
    setExVec( exVec );
    setExErrorVec( exErrorVec );    
  }
  
  /// add Ey
  void CMXEtSums_v2::addEy(const std::vector<unsigned int>& eyVec,const std::vector<int>& eyErrorVec)
  {
    setEyVec( eyVec );
    setEyErrorVec( eyErrorVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v2 , int , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v2 , int , source , setSource )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXEtSums_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<unsigned int> , etVec , setEtVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<unsigned int> , exVec , setExVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<unsigned int> , eyVec , setEyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<int> , etErrorVec , setEtErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<int> , exErrorVec , setExErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXEtSums_v2 , std::vector<int> , eyErrorVec , setEyErrorVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get etVec at peak bunch crossing
  unsigned int CMXEtSums_v2::et() const
  {
    return etVec()[ peak() ];
  }

  /// get exVec at peak bunch crossing
  unsigned int CMXEtSums_v2::ex() const
  {
    return exVec()[ peak() ];
  }

  /// get eyVec at peak bunch crossing
  unsigned int CMXEtSums_v2::ey() const
  {
    return eyVec()[ peak() ];
  }

  /// get etErrorVec at peak bunch crossing
  int CMXEtSums_v2::etError() const
  {
    return etErrorVec()[ peak() ];
  }

  /// get exErrorVec at peak bunch crossing
  int CMXEtSums_v2::exError() const
  {
    return exErrorVec()[ peak() ];
  }

  /// get eyErrorVec at peak bunch crossing
  int CMXEtSums_v2::eyError() const
  {
    return eyErrorVec()[ peak() ];
  }

} // namespace xAOD


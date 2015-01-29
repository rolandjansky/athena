/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPTob_v2.h"

namespace xAOD{

  CMXCPTob_v2::CMXCPTob_v2() :
    SG::AuxElement()
  {
  }
  
  /// Alternative constructor
  CMXCPTob_v2::CMXCPTob_v2(const int crate,const int cmx,const int cpm,const int chip,const int loc) :
    SG::AuxElement()
  {
    setCrate( crate );
    setCmx( cmx );
    setCpm( cpm );
    setChip( chip );
    setLocation( loc );
  }
  
  /// Alternative constructor
  CMXCPTob_v2::CMXCPTob_v2(const int crate,const int cmx,const int cpm,const int chip,const int loc,
                           const std::vector<int>& energyVec,
                           const std::vector<int>& isolationVec,
                           const std::vector<int>& errorVec,
                           const std::vector<unsigned int>& presenceMapVec,
                           const uint_least8_t peak) :
    SG::AuxElement()
  {
    setCrate( crate );
    setCmx( cmx );
    setCpm( cpm );
    setChip( chip );
    setLocation( loc );
    setEnergyVec( energyVec );
    setIsolationVec( isolationVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );
    setPeak( peak );
  }
    
  /// add data to existing object
  void CMXCPTob_v2::addTob(const std::vector<int>& energyVec,
                           const std::vector<int>& isolationVec,
                           const std::vector<int>& errorVec,
                           const std::vector<unsigned int>& presenceMapVec)
  {
    setEnergyVec( energyVec );
    setIsolationVec( isolationVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , int , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , int , cmx , setCmx )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , int , cpm , setCpm )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , int , chip , setChip )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , int , location , setLocation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<int> , energyVec , setEnergyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<int> , isolationVec , setIsolationVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<int> , errorVec , setErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<unsigned int> , presenceMapVec , setPresenceMapVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get energyVec at peak bunch crossing
  int CMXCPTob_v2::energy() const
  {
    return energyVec()[ peak() ];
  }

  /// get isolationVec at peak bunch crossing
  int CMXCPTob_v2::isolation() const
  {
    return isolationVec()[ peak() ];
  }

  /// get errorVec at peak bunch crossing
  int CMXCPTob_v2::error() const
  {
    return errorVec()[ peak() ];
  }

  /// get presenceMapVec at peak bunch crossing
  unsigned int CMXCPTob_v2::presenceMap() const
  {
    return presenceMapVec()[ peak() ];
  }

} // namespace xAOD


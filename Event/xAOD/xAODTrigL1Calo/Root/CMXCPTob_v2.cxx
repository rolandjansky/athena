/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTob_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPTob_v2.h"

namespace xAOD{

  CMXCPTob_v2::CMXCPTob_v2() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXCPTob_v2::initialize(const uint_least8_t crate,const uint_least8_t cmx,
                               const uint_least8_t cpm,const uint_least8_t chip,const uint_least8_t loc)
  {
    setCrate( crate );
    setCmx( cmx );
    setCpm( cpm );
    setChip( chip );
    setLocation( loc );
  }
  
  /// initialize
  void CMXCPTob_v2::initialize(const uint_least8_t crate,const uint_least8_t cmx,
                               const uint_least8_t cpm,const uint_least8_t chip,const uint_least8_t loc,
                               const std::vector<uint_least8_t>& energyVec,
                               const std::vector<uint_least8_t>& isolationVec,
                               const std::vector<uint_least8_t>& errorVec,
                               const std::vector<uint_least16_t>& presenceMapVec,
                               const uint_least8_t peak)
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
  void CMXCPTob_v2::addTob(const std::vector<uint_least8_t>& energyVec,
                           const std::vector<uint_least8_t>& isolationVec,
                           const std::vector<uint_least8_t>& errorVec,
                           const std::vector<uint_least16_t>& presenceMapVec)
  {
    setEnergyVec( energyVec );
    setIsolationVec( isolationVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , cmx , setCmx )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , cpm , setCpm )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , chip , setChip )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , location , setLocation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<uint_least8_t> , energyVec , setEnergyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<uint_least8_t> , isolationVec , setIsolationVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<uint_least8_t> , errorVec , setErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v2 , std::vector<uint_least16_t> , presenceMapVec , setPresenceMapVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get energyVec at peak bunch crossing
  uint_least8_t CMXCPTob_v2::energy() const
  {
    return energyVec()[ peak() ];
  }

  /// get isolationVec at peak bunch crossing
  uint_least8_t CMXCPTob_v2::isolation() const
  {
    return isolationVec()[ peak() ];
  }

  /// get errorVec at peak bunch crossing
  uint_least8_t CMXCPTob_v2::error() const
  {
    return errorVec()[ peak() ];
  }

  /// get presenceMapVec at peak bunch crossing
  uint_least16_t CMXCPTob_v2::presenceMap() const
  {
    return presenceMapVec()[ peak() ];
  }

} // namespace xAOD


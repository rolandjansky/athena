/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTob_v1.cxx 687949 2015-08-06 15:48:49Z amazurov $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXCPTob_v1.h"

namespace xAOD{

  CMXCPTob_v1::CMXCPTob_v1() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXCPTob_v1::initialize(const uint8_t crate,const uint8_t cmx,
                               const uint8_t cpm,const uint8_t chip,const uint8_t loc)
  {
    setCrate( crate );
    setCmx( cmx );
    setCpm( cpm );
    setChip( chip );
    setLocation( loc );
  }
  
  /// initialize
  void CMXCPTob_v1::initialize(const uint8_t crate,const uint8_t cmx,
                               const uint8_t cpm,const uint8_t chip,const uint8_t loc,
                               const std::vector<uint8_t>& energyVec,
                               const std::vector<uint8_t>& isolationVec,
                               const std::vector<uint32_t>& errorVec,
                               const std::vector<uint16_t>& presenceMapVec,
                               const uint8_t peak)
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
  void CMXCPTob_v1::addTob(const std::vector<uint8_t>& energyVec,
                           const std::vector<uint8_t>& isolationVec,
                           const std::vector<uint32_t>& errorVec,
                           const std::vector<uint16_t>& presenceMapVec)
  {
    setEnergyVec( energyVec );
    setIsolationVec( isolationVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v1 , uint8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v1 , uint8_t , cmx , setCmx )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v1 , uint8_t , cpm , setCpm )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v1 , uint8_t , chip , setChip )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v1 , uint8_t , location , setLocation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXCPTob_v1 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v1 , std::vector<uint8_t> , energyVec , setEnergyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v1 , std::vector<uint8_t> , isolationVec , setIsolationVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v1 , std::vector<uint32_t> , errorVec , setErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXCPTob_v1 , std::vector<uint16_t> , presenceMapVec , setPresenceMapVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get energyVec at peak bunch crossing
  uint8_t CMXCPTob_v1::energy() const
  {
    return energyVec()[ peak() ];
  }

  /// get isolationVec at peak bunch crossing
  uint8_t CMXCPTob_v1::isolation() const
  {
    return isolationVec()[ peak() ];
  }

  /// get errorVec at peak bunch crossing
  uint32_t CMXCPTob_v1::error() const
  {
    return errorVec()[ peak() ];
  }

  /// get presenceMapVec at peak bunch crossing
  uint16_t CMXCPTob_v1::presenceMap() const
  {
    return presenceMapVec()[ peak() ];
  }

} // namespace xAOD


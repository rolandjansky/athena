/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTob_v1.cxx 687949 2015-08-06 15:48:49Z amazurov $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetTob_v1.h"

namespace xAOD{

  CMXJetTob_v1::CMXJetTob_v1() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXJetTob_v1::initialize(const uint8_t crate,const uint8_t jem,const uint8_t frame,const uint8_t loc)
  {
    setCrate( crate );
    setJem( jem );
    setFrame( frame );
    setLocation( loc );
  }
  
  /// initialize
  void CMXJetTob_v1::initialize(const uint8_t crate,const uint8_t jem,const uint8_t frame,const uint8_t loc,
                                const std::vector<uint16_t>& energyLargeVec,
                                const std::vector<uint16_t>& energySmallVec,
                                const std::vector<uint32_t>& errorVec,
                                const std::vector<uint16_t>& presenceMapVec,
                                const uint8_t peak)
  {
    setCrate( crate );
    setJem( jem );
    setFrame( frame );
    setLocation( loc );
    setEnergyLargeVec( energyLargeVec );
    setEnergySmallVec( energySmallVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );
    setPeak( peak );
  }
  
  
  /// add data to existing object
  void CMXJetTob_v1::addTob(const std::vector<uint16_t>& energyLargeVec,
                            const std::vector<uint16_t>& energySmallVec,
                            const std::vector<uint32_t>& errorVec,
                            const std::vector<uint16_t>& presenceMapVec)
  {
    setEnergyLargeVec( energyLargeVec );
    setEnergySmallVec( energySmallVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v1 , uint8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v1 , uint8_t , jem , setJem )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v1 , uint8_t , frame , setFrame )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v1 , uint8_t , location , setLocation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v1 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v1 , std::vector<uint16_t> , energyLargeVec , setEnergyLargeVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v1 , std::vector<uint16_t> , energySmallVec , setEnergySmallVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v1 , std::vector<uint32_t> , errorVec , setErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v1 , std::vector<uint16_t> , presenceMapVec , setPresenceMapVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get energyLargeVec at peak bunch crossing
  uint16_t CMXJetTob_v1::energyLarge() const
  {
    return energyLargeVec()[ peak() ];
  }

  /// get energySmallVec at peak bunch crossing
  uint16_t CMXJetTob_v1::energySmall() const
  {
    return energySmallVec()[ peak() ];
  }

  /// get errorVec at peak bunch crossing
  uint32_t CMXJetTob_v1::error() const
  {
    return errorVec()[ peak() ];
  }

  /// get presenceMapVec at peak bunch crossing
  uint16_t CMXJetTob_v1::presenceMap() const
  {
    return presenceMapVec()[ peak() ];
  }

} // namespace xAOD


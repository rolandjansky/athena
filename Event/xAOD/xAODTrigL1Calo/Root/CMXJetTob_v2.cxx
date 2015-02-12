/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTob_v2.cxx 646335 2015-02-12 01:16:10Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetTob_v2.h"

namespace xAOD{

  CMXJetTob_v2::CMXJetTob_v2() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXJetTob_v2::initialize(const uint_least8_t crate,const uint_least8_t jem,const uint_least8_t frame,const uint_least8_t loc)
  {
    setCrate( crate );
    setJem( jem );
    setFrame( frame );
    setLocation( loc );
  }
  
  /// initialize
  void CMXJetTob_v2::initialize(const uint_least8_t crate,const uint_least8_t jem,const uint_least8_t frame,const uint_least8_t loc,
                                const std::vector<uint_least16_t>& energyLargeVec,
                                const std::vector<uint_least16_t>& energySmallVec,
                                const std::vector<uint_least8_t>& errorVec,
                                const std::vector<uint_least16_t>& presenceMapVec,
                                const uint_least8_t peak)
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
  void CMXJetTob_v2::addTob(const std::vector<uint_least16_t>& energyLargeVec,
                            const std::vector<uint_least16_t>& energySmallVec,
                            const std::vector<uint_least8_t>& errorVec,
                            const std::vector<uint_least16_t>& presenceMapVec)
  {
    setEnergyLargeVec( energyLargeVec );
    setEnergySmallVec( energySmallVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , uint_least8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , uint_least8_t , jem , setJem )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , uint_least8_t , frame , setFrame )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , uint_least8_t , location , setLocation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<uint_least16_t> , energyLargeVec , setEnergyLargeVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<uint_least16_t> , energySmallVec , setEnergySmallVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<uint_least8_t> , errorVec , setErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<uint_least16_t> , presenceMapVec , setPresenceMapVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get energyLargeVec at peak bunch crossing
  uint_least16_t CMXJetTob_v2::energyLarge() const
  {
    return energyLargeVec()[ peak() ];
  }

  /// get energySmallVec at peak bunch crossing
  uint_least16_t CMXJetTob_v2::energySmall() const
  {
    return energySmallVec()[ peak() ];
  }

  /// get errorVec at peak bunch crossing
  uint_least8_t CMXJetTob_v2::error() const
  {
    return errorVec()[ peak() ];
  }

  /// get presenceMapVec at peak bunch crossing
  uint_least16_t CMXJetTob_v2::presenceMap() const
  {
    return presenceMapVec()[ peak() ];
  }

} // namespace xAOD


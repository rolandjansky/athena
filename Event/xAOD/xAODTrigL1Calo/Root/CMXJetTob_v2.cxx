/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetTob_v2.h"

namespace xAOD{

  CMXJetTob_v2::CMXJetTob_v2() :
    SG::AuxElement()
  {
  }
  
  /// Alternative constructor
  CMXJetTob_v2::CMXJetTob_v2(const int crate,const int jem,const int frame,const int loc) :
    SG::AuxElement()
  {
    setCrate( crate );
    setJem( jem );
    setFrame( frame );
    setLocation( loc );
  }
  
  /// Alternative constructor
  CMXJetTob_v2::CMXJetTob_v2(const int crate,const int jem,const int frame,const int loc,
                             const std::vector<int>& energyLargeVec,
                             const std::vector<int>& energySmallVec,
                             const std::vector<int>& errorVec,
                             const std::vector<unsigned int>& presenceMapVec,
                             const uint_least8_t peak) :
    SG::AuxElement()
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
  void CMXJetTob_v2::addTob(const std::vector<int>& energyLargeVec,
                            const std::vector<int>& energySmallVec,
                            const std::vector<int>& errorVec,
                            const std::vector<unsigned int>& presenceMapVec)
  {
    setEnergyLargeVec( energyLargeVec );
    setEnergySmallVec( energySmallVec );
    setErrorVec( errorVec );
    setPresenceMapVec( presenceMapVec );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , int , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , int , jem , setJem )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , int , frame , setFrame )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , int , location , setLocation )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetTob_v2 , uint_least8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<int> , energyLargeVec , setEnergyLargeVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<int> , energySmallVec , setEnergySmallVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<int> , errorVec , setErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetTob_v2 , std::vector<unsigned int> , presenceMapVec , setPresenceMapVec )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get energyLargeVec at peak bunch crossing
  int CMXJetTob_v2::energyLarge() const
  {
    return energyLargeVec()[ peak() ];
  }

  /// get energySmallVec at peak bunch crossing
  int CMXJetTob_v2::energySmall() const
  {
    return energySmallVec()[ peak() ];
  }

  /// get errorVec at peak bunch crossing
  int CMXJetTob_v2::error() const
  {
    return errorVec()[ peak() ];
  }

  /// get presenceMapVec at peak bunch crossing
  unsigned int CMXJetTob_v2::presenceMap() const
  {
    return presenceMapVec()[ peak() ];
  }

} // namespace xAOD


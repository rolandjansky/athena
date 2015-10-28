/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoI_v1.cxx 652708 2015-03-09 15:46:47Z krasznaa $

// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/EmTauRoI_v1.h"

namespace xAOD {

   EmTauRoI_v1::EmTauRoI_v1()
      : SG::AuxElement() {

   }

   void EmTauRoI_v1::initialize( uint32_t roiword, float eta, float phi ) {

      setRoIWord( roiword );
      setEta( eta );
      setPhi( phi );

      return;
   }

   /// This function must be used by all analysers to check the type of the RoI
   /// before doing anything further with it. Since the way to use Run 1 and
   /// Run 2 L1Calo RoI words is quite different.
   ///
   /// @returns The "type" of the RoI word in question
   ///
   EmTauRoI_v1::RoIType EmTauRoI_v1::roiType() const {

      const uint32_t roi = roiWord();
      if( ( roi & 0xc0000000 ) == 0 ) {
         return CPRoIWord;
      } else if( ( roi & 0xf0000000 ) == 0xa0000000 ) {
         return EMRoIWord;
      } else if( ( roi & 0xf0000000 ) == 0xb0000000 ) {
         return TauRoIWord;
      }

      throw std::runtime_error( "xAOD::EmTauRoI_v1::roiType() "
                                "Unknown RoI word type found" );
      return static_cast< RoIType >( -1 );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, eta, setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, phi, setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, uint32_t, roiWord,
                                         setRoIWord )

   /// This is a convenience function for accessing the threshold pattern part
   /// of the RoI.
   ///
   uint32_t EmTauRoI_v1::thrPattern() const {

      return ( roiWord() & 0xffff );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, core, setCore )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, emClus, setEmClus )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, tauClus,
                                         setTauClus )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, emIsol, setEmIsol )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, hadIsol,
                                         setHadIsol ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v1, float, hadCore,
                                         setHadCore ) 

   /// Accessor for the names of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< std::string > >
      names( "thrNames" );
   /// Accessor for the values (in MeV) of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< float > >
      values( "thrValues" );

   /// @param name The name of the passed threshold
   /// @param value The value (in MeV) of the passed threshold
   ///
   void EmTauRoI_v1::addThreshold( const std::string& name, float value ) {

      names( *this ).push_back( name );
      values( *this ).push_back( value );

      return;
   }

   /// This function can be used to clear the thresholds that are currently
   /// held by the object.
   ///
   void EmTauRoI_v1::clearThresholds() {

      names( *this ).clear();
      values( *this ).clear();

      return;
   }

   AUXSTORE_OBJECT_GETTER( EmTauRoI_v1, std::vector< std::string >, thrNames )
   AUXSTORE_OBJECT_GETTER( EmTauRoI_v1, std::vector< float >, thrValues )

} // namespace xAOD

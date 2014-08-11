/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoI_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/JetRoI_v1.h"

namespace xAOD {

   JetRoI_v1::JetRoI_v1()
      : SG::AuxElement() {

   }

   void JetRoI_v1::initialize( uint32_t roiword, float eta, float phi ) {

      setRoIWord( roiword );
      setEta( eta );
      setPhi( phi );

      return;
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v1, float, eta, setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v1, float, phi, setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v1, uint32_t, roiWord,
                                         setRoIWord )

   /// This is a convenience function for accessing the threshold pattern part
   /// of the RoI.
   ///
   uint32_t JetRoI_v1::thrPattern() const {

      return ( roiWord() & 0xfff );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v1, float, et4x4, setEt4x4 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v1, float, et6x6, setEt6x6 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v1, float, et8x8, setEt8x8 )

   /// Accessor for the names of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< std::string > >
      names( "thrNames" );
   /// Accessor for the values (in MeV) of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< float > >
      values( "thrValues" );

   /// @param name The name of the passed threshold
   /// @param value The value (in MeV) of the passed threshold
   ///
   void JetRoI_v1::addThreshold( const std::string& name, float value ) {

      names( *this ).push_back( name );
      values( *this ).push_back( value );

      return;
   }

   /// This function can be used to clear the thresholds that are currently
   /// held by the object.
   ///
   void JetRoI_v1::clearThresholds() {

      names( *this ).clear();
      values( *this ).clear();

      return;
   }

   AUXSTORE_OBJECT_GETTER( JetRoI_v1, std::vector< std::string >, thrNames )
   AUXSTORE_OBJECT_GETTER( JetRoI_v1, std::vector< float >, thrValues )

} // namespace xAOD

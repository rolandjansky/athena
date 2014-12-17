/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoI_v2.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/JetRoI_v2.h"

namespace xAOD {

   JetRoI_v2::JetRoI_v2()
      : SG::AuxElement() {

   }

   void JetRoI_v2::initialize( uint32_t roiword, float eta, float phi ) {

      setRoIWord( roiword );
      setEta( eta );
      setPhi( phi );

      return;
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, float, eta, setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, float, phi, setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, uint32_t, roiWord,
                                         setRoIWord )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, float, etScale, setEtScale )
   
   /// Functions for accessing the jet ET part
   /// of the RoI.
   ///
   float JetRoI_v2::etLarge() const {
      if ( (roiWord() & 0xe0000000) == 0 ) {
        if (etScale() == 0) return ( roiWord() & 0x3ff );
        return ( (roiWord() & 0x3ff)*etScale() );
      }  
      return 0;
   }

   float JetRoI_v2::etSmall() const {
      if ( (roiWord() & 0xe0000000) == 0 ) {
        if (etScale() == 0) return ( (roiWord()>>10) & 0x1ff );
        return ( ((roiWord()>>10) & 0x1ff)*etScale() );
      }  
      return 0;
   }

   /// Accessor for threshold pattern (not part of RoI in Run 2)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, uint32_t, thrPattern,
                                         setThrPattern )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, float, et4x4, setEt4x4 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, float, et6x6, setEt6x6 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetRoI_v2, float, et8x8, setEt8x8 )

   /// Accessor for the names of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< std::string > >
      names( "thrNames" );
   /// Accessor for the values (in MeV) of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< float > >
      values( "thrValues" );

   /// @param name The name of the passed threshold
   /// @param value The value (in MeV) of the passed threshold
   ///
   void JetRoI_v2::addThreshold( const std::string& name, float value ) {

      names( *this ).push_back( name );
      values( *this ).push_back( value );

      return;
   }

   /// This function can be used to clear the thresholds that are currently
   /// held by the object.
   ///
   void JetRoI_v2::clearThresholds() {

      names( *this ).clear();
      values( *this ).clear();

      return;
   }

   AUXSTORE_OBJECT_GETTER( JetRoI_v2, std::vector< std::string >, thrNames )
   AUXSTORE_OBJECT_GETTER( JetRoI_v2, std::vector< float >, thrValues )

} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoI_v2.cxx 625821 2014-11-03 17:03:16Z krasznaa $

// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/EmTauRoI_v2.h"

namespace xAOD {

   EmTauRoI_v2::EmTauRoI_v2()
      : SG::AuxElement() {

   }

   void EmTauRoI_v2::initialize( uint32_t roiword, float eta, float phi ) {

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
   EmTauRoI_v2::RoIType EmTauRoI_v2::roiType() const {

      const uint32_t roi = roiWord();
      if( ( roi & 0xc0000000 ) == 0 ) {
         return CPRoIWord;
      } else if( ( roi & 0xf0000000 ) == 0xa0000000 ) {
         return EMRoIWord;
      } else if( ( roi & 0xf0000000 ) == 0xb0000000 ) {
         return TauRoIWord;
      }

      throw std::runtime_error( "xAOD::EmTauRoI_v2::roiType() "
                                "Unknown RoI word type found" );
      return static_cast< RoIType >( -1 );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, eta, setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, phi, setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, uint32_t, roiWord,
                                         setRoIWord )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, etScale, setEtScale )

   /// Accessor for threshold pattern (not part of RoI in Run 2)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, uint32_t, thrPattern,
                                         setThrPattern )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, core, setCore )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, emClus, setEmClus )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, tauClus,
                                         setTauClus )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, emIsol, setEmIsol )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, hadIsol,
                                         setHadIsol ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EmTauRoI_v2, float, hadCore,
                                         setHadCore )
   
   /// Get cluster ET from the RoI word
   float EmTauRoI_v2::eT() const {
      if ( roiType() == EMRoIWord || roiType() == TauRoIWord ) {
        if (etScale() == 0) return ( roiWord() & 0xff );
        return ( (roiWord() & 0xff)*etScale() );
      }  
      return 0;
   }

   /// Get isolation results from the RoI word
   uint8_t EmTauRoI_v2::isol() const {
      if ( roiType() == EMRoIWord || roiType() == TauRoIWord ) 
          return ( (roiWord()>>8) & 0x1f );  
      return 0;
   }

   /// Accessor for the names of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< std::string > >
      names( "thrNames" );
   /// Accessor for the values (in MeV) of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< float > >
      values( "thrValues" );

   /// @param name The name of the passed threshold
   /// @param value The value (in MeV) of the passed threshold
   ///
   void EmTauRoI_v2::addThreshold( const std::string& name, float value ) {

      names( *this ).push_back( name );
      values( *this ).push_back( value );

      return;
   }

   /// This function can be used to clear the thresholds that are currently
   /// held by the object.
   ///
   void EmTauRoI_v2::clearThresholds() {

      names( *this ).clear();
      values( *this ).clear();

      return;
   }

   AUXSTORE_OBJECT_GETTER( EmTauRoI_v2, std::vector< std::string >, thrNames )
   AUXSTORE_OBJECT_GETTER( EmTauRoI_v2, std::vector< float >, thrValues )

} // namespace xAOD

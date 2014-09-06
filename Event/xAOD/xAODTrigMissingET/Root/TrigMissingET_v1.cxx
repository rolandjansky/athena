/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingET_v1.cxx 615412 2014-09-06 12:55:36Z florianb $

// Implement the MissingET trigger object.

// System include(s):
#include <vector>
#include <string>
#include <stdexcept>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigMissingET/versions/TrigMissingET_v1.h"

namespace xAOD {

   // Default ctor
   TrigMissingET_v1::TrigMissingET_v1()
      : SG::AuxElement() {

   }

   // Simple accessor functions for the "default variable" access pattern

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, float, ex, setEx)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, float, ey, setEy)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, float, ez, setEz)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, float, sumEt,
                                         setSumEt)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, float, sumE, setSumE)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, int, flag, setFlag)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigMissingET_v1, uint32_t, roiWord,
                                         setRoiWord)

   //
   // Accessors used by multiple functions:
   //
   static SG::AuxElement::Accessor< std::vector< std::string > >
      nameAcc( "nameOfComponent" );
   static SG::AuxElement::Accessor< std::vector< float > >
      exAcc( "exComponent" );
   static SG::AuxElement::Accessor< std::vector< float > >
      eyAcc( "eyComponent" );


   // Define a component name
   void TrigMissingET_v1::
   defineComponents( const std::vector< std::string >& componentNames ) {

      // Make sure that we aren't killing off something that already exists
      if( nameAcc( *this ).size() > 0) {
         throw std::runtime_error( "Attempt to redefine component list for "
                                   "xAOD::TrigMissingET object" );
      }

      // Assign the component names:
      nameAcc( *this ) = componentNames;

      // Initialise all the variables:
      exAcc( *this ).resize( componentNames.size(), 0.0 );
      eyAcc( *this ).resize( componentNames.size(), 0.0 );

      return;
   }

   // Store the name of the component
   void TrigMissingET_v1::setNameOfComponent( unsigned int index,
                                              const std::string& str ) {

      nameAcc( *this ).at( index ) = str;
      return;
   }

   // Give back the number of components
   unsigned int TrigMissingET_v1::getNumberOfComponents() const {
      return nameAcc( *this ).size();
   }

   const std::string&
   TrigMissingET_v1::nameOfComponent( unsigned int index ) const {

      return nameAcc( *this ).at( index );
   }

   void TrigMissingET_v1::setExComponent( unsigned int index, float val ) {

      exAcc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::exComponent( unsigned int index ) const {

      return exAcc( *this ).at( index );
   }
   
   void TrigMissingET_v1::setEyComponent( unsigned int index, float val ) {

      eyAcc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::eyComponent( unsigned int index ) const {

      return eyAcc( *this ).at( index );
   }   

} // namespace xAOD

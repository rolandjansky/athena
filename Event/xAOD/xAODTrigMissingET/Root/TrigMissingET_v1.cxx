/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


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
   static const SG::AuxElement::Accessor< std::vector< std::string > >
      nameAcc( "nameOfComponent" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      exAcc( "exComponent" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      eyAcc( "eyComponent" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      ezAcc( "ezComponent" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      sumEtAcc( "sumEtComponent" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      sumEAcc( "sumEComponent" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      calib0Acc( "calib0Component" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      calib1Acc( "calib1Component" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      sumOfSignsAcc( "sumOfSignsComponent" );
   static const SG::AuxElement::Accessor< std::vector< short > >
      statusAcc( "statusComponent" );
   static const SG::AuxElement::Accessor< std::vector< unsigned short > >
      usedchannelsAcc( "usedChannelsComponent" );      

   // Define a component name
   void TrigMissingET_v1::
   defineComponents( const std::vector< std::string >& componentNames ) {

      // Make sure that we aren't killing off something that already exists
      if( !nameAcc( *this ).empty()) {
         throw std::runtime_error( "Attempt to redefine component list for "
                                   "xAOD::TrigMissingET object" );
      }

      // Assign the component names:
      nameAcc( *this ) = componentNames;

      // Initialise all the variables:
      exAcc( *this ).resize( componentNames.size(), 0.0 );
      eyAcc( *this ).resize( componentNames.size(), 0.0 );
      ezAcc( *this ).resize( componentNames.size(), 0.0 );
      sumEtAcc( *this ).resize( componentNames.size(), 0.0 );
      sumEAcc( *this ).resize( componentNames.size(), 0.0 );
      calib0Acc( *this ).resize( componentNames.size(), 0.0 );
      calib1Acc( *this ).resize( componentNames.size(), 1.0 );
      sumOfSignsAcc( *this ).resize( componentNames.size(), 1.0 );
      statusAcc( *this ).resize( componentNames.size(), 0 );
      usedchannelsAcc( *this ).resize( componentNames.size(), 1 );
      

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

   void TrigMissingET_v1::setEzComponent( unsigned int index, float val ) {

      ezAcc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::ezComponent( unsigned int index ) const {

      return ezAcc( *this ).at( index );
   }  

   void TrigMissingET_v1::setSumEtComponent( unsigned int index, float val ) {

      sumEtAcc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::sumEtComponent( unsigned int index ) const {

      return sumEtAcc( *this ).at( index );
   }  
   
   void TrigMissingET_v1::setSumEComponent( unsigned int index, float val ) {

      sumEAcc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::sumEComponent( unsigned int index ) const {

      return sumEAcc( *this ).at( index );
   }     

   void TrigMissingET_v1::setCalib0Component( unsigned int index, float val ) {

      calib0Acc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::calib0Component( unsigned int index ) const {

      return calib0Acc( *this ).at( index );
   }   
 
   void TrigMissingET_v1::setCalib1Component( unsigned int index, float val ) {

      calib1Acc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::calib1Component( unsigned int index ) const {

      return calib1Acc( *this ).at( index );
   }     
 
   void TrigMissingET_v1::setSumOfSignsComponent( unsigned int index, float val ) {

      sumOfSignsAcc( *this ).at( index ) = val;
      return;
   }

   float TrigMissingET_v1::sumOfSignsComponent( unsigned int index ) const {

      return sumOfSignsAcc( *this ).at( index );
   } 
   
   void TrigMissingET_v1::setStatusComponent( unsigned int index, float val ) {

      statusAcc( *this ).at( index ) = val;
      return;
   }

   short TrigMissingET_v1::statusComponent( unsigned int index ) const {

      return statusAcc( *this ).at( index );
   }        

   void TrigMissingET_v1::setUsedChannelsComponent( unsigned int index, float val ) {

      usedchannelsAcc( *this ).at( index ) = val;
      return;
   }

   unsigned short TrigMissingET_v1::usedChannelsComponent( unsigned int index ) const {

      return usedchannelsAcc( *this ).at( index );
   }      


} // namespace xAOD

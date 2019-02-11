/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>
#include <stdexcept>

// Local include(s):
#include "xAODEventShape/versions/EventShape_v1.h"

/// Helper macro for managing EventShape Accessor objects
#define DEFINE_ACCESSOR( NAME )                                   \
  case xAOD::EventShape_v1::NAME:                                 \
     {                                                            \
        static const SG::AuxElement::Accessor< float > a( #NAME );      \
        return &a;                                                \
     }                                                            \
     break

namespace {

   /// Helper function for getting the Accessor for a given EventShapeID
   const SG::AuxElement::Accessor< float >*
   eventShapeAccessor( xAOD::EventShape_v1::EventShapeID id ) {

      switch( id ) {
         DEFINE_ACCESSOR( Thrust );
         DEFINE_ACCESSOR( ThrustEta );
         DEFINE_ACCESSOR( ThrustPhi );
         DEFINE_ACCESSOR( ThrustMinor );
         DEFINE_ACCESSOR( Sphericity );
         DEFINE_ACCESSOR( FoxWolfram );
      default:
         break;
      }

      // Complain:
      std::cerr << "eventShapeAccessor ERROR Received unknown "
                << "xAOD::EventShape::EventShapeID (" << id << ")" << std::endl;
      return 0;
   }

   /// Helper function for getting the Accessor for a given EventDensityID
   const SG::AuxElement::Accessor< float >*
   eventDensityAccessor( xAOD::EventShape_v1::EventDensityID id ) {

      switch( id ) {
         DEFINE_ACCESSOR( Density );
         DEFINE_ACCESSOR( DensitySigma );
         DEFINE_ACCESSOR( DensityArea );
      default:
         break;
      }

      // Complain:
      std::cerr << "eventDensityAccessor ERROR Received unknown "
                << "xAOD::EventShape::EventDensityID (" << id << ")"
                << std::endl;
      return 0;
   }

   /// Helper function retrieving a float variable from the EventShape object
   bool getAttribute( const SG::AuxElement::Accessor< float >* acc,
                      const xAOD::EventShape_v1& es, double &v ) {

      if( acc && acc->isAvailable( es ) ) {
         v = ( *acc )( es );
         return true;
      } else {
         return false;
      }
   }

   /// Helper function retrieving a float variable from the EventShape object
   double getAttribute( const SG::AuxElement::Accessor< float >* acc,
                        const xAOD::EventShape_v1& es ) {

      if( acc ) {
         return ( *acc )( es );
      } else {
         throw std::runtime_error( "Asked for unknown xAOD::EventShape "
                                   "property" );
      }
   }

   /// Helper function setting a float variable on the EventShape object
   bool setAttribute( const SG::AuxElement::Accessor< float >* acc,
                      xAOD::EventShape_v1& es, double v ) {

      if( acc ) {
         ( *acc )( es ) = v;
         return true;
      } else {
         return false;
      }
   }

} // private namespace

namespace xAOD {

   EventShape_v1::EventShape_v1()
      : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the shape handling function(s)

   bool EventShape_v1::getShape( EventShapeID id, double& v ) const {

      return getAttribute( eventShapeAccessor( id ), *this, v );
   }

   double EventShape_v1::getShape( EventShapeID id ) const {

      return getAttribute( eventShapeAccessor( id ), *this );
   }

   bool EventShape_v1::setShape( EventShapeID id, double v )  {

      return setAttribute( eventShapeAccessor( id ), *this, v );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the density handling function(s)

   bool EventShape_v1::getDensity( EventDensityID id, double &v ) const {

      return getAttribute( eventDensityAccessor( id ), *this, v );
   }

   double EventShape_v1::getDensity( EventDensityID id ) const {

      return getAttribute( eventDensityAccessor( id ), *this );
   }

   bool EventShape_v1::setDensity( EventDensityID id, double v ) {

      return setAttribute( eventDensityAccessor( id ), *this, v );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

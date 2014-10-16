/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// STL include(s):
#include <iostream>
#include <iomanip>

// Local include(s):
#include "xAODEventShape/versions/EventShape_v1.h"


/// Helper macro for managing EventShape Accessor objects
#define DEFINE_ACCESSOR( NAME )                                   \
  case EventShape_v1::NAME:                                \
  {                                                               \
    static SG::AuxElement::Accessor< float > a( #NAME );          \
    return &a;                                                    \
  }                                                               \
   break
namespace xAOD {


   EventShape_v1::EventShape_v1()
      : SG::AuxElement() {

   }


  SG::AuxElement::Accessor< float > *  eventShapeAccessor(EventShape_v1::EventShapeID id) {
    switch( id ) {
      DEFINE_ACCESSOR( Thrust ) ;
      DEFINE_ACCESSOR(ThrustEta) ;
      DEFINE_ACCESSOR(ThrustPhi) ;
      DEFINE_ACCESSOR(ThrustMinor) ;
      DEFINE_ACCESSOR( Sphericity ) ;
      DEFINE_ACCESSOR( FoxWolfram ) ;
    default:
      break;
    }
    return 0;
  }

  SG::AuxElement::Accessor< float > *  eventDensityAccessor(EventShape_v1::EventDensityID id) {
    switch( id ) {

      DEFINE_ACCESSOR( Density);
      DEFINE_ACCESSOR( DensitySigma);
      DEFINE_ACCESSOR( DensityArea);


    default:
      break;

    }
    return 0;
  }



  namespace {
    // helpers to get/set variables from an Accessor pointer
    // return false if the pointer is null or isAvailable() is false
    bool getAttribute( SG::AuxElement::Accessor< float >* acc, const EventShape_v1& es, double &v){
      if( (acc) && acc->isAvailable(es) )  {
        v= (*acc)( es ); 
        return true;
      }
      return false;
    }

    bool setAttribute( SG::AuxElement::Accessor< float >* acc, EventShape_v1& es, double v){
      if(acc)  {
        (*acc)( es ) = v;    
        return true;
      }
      return false;
    }
  } // anonymous namespace


  // shapes ------------------------------
  bool EventShape_v1::getShape(EventShapeID id, double& v) const {
    return getAttribute( eventShapeAccessor( id ), *this, v);
  }

  bool EventShape_v1::setShape(EventShapeID id, double v)  {
    return setAttribute(eventShapeAccessor( id ), *this, v);
  }


  // densities ------------------------------  
  bool EventShape_v1::getDensity(EventDensityID id, double &v) const {
    return getAttribute( eventDensityAccessor( id ), *this, v);
  }

  bool EventShape_v1::setDensity(EventDensityID id, double v) {
    return setAttribute(eventDensityAccessor( id ), *this, v);
  }


  void EventShape_v1::toPersistent() {

  }  


} // namespace xAOD

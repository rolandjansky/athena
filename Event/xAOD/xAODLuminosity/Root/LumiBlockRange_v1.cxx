/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRange_v1.cxx 647501 2015-02-17 14:22:33Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRange_v1.h"

namespace xAOD {

   LumiBlockRange_v1::LumiBlockRange_v1()
      : SG::AuxElement() {

   }

   LumiBlockRange_v1::LumiBlockRange_v1( const LumiBlockRange_v1& parent )
      : SG::AuxElement(parent) {

      // Create a private store with the parent's payload:
      makePrivateStore( parent );
   }

   LumiBlockRange_v1&
   LumiBlockRange_v1::operator= ( const LumiBlockRange_v1& rhs ) {

      // Protect against self-assignment:
      if( this == &rhs ) {
         return *this;
      }

      // If the object is not part of a container, and it doesn't have a private
      // store, then let's make one now. Otherwise the code would throw an
      // exception at this point.
      if( ( ! container() ) && ( ! hasStore() ) ) {
         makePrivateStore( rhs );
         return *this;
      }

      // If it is connected to some store already, then let's use the assignment
      // operator of the base class to do the heavy lifting:
      SG::AuxElement::operator=( rhs );

      // Return this object:
      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation for the start time functions

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( LumiBlockRange_v1, uint32_t,
                                         startRunNumber, setStartRunNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( LumiBlockRange_v1, uint32_t,
                                         startLumiBlockNumber,
                                         setStartLumiBlockNumber )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation for the stop time functions

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( LumiBlockRange_v1, uint32_t,
                                         stopRunNumber, setStopRunNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( LumiBlockRange_v1, uint32_t,
                                         stopLumiBlockNumber,
                                         setStopLumiBlockNumber )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation for the event number accessors

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( LumiBlockRange_v1, uint32_t,
                                         eventsExpected, setEventsExpected )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( LumiBlockRange_v1, uint32_t,
                                         eventsSeen, setEventsSeen )

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

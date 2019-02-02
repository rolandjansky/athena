//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// xAOD include(s):
#ifdef XAOD_STANDALONE
#   include "xAODRootAccessInterfaces/TActiveEvent.h"
#   include "xAODRootAccess/TEvent.h"
#endif // XAOD_STANDALONE

// Local include(s):
#include "AsgTools/SgTEventMeta.h"

namespace asg {

   SgTEventMeta::SgTEventMeta( StoreType type, xAOD::TEvent* event )
      : m_type( type ), m_event( event ) {

   }

   /// This function is used by the template functions to try to retrieve
   /// a valid pointer to the active TEvent object, if one is not available
   /// yet.
   ///
   /// @returns <code>StatusCode::FAILURE</code> if TEvent can't be found,
   ///          <code>StatusCode::SUCCESS</code> otherwise
   ///
   StatusCode SgTEventMeta::initialize() const {

      // Return right away if we already have a non-null pointer:
      if( m_event ) {
         return StatusCode::SUCCESS;
      }

      // Check if there's an active event:
      xAOD::TVirtualEvent* event = xAOD::TActiveEvent::event();
      if( ! event ) {
         std::cout << META_ERROR_SOURCE << "Couldn't find an active event in "
                   << "the job" << std::endl;
         return StatusCode::FAILURE;
      }

      // This should actually be a TEvent:
      m_event = dynamic_cast< xAOD::TEvent* >( event );
      if( ! m_event ) {
         std::cout << META_ERROR_SOURCE << "The active event is not of type "
                   << "xAOD::TEvent?!?" << std::endl;
         return StatusCode::FAILURE;
      }

      // We succeeded:
      return StatusCode::SUCCESS;
   }

} // namespace asg

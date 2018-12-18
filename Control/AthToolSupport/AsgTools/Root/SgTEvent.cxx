

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/SgTEvent.h"

// RootCore include(s):
#ifdef XAOD_STANDALONE
#   include "xAODRootAccessInterfaces/TActiveEvent.h"
#   include "xAODRootAccess/TActiveStore.h"
#endif // XAOD_STANDALONE

namespace asg {

   SgTEvent::SgTEvent( xAOD::TEvent* pevm, xAOD::TStore* ptds )
      : m_pevm( pevm ), m_ptds( ptds ) {

   }

   xAOD::TEvent* SgTEvent::event() const {

      if( ! m_pevm ) {
         initialize().ignore();
      }

      return m_pevm;
   }

   xAOD::TStore* SgTEvent::tds() const {

      // I'm checking the value of m_pevm on purpose. Since m_ptds may be
      // missing under normal circumstances as well.
      if( ! m_pevm ) {
         initialize().ignore();
      }

      return m_ptds;
   }

   StatusCode SgTEvent::initialize() const {

      // Return right away if we already have a pointer to both stores:
      if( m_pevm && m_ptds ) {
         return StatusCode::SUCCESS;
      }

      // Look for a pointer to the active event if necessary:
      if( ! m_pevm ) {
         // Check if there's an active event:
         xAOD::TVirtualEvent* event = xAOD::TActiveEvent::event();
         if( ! event ) {
            std::cout << ERROR_SOURCE << "Couldn't find an active event in "
                      << "the job" << std::endl;
            return StatusCode::FAILURE;
         }

         // This should actually be a TEvent:
         m_pevm = dynamic_cast< xAOD::TEvent* >( event );
         if( ! m_pevm ) {
            std::cout << ERROR_SOURCE << "The active event is not of type "
                      << "xAOD::TEvent?!?" << std::endl;
            return StatusCode::FAILURE;
         }
      }

      // Look for a pointer to the active store if necessary:
      if( ! m_ptds ) {
         m_ptds = xAOD::TActiveStore::store();
         if( ! m_ptds ) {
            std::cout << "asg::SgTEvent             WARNING "
                      << "No xAOD::TStore object is available" << std::endl;
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace asg

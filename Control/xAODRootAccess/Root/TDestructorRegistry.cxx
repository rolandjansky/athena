// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// ROOT include(s):
#include <TError.h>

// EDM include(s):
#include "AthContainers/normalizedTypeinfoName.h"

// Local include(s):
#include "xAODRootAccess/tools/TDestructorRegistry.h"
#include "xAODRootAccess/tools/Message.h"

namespace xAOD {

   TDestructorRegistry& TDestructorRegistry::instance() {

      static TDestructorRegistry obj;
      return obj;
   }

   const TVirtualDestructor*
   TDestructorRegistry::get( const std::type_info& ti ) const {

      // Get a "read lock":
      std::shared_lock< std::shared_timed_mutex > lock( m_mutex );

      // Look for this type:
      Map_t::const_iterator itr = m_types.find( &ti );
      if( itr != m_types.end() ) {
         return itr->second.get();
      }

      // We didn't find it:
      ::Error( "xAOD::TDestructorRegistry::get",
               XAOD_MESSAGE( "Destructor not known for type %s" ),
               SG::normalizedTypeinfoName( ti ).c_str() );
      return nullptr;
   }

   TDestructorRegistry::TDestructorRegistry()
      : m_types(), m_mutex() {

   }

} // namespace xAOD

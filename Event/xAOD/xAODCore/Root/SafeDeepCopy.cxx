/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SafeDeepCopy.cxx 676241 2015-06-18 06:15:44Z krasznaa $

// EDM include(s):
//#define protected public
#   include "AthContainers/AuxElement.h"
//#undef protected
#include "AthContainers/exceptions.h"
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODCore/tools/SafeDeepCopy.h"

namespace xAOD {

   void safeDeepCopy( const SG::AuxElement& orig, SG::AuxElement& copy ) {

      //
      // Check that the target object is healthy:
      //
      if( ! copy.container() ) {
         return;
      }
      if( ! copy.container()->hasStore() ) {
         return;
      }
      if( ! copy.container()->hasNonConstStore() ) {
         throw SG::ExcConstAuxData( "safeDeepCopy" );
      }

      //
      // Access the original's data:
      //
      const SG::AuxVectorData* ocont = orig.container();
      if( ( ! ocont ) || ( ! ocont->hasStore() ) ) {
         // In this case let the assignment operator of SG::AuxElement take
         // care of clearing out the target object.
         copy = orig;
         return;
      }

      //
      // Do the copy:
      //
      const size_t iindex = orig.index();
      const size_t oindex = copy.index();
      SG::auxid_set_t other_ids = ocont->getAuxIDs();
      SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

      // Copy the variables that exist on the input object:
      for( SG::auxid_t auxid : other_ids ) {
         void* dst = copy.container()->getDataArray( auxid );
         // This is the tricky part. In certain cases an input object/container
         // can claim that it has a certain variable, but still not be able to
         // provide it. Most usually this happens when reading a container from
         // a file, in which not all elements were decorated consistently.
         // This has to be caught here:
         const void* src = nullptr;
         try {
            src = ocont->getDataArray( auxid );
         } catch( const SG::ExcBadAuxVar& ) {
            // In this case just fill dummy values into the output:
            r.clear( auxid, dst, oindex );
            continue;
         }
         // Apparently the copy *can* be done:
         r.copy( auxid, dst, oindex, src, iindex );
      }
      // Clear out the variables that only exist on the output object:
      for( SG::auxid_t auxid : copy.container()->getWritableAuxIDs() ) {
         if( !other_ids.test( auxid ) ) {
            void* dst = copy.container()->getDataArray( auxid );
            r.clear( auxid, dst, oindex );
         }
      }
   }

} // namespace xAOD

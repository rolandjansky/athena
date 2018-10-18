/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <ThreadLocalStorage.h>

// Local include(s):
#include "xAODRootAccess/TActiveStore.h"

namespace xAOD {

   /// Pointer to the currently active TStore object of the thread
   TTHREAD_TLS( TStore* ) s_store;

   TStore* TActiveStore::store() {

      return s_store;
   }

   void TActiveStore::setStore( TStore* ptr ) {

      s_store = ptr;
      return;
   }

} // namespace xAOD

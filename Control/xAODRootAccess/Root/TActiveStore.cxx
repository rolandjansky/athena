/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TActiveStore.cxx 599784 2014-06-01 17:58:48Z krasznaa $

// Local include(s):
#include "xAODRootAccess/TActiveStore.h"

namespace xAOD {

   // Initialize the store pointer:
   TStore* TActiveStore::s_store = 0;

   TStore* TActiveStore::store() {

      return s_store;
   }

} // namespace xAOD

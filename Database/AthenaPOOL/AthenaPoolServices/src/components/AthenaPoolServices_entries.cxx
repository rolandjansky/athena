/** @file AthenaPoolServices_entries.cxx
 *  @brief This file is needed for linker to load all the components of the library.
 *  @author Marcin Nowak
 *  $Id: 
 **/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../AthenaRootStreamerSvc.h"

DECLARE_SERVICE_FACTORY(AthenaRootStreamerSvc)

DECLARE_FACTORY_ENTRIES(AthenaRootStreamerSvc) {
   DECLARE_SERVICE(AthenaRootStreamerSvc)
}



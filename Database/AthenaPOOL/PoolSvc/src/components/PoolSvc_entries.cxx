/** @file PoolSvc_entries.cxx
 *  @brief This file is needed for linker to load all the components of the library.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: PoolSvc_entries.cxx,v 1.8 2007-02-27 16:38:54 gemmeren Exp $
 **/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../PoolSvc.h"

DECLARE_SERVICE_FACTORY(PoolSvc)
DECLARE_FACTORY_ENTRIES(PoolSvc) {
   DECLARE_SERVICE(PoolSvc);
}

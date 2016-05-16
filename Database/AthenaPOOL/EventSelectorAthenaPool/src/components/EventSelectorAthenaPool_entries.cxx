/** @file EventSelectorAthenaPool_entries.cxx
 *  @brief This file is needed for linker to load all the components of the library.
 *  @author Peter van Gemmeren <gemmeren@bnl.gov>
 *  $Id: EventSelectorAthenaPool_entries.cxx,v 1.6 2007-07-25 07:14:28 schaffer Exp $
 **/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../AthenaPoolAddressProviderSvc.h"
#include "../EventSelectorAthenaPool.h"
#include "../CondProxyProvider.h"

DECLARE_SERVICE_FACTORY(AthenaPoolAddressProviderSvc)
DECLARE_SERVICE_FACTORY(EventSelectorAthenaPool)
DECLARE_SERVICE_FACTORY(CondProxyProvider)

DECLARE_FACTORY_ENTRIES(EventSelectorAthenaPool) {
   DECLARE_SERVICE(AthenaPoolAddressProviderSvc)
   DECLARE_SERVICE(EventSelectorAthenaPool)
   DECLARE_SERVICE(CondProxyProvider)
}

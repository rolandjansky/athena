// $Id: DataModelAthenaPool_entries.cxx,v 1.3 2006-04-12 22:14:07 ssnyder Exp $
/**
 * @file  src/components/DataModelAthenaPool_entries.cxx
 * @author scott snyder
 * @date Nov 2005
 * @brief Gaudi algorithm factory declarations.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DataModelAthenaPool/DataModelCompatSvc.h"

DECLARE_SERVICE_FACTORY(DataModelCompatSvc)

DECLARE_FACTORY_ENTRIES(DataModelAthenaPool) {
  DECLARE_SERVICE(DataModelCompatSvc)
}

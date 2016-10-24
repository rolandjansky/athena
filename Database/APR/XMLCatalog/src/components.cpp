/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
  Declare dynamically loaded components
*/

#include "XMLCatalog/XMLFileCatalog.h"
#include "XMLCatalog/XMLMetaDataCatalog.h"

#include "Gaudi/PluginService.h"

DECLARE_FACTORY_WITH_ID( pool::XMLFileCatalog, "xmlcatalog", Gaudi::PluginService::Factory<pool::FCImpl*> )

// typedef to prevent the template with 2 arguments being split into 2 macro arguments
typedef Gaudi::PluginService::Factory<pool::FCMetaImpl*, pool::FCImpl*>  XMLMetaDataCatalogFactory;  
DECLARE_FACTORY_WITH_ID( pool::XMLMetaDataCatalog, "xmlcatalog_meta", XMLMetaDataCatalogFactory )

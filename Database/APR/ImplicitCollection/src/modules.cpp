/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ImplicitCollection.h"

using pool::ImplicitCollection;

#ifdef HAVE_GAUDI_PLUGINSVC
DECLARE_COMPONENT_WITH_ID(ImplicitCollection, "ImplicitCollection")
#else
#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/ICollectionDescription.h"
#include "PersistencySvc/ISession.h"
#include "Reflex/PluginService.h"
#include <string>
// type used to define collection factories in Reflex plugins  
typedef ImplicitCollection ModuleCollectionType;
typedef pool::ICollection* CollectionXtorSignature(const pool::ICollectionDescription* description, pool::ICollection::OpenMode openMode, pool::ISession* session);
PLUGINSVC_FACTORY_WITH_ID(ImplicitCollection, std::string("ImplicitCollection"), CollectionXtorSignature)
#endif

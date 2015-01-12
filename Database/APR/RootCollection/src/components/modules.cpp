#include "../RootCollection.h"

using pool::RootCollection::RootCollection;

#ifdef HAVE_GAUDI_PLUGINSVC
DECLARE_COMPONENT_WITH_ID(RootCollection, "RootCollection")
#else
#include "CollectionBase/CollectionFactory.h"  
#include "CollectionBase/ICollectionDescription.h" 
#include "PersistencySvc/ISession.h"
#include "Reflex/PluginService.h"
#include <string>
// type used to define collection factories in Reflex plugins
typedef  pool::ICollection* CollectionXtorSignature(const pool::ICollectionDescription* description, pool::ICollection::OpenMode openMode, pool::ISession* session);
PLUGINSVC_FACTORY_WITH_ID(RootCollection, std::string("RootCollection"), CollectionXtorSignature)
#endif


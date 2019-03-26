/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database implementation
//--------------------------------------------------------------------
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTSTORAGESVC_ROOTOODB_H
#define POOL_ROOTSTORAGESVC_ROOTOODB_H 1

// Framework include files
#include "StorageSvc/OODatabaseImp.h"

#ifdef HAVE_GAUDI_PLUGINSVC
#include "Gaudi/PluginService.h"
#endif
#include "GAUDI_VERSION.h"

/*
 *  POOL namespace
 */
namespace pool  { 

  /** @class RootOODb RootOODb.h src/RootOODb.h
    *
    * Description:
    * ROOT specific implementation of the POOL database
    * structure. The object allows to create
    * - Abstract domain implementation objects
    * - Abstract database objects (ie. ROOT files)
    * - Abstract container implementations
    *
    * @author  M.Frank
    * @version 1.0
    */
  class RootOODb  : public OODatabaseImp  {
  public:
#ifdef HAVE_GAUDI_PLUGINSVC
  #if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
    typedef Gaudi::PluginService::Factory<IOODatabase*, void*> Factory;
  #else  
    typedef Gaudi::PluginService::Factory1<IOODatabase*, void*> Factory;
  #endif
#endif
    /// Standard Constructor
    RootOODb(void* ctxt);
    /// Standard Constructor
    RootOODb(void* ctxt, DbType typ);
    /// Standard Destructor
    virtual ~RootOODb();
    /// Label of the specific class
    static const char* catalogLabel()  {   return "ROOT_All";       }
    /// Create Root Domain object
    IDbDomain* createDomain();
    /// Create Root Database object (TFile)
    IDbDatabase* createDatabase();
    /// Create Root Container object
    IDbContainer* createContainer(const DbType& typ);
  };
  class RootOOKey : public RootOODb {
  public:
    /// Standard Constructor
    RootOOKey(void* ctxt) : RootOODb(ctxt, ROOTKEY_StorageType)   {   }
    /// Label of the specific class
    static const char* catalogLabel()  {   return "ROOT_Key";         }
  };
  class RootOOTree : public RootOODb {
  public:
    /// Standard Constructor
    RootOOTree(void* ctxt) : RootOODb(ctxt,ROOTTREE_StorageType)   {  }
    /// Label of the specific class
    static const char* catalogLabel()  {   return "ROOT_Tree";        }
  };
  class RootOOTreeIndex : public RootOODb {
  public:
    /// Standard Constructor
    RootOOTreeIndex(void* ctxt) : RootOODb(ctxt,ROOTTREEINDEX_StorageType)   {  }
    /// Label of the specific class
    static const char* catalogLabel()  {   return "ROOT_TreeIndex";        }
  };
}       // end namespace pool

#endif  /* POOL_ROOTSTORAGESVC_ROOTOODB_H  */

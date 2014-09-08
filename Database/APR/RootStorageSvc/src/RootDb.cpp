/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database factories implementation
//--------------------------------------------------------------------
//
//        Package    : POOLCore/RootDb (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#include "RootOODb.h"
#include "StorageSvc/IOODatabase.h"

#ifdef HAVE_GAUDI_PLUGINSVC
// declare types provided by this Storage plugin
DECLARE_COMPONENT_WITH_ID(pool::RootOODb, "ROOT_All")
DECLARE_COMPONENT_WITH_ID(pool::RootOOTree, "ROOT_Tree")
DECLARE_COMPONENT_WITH_ID(pool::RootOOKey, "ROOT_Key")
#else
#include "StorageSvc/DbType.h"
#include "StorageSvc/IOODatabaseFactory.h"

struct ROOT_All : public pool::RootOODb{ ROOT_All(void* c) : RootOODb(c)   {}};
struct ROOT_Tree: public pool::RootOODb{ ROOT_Tree(void* c): RootOODb(c,pool::ROOTTREE_StorageType){}};
struct ROOT_Key : public pool::RootOODb{ ROOT_Key(void* c) : RootOODb(c,pool::ROOTKEY_StorageType) {}};

DECLARE_OODATABASE(ROOT_All)
DECLARE_OODATABASE(ROOT_Tree)
DECLARE_OODATABASE(ROOT_Key)
#endif

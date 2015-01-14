/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_PrioritySetLoader
#define TrigConf_PrioritySetLoader

#include "TrigConfStorage/IPrioritySetLoader.h"
#include "TrigConfStorage/DBLoader.h"


namespace TrigConf {
  
   class PrioritySetLoader : virtual public IPrioritySetLoader, public DBLoader {
   public:

      PrioritySetLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader(sm, session) {}
      virtual ~PrioritySetLoader(){};

      virtual bool load( PrioritySet& data);
   };
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_BunchGroupLoader
#define TrigConf_BunchGroupLoader 

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {

   class BunchGroupLoader : public DBLoader, virtual public IBunchGroupLoader  {
   public:
      BunchGroupLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader("BunchGroupLoader", sm, session) {}
      virtual ~BunchGroupLoader(){};

      virtual bool load( BunchGroup& data);
   };
}

#endif

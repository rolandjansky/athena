/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_CTPConfigLoader
#define TrigConf_CTPConfigLoader 

#include "TrigConfStorage/ICTPConfigLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
  class CTPConfigLoader : virtual public ICTPConfigLoader, public DBLoader {
  public:
     CTPConfigLoader( StorageMgr& sm,  coral::ISessionProxy& session ) : DBLoader("CTPConfigLoader", sm, session) {}
     virtual ~CTPConfigLoader(){};

     virtual bool load( CTPConfig& data);
  };

}

#endif

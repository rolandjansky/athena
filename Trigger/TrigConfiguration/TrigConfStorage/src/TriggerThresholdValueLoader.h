/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TriggerThresholdValueLoader
#define TrigConf_TriggerThresholdValueLoader 

#include "TrigConfStorage/ITriggerThresholdValueLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
   class TriggerThresholdValueLoader : virtual public ITriggerThresholdValueLoader, public DBLoader {
   public:

      TriggerThresholdValueLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader("TriggerThresholdValueLoader", sm, session) {}
      virtual ~TriggerThresholdValueLoader(){};

      virtual bool load( TriggerThresholdValue& data);
   };
}

#endif

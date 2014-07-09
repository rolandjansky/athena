/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IBunchGroupLoader
#define TrigConf_IBunchGroupLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfL1Data/BunchGroup.h"

namespace TrigConf {

   class IBunchGroupLoader : virtual public ILoader {
   public:
      virtual ~IBunchGroupLoader() {};
      virtual bool load( BunchGroup& data ) = 0;
   };

}

#endif

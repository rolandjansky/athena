/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IHLTPrescaleSetCollectionLoader
#define TrigConf_IHLTPrescaleSetCollectionLoader

#include "TrigConfStorage/ILoader.h"

#include <sys/types.h>

namespace TrigConf {

   class HLTPrescaleSetCollection;
  
   class IHLTPrescaleSetCollectionLoader : virtual public ILoader {
   public:
      virtual ~IHLTPrescaleSetCollectionLoader() override = default;
      virtual bool load( HLTPrescaleSetCollection& hltPS, uint requestcounter, const std::string& partition) = 0;
   };

}

#endif

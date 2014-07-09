/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IPrioritySetLoader
#define TrigConf_IPrioritySetLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfL1Data/PrioritySet.h"

namespace TrigConf {

   class IPrioritySetLoader : virtual public ILoader {
   public:
      virtual ~IPrioritySetLoader() {};
      virtual bool load( PrioritySet& data ) = 0;
   };

}

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IPrescaleSetLoader
#define TrigConf_IPrescaleSetLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfL1Data/PrescaleSet.h"

namespace TrigConf {

   class IPrescaleSetLoader : virtual public ILoader {
   public:
      virtual ~IPrescaleSetLoader() {};
      virtual bool load( unsigned int ctpVersion, PrescaleSet& data ) = 0;
   };

}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IHLTPrescaleSetLoader
#define TrigConf_IHLTPrescaleSetLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {

  class HLTPrescaleSet;

  class IHLTPrescaleSetLoader : virtual public ILoader  {
  public:
    virtual ~IHLTPrescaleSetLoader(){};
    virtual bool load(HLTPrescaleSet& hltpss) = 0;
  };

}

#endif

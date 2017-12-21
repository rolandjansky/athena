///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_SGTOOLSDICT_H
#define SGTOOLS_SGTOOLSDICT_H

#include <vector>
#include "SGTools/DataProxy.h"
#include "SGTools/CurrentEventStore.h"

// Need to instantiate iterators
namespace SGToolsDict
{
  struct _tmp {
    std::vector<      SG::DataProxy*> m_sg_data_proxies;
    std::vector<const SG::DataProxy*> m_const_sg_data_proxies;
  };
}

#endif // not SGTOOLS_SGTOOLSDICT_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSICSBASE_DICT_H
#define PHYSICSBASE_DICT_H

#include "TrigCostBase/RunModule.h"
#include "TrigCostBase/DataPair.h"
#include "TrigCostBase/Registry.h"

#ifdef __GCCXML__

namespace Anp
{
  template void Registry::Set<int>        (const std::string &, const int &);
  template void Registry::Set<double>     (const std::string &, const double &);
  template void Registry::Set<std::string>(const std::string &, const std::string &);
  template void Registry::Set<Registry>   (const std::string &, const Registry &);
}

#endif
#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_IJGTOWERMAPPINGMAKER_H
#define TRIGT1CALOFEXPERF_IJGTOWERMAPPINGMAKER_H

#include "GaudiKernel/IAlgTool.h"
#include "JGTowerHelper.h"

namespace LVL1
{
  class IJGTowerMappingMaker : virtual public IAlgTool
  {
  public:
    virtual const std::vector<JGTowerHelper> &getMapping() const = 0;
    DeclareInterfaceID(IJGTowerMappingMaker, 1, 0);
  }; //> end class IJGTowerMappingMaker
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_IJGTOWERMAPPINGMAKER_H
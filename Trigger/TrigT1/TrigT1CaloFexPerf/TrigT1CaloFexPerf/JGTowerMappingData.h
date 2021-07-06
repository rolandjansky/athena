/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERMAPPINGDATA_H
#define TRIGT1CALOFEXPERF_JGTOWERMAPPINGDATA_H

#include <vector>
#include "TrigT1CaloFexPerf/JGTowerHelper.h"

namespace LVL1
{
  using JGTowerMappingData = std::vector<JGTowerHelper>;
}

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF(LVL1::JGTowerMappingData, 135535277, 1)
CONDCONT_DEF(LVL1::JGTowerMappingData, 224134503);

#endif //> !TRIGT1CALOFEXPERF_JGTOWERMAPPINGDATA_H
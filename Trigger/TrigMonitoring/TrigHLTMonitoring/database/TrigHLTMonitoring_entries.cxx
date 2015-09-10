/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigHLTMonitoring/HLTMonTool.h"
#include "TrigHLTMonitoring/MenuAwareDQMonitoring.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HLTMonTool )

DECLARE_FACTORY_ENTRIES(TrigHLTMonitoring) {
DECLARE_ALGTOOL( HLTMonTool )
}

DECLARE_ALGORITHM_FACTORY(MenuAwareDQMonitoring)


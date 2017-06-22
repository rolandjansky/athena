/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../AFPSiClusterTool.h"
#include "../AFPSiClusterSingleHitTool.h"
#include "../AFPSiRowColToLocalCSTool.h"
#include "../AFPSiClusterBasicNearestNeighbour.h"

DECLARE_TOOL_FACTORY(AFPSiClusterTool)
DECLARE_TOOL_FACTORY(AFPSiClusterSingleHitTool)
DECLARE_TOOL_FACTORY(AFPSiRowColToLocalCSTool)
DECLARE_TOOL_FACTORY(AFPSiClusterBasicNearestNeighbour)

DECLARE_FACTORY_ENTRIES(AFP_SiClusterTools) {
}

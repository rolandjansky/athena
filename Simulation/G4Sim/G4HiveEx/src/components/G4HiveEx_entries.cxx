/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../G4InputLoader.h"
#include "../G4InitTool.h"

DECLARE_ALGORITHM_FACTORY(G4InputLoader)
DECLARE_TOOL_FACTORY( G4InitTool )


DECLARE_FACTORY_ENTRIES(G4HiveEx) {
 DECLARE_ALGORITHM(G4InputLoader)
 DECLARE_TOOL(G4InitTool)

}

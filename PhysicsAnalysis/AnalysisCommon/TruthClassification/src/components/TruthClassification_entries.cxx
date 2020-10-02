/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <GaudiKernel/DeclareFactoryEntries.h>

#include "TruthClassification/TruthClassificationTool.h"

DECLARE_TOOL_FACTORY(TruthClassificationTool)
 
DECLARE_FACTORY_ENTRIES(TruthClassification) {
  DECLARE_TOOL(TruthClassificationTool)
}

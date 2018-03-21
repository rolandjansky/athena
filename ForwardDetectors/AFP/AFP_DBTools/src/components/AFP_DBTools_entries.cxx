/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_DBTools/SiLocAlignDBTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY(AFP, SiLocAlignDBTool)

DECLARE_FACTORY_ENTRIES(AFP_DBTools) {
  DECLARE_NAMESPACE_TOOL(AFP, SiLocAlignDBTool)
}

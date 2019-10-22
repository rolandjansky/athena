/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#include <GaudiKernel/DeclareFactoryEntries.h>

#include <CutBookkeeperUtils/OriginalAodCountsTool.h>

DECLARE_TOOL_FACTORY (OriginalAodCountsTool)
DECLARE_FACTORY_ENTRIES(CutBookkeeperUtils) {
  DECLARE_TOOL(OriginalAodCountsTool)
}

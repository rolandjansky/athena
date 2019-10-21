/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#include <GaudiKernel/DeclareFactoryEntries.h>

#include <CutBookkeeperUtils/OriginalAODCountsTool.h>

DECLARE_TOOL_FACTORY (OriginalAODCountsTool)
DECLARE_FACTORY_ENTRIES(CutBookkeeperUtils) {
  DECLARE_TOOL(OriginalAODCountsTool)
}

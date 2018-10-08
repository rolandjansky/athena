/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "BPhysTools/BPhysTrackVertexMapTool.h"
#include "BPhysTools/BPhysBlindingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAOD, BPhysTrackVertexMapTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAOD, BPhysBlindingTool )

DECLARE_FACTORY_ENTRIES( BPhysTools ) {

  DECLARE_NAMESPACE_TOOL( xAOD, BPhysTrackVertexMapTool )
  DECLARE_NAMESPACE_TOOL( xAOD, BPhysBlindingTool )

}

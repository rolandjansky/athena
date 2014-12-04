#include "GaudiKernel/DeclareFactoryEntries.h"

// Top level tool
#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"

// Algs
#include "../TrackVertexAssoTestAlg.h"

using namespace xAOD;

DECLARE_NAMESPACE_TOOL_FACTORY(CP,LooseTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP,TightTrackVertexAssociationTool)
DECLARE_ALGORITHM_FACTORY(TrackVertexAssoTestAlg)
//

DECLARE_FACTORY_ENTRIES(TrackVertexAssociationTool) {
  DECLARE_NAMESPACE_TOOL(CP,LooseTrackVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL(CP,TightTrackVertexAssociationTool)
  // Algs
  DECLARE_ALGORITHM(TrackVertexAssoTestAlg)
}

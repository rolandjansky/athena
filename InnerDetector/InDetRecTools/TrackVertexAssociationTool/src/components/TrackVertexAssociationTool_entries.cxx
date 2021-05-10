#include "GaudiKernel/DeclareFactoryEntries.h"

// Top level tool
#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/BaseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/ElectronTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/MuonTrackVertexAssociationTool.h"

// Algs
#include "../TrackVertexAssoTestAlg.h"

using namespace xAOD;

DECLARE_NAMESPACE_TOOL_FACTORY(CP,TrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP,LooseTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP,TightTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP,BaseTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP,ElectronTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP,MuonTrackVertexAssociationTool)
DECLARE_ALGORITHM_FACTORY(TrackVertexAssoTestAlg)
//

DECLARE_FACTORY_ENTRIES(TrackVertexAssociationTool) {
  DECLARE_NAMESPACE_TOOL(CP,TrackVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL(CP,LooseTrackVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL(CP,TightTrackVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL(CP,BaseTrackVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL(CP,ElectronTrackVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL(CP,MuonTrackVertexAssociationTool)
  // Algs
  DECLARE_ALGORITHM(TrackVertexAssoTestAlg)
}

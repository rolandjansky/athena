#include "TrkVertexFitterValidationTools/BasicVtxValidationNtupleTool.h"
#include "TrkVertexFitterValidationTools/TrueTracksNtupleTool.h"
#include "TrkVertexFitterValidationTools/McEventNtupleTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, BasicVtxValidationNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrueTracksNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, McEventNtupleTool )

DECLARE_FACTORY_ENTRIES(TrkVertexFitterValidationTools)
{ // in this package name these methods are registered
  DECLARE_NAMESPACE_TOOL(Trk, BasicVtxValidationNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, TrueTracksNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, McEventNtupleTool)
}

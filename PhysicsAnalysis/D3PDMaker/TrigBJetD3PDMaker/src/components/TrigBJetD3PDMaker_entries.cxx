#include "../TrigL2BjetFillerTool.h"
#include "../TrigEFBjetFillerTool.h"
#include "../L2BjetToTrackAssociationTool.h"
#include "../EFBjetToTrackAssociationTool.h"
#include "../BjetAnalysis.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigL2BjetFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, TrigEFBjetFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, L2BjetToTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY(D3PD, EFBjetToTrackAssociationTool)
DECLARE_ALGORITHM_FACTORY(BjetAnalysis)

DECLARE_FACTORY_ENTRIES (TrigBJetD3PDMaker) {
  DECLARE_NAMESPACE_TOOL(D3PD, TrigL2BjetFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, TrigEFBjetFillerTool)
  DECLARE_NAMESPACE_TOOL(D3PD, L2BjetToTrackAssociationTool)
  DECLARE_NAMESPACE_TOOL(D3PD, EFBjetToTrackAssociationTool)
  DECLARE_ALGORITHM(BjetAnalysis)
}


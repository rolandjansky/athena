// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "PhotonVertexSelection/PhotonVertexSelectionTool.h"
#include "PhotonVertexSelection/ElectronRemovalTool.h"
#include "PhotonVertexSelection/PhotonPointingTool.h"
#include "../PhotonVertexSelectionAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY(CP, PhotonVertexSelectionTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, ElectronRemovalTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, PhotonPointingTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, PhotonVertexSelectionAlg )

DECLARE_FACTORY_ENTRIES(PhotonVertexSelection) {

   DECLARE_NAMESPACE_TOOL(CP, PhotonVertexSelectionTool)
   DECLARE_NAMESPACE_TOOL(CP, ElectronRemovalTool)
   DECLARE_NAMESPACE_TOOL(CP, PhotonPointingTool)
   DECLARE_NAMESPACE_ALGORITHM( CP, PhotonVertexSelectionAlg )

}

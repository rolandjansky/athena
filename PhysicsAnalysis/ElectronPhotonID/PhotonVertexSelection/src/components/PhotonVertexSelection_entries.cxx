#include "PhotonVertexSelection/PhotonVertexSelectionTool.h"
#include "PhotonVertexSelection/ElectronRemovalTool.h"
#include "PhotonVertexSelection/PhotonPointingTool.h"
#include "../PhotonVertexSelectionAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY(CP, PhotonVertexSelectionTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, ElectronRemovalTool)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, PhotonPointingTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, PhotonVertexSelectionAlg )


#include "GaudiKernel/DeclareFactoryEntries.h"

#include "AthOnnxRuntimeBJT/JSSMLTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(AthONNX, JSSMLTool)

DECLARE_FACTORY_ENTRIES(AthOnnxRuntimeBJT) {
  DECLARE_NAMESPACE_TOOL(AthONNX, JSSMLTool)
}

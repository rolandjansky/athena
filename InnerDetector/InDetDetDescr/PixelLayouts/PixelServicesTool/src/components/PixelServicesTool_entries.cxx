
#include "PixelServicesTool/PixelServicesTool.h"
//#include "PixelServicesTool/InDetServiceBuilderTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(PixelServicesTool)
//DECLARE_TOOL_FACTORY(InDetServiceBuilderTool)

DECLARE_FACTORY_ENTRIES(PixelServicesTool) {
  DECLARE_TOOL(PixelServicesTool)
    //  DECLARE_TOOL(InDetServiceBuilderTool)
}


// DECLARE_NAMESPACE_TOOL_FACTORY(InDet,PixelServicesTool)

// DECLARE_FACTORY_ENTRIES(PixelServicesTool) {
//   DECLARE_NAMESPACE_TOOL  (InDet, PixelServicesTool)
//     }

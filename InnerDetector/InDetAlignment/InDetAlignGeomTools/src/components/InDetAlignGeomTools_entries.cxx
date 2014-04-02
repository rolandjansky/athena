#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetAlignGeomTools/TRTGeometryManagerTool.h"
#include "InDetAlignGeomTools/PixelGeometryManagerTool.h"
#include "InDetAlignGeomTools/SCTGeometryManagerTool.h"
#include "InDetAlignGeomTools/SiGeometryManagerTool.h"
#include "InDetAlignGeomTools/InDetGeometryManagerTool.h"
#include "InDetAlignGeomTools/InDetAlignModuleTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRTGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, PixelGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SCTGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SiGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetAlignModuleTool )

DECLARE_FACTORY_ENTRIES( InDetAlignGeomTools )
{
   DECLARE_NAMESPACE_TOOL( InDet, TRTGeometryManagerTool )
   DECLARE_NAMESPACE_TOOL( InDet, PixelGeometryManagerTool )
   DECLARE_NAMESPACE_TOOL( InDet, SCTGeometryManagerTool )
   DECLARE_NAMESPACE_TOOL( InDet, SiGeometryManagerTool )
   DECLARE_NAMESPACE_TOOL( InDet, InDetGeometryManagerTool )
   DECLARE_NAMESPACE_TOOL( InDet, InDetAlignModuleTool )
}


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrackExtensionTool_xk.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrackExtensionToolCosmics.h"

using namespace InDet;

DECLARE_TOOL_FACTORY(TRT_TrackExtensionTool_xk)
DECLARE_TOOL_FACTORY(TRT_TrackExtensionToolCosmics)

DECLARE_FACTORY_ENTRIES( TRT_TrackExtensionTool_xk )
{
  DECLARE_TOOL( TRT_TrackExtensionTool_xk )
  DECLARE_TOOL( TRT_TrackExtensionToolCosmics )
}






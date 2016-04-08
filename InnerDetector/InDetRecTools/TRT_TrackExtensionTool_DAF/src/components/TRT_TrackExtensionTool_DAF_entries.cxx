
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrackExtensionTool_DAF/TRT_TrackExtensionTool_DAF.h"

//using namespace InDet;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_TrackExtensionTool_DAF )

DECLARE_FACTORY_ENTRIES(TRT_TrackExtensionTool_DAF)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(InDet, TRT_TrackExtensionTool_DAF)
}






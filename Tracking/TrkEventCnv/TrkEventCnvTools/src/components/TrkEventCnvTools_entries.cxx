#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkEventCnvTools/EventCnvSuperTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( Trk, EventCnvSuperTool )

DECLARE_FACTORY_ENTRIES(TrkEventCnvTools)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, EventCnvSuperTool)
}

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../EntryLayerTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , EntryLayerTool  )

DECLARE_FACTORY_ENTRIES( ISF_Geant4Tools ) {
  DECLARE_NAMESPACE_TOOL ( ISF , EntryLayerTool )
}

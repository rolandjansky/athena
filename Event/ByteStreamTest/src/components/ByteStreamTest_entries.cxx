#include "../EvenEventsSelectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(EvenEventsSelectorTool)

DECLARE_FACTORY_ENTRIES(ByteStreamTest) {
   DECLARE_ALGTOOL(EvenEventsSelectorTool);
}

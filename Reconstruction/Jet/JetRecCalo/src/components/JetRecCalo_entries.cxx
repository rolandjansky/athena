// JetRecCalo_entries.cxx

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JetRecCalo/MissingCellListTool.h"

DECLARE_TOOL_FACTORY(MissingCellListTool)

DECLARE_FACTORY_ENTRIES(JetRecCalo) {
  DECLARE_TOOL(MissingCellListTool)
}

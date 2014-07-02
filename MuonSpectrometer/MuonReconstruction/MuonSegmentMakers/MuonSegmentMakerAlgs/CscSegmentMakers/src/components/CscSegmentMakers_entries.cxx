#include "../CscSegmentMaker.h"
#include "../Csc2dSegmentMaker.h"
#include "../Csc4dSegmentMaker.h"
#include "../CscSegmentUtilTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(CscSegmentMaker)
DECLARE_TOOL_FACTORY(Csc2dSegmentMaker)
DECLARE_TOOL_FACTORY(Csc4dSegmentMaker)
DECLARE_TOOL_FACTORY(CscSegmentUtilTool)

DECLARE_FACTORY_ENTRIES(CscSegmentMakers) {
  DECLARE_ALGORITHM(CscSegmentMaker);
  DECLARE_TOOL(Csc2dSegmentMaker);
  DECLARE_TOOL(Csc4dSegmentMaker);
  DECLARE_TOOL(CscSegmentUtilTool);
}

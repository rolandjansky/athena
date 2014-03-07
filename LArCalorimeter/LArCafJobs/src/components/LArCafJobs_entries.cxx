#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArCafJobs/LArShapeDumperTool.h"
#include "LArCafJobs/LArShapeDumper.h"
#include "LArCafJobs/LArSimpleShapeDumper.h"

DECLARE_ALGORITHM_FACTORY(LArShapeDumper)
DECLARE_ALGORITHM_FACTORY(LArSimpleShapeDumper)
DECLARE_TOOL_FACTORY(LArShapeDumperTool)

DECLARE_FACTORY_ENTRIES(LArCafJobs) {
  DECLARE_ALGORITHM(LArShapeDumper)
  DECLARE_ALGORITHM(LArSimpleShapeDumper)
  DECLARE_TOOL(LArShapeDumperTool)
}


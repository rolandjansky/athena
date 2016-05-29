#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArCafJobs/LArShapeDumperTool.h"
#include "LArCafJobs/LArShapeDumper.h"
#include "LArCafJobs/LArSimpleShapeDumper.h"
#include "LArCafJobs/LArNoiseBursts.h"
#include "LArCafJobs/LArHECNoise.h"

DECLARE_ALGORITHM_FACTORY(LArShapeDumper)
DECLARE_ALGORITHM_FACTORY(LArSimpleShapeDumper)
DECLARE_ALGORITHM_FACTORY(LArNoiseBursts)
DECLARE_ALGORITHM_FACTORY(LArHECNoise)
DECLARE_TOOL_FACTORY(LArShapeDumperTool)

DECLARE_FACTORY_ENTRIES(LArCafJobs) {
  DECLARE_ALGORITHM(LArShapeDumper)
  DECLARE_ALGORITHM(LArSimpleShapeDumper)
  DECLARE_ALGORITHM(LArNoiseBursts)
  DECLARE_ALGORITHM(LArHECNoise)
  DECLARE_TOOL(LArShapeDumperTool)
}


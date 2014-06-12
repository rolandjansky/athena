#include "LArMonitoring/LArNoiseBursts.h"
#include "LArMonitoring/LArOddCellsMonTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(LArNoiseBursts)
DECLARE_TOOL_FACTORY(LArOddCellsMonTool)

DECLARE_FACTORY_ENTRIES(LArMonitoring) {
  DECLARE_ALGORITHM(LArNoiseBursts)
  DECLARE_ALGTOOL(LArOddCellsMonTool)
}

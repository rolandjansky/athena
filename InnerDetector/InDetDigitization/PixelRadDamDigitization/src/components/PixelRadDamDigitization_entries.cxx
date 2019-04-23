#include "GaudiKernel/DeclareFactoryEntries.h"

#include "src/PixelDigitization.h"
#include "src/PixelDigitizationTool.h"
#include "src/PixelNoisyCellGenerator.h"
#include "src/PixelChargeSmearer.h"
#include "src/PixelDiodeCrossTalkGenerator.h"
#include "src/PixelRandomDisabledCellGenerator.h"
#include "src/SensorSimPlanarTool.h"
#include "src/SensorSim3DTool.h"
#include "src/ChargeCollProbSvc.h"
#include "src/EnergyDepositionTool.h"
#include "src/RadDamageUtil.h"
#include "src/EfieldInterpolator.h"
#include "src/RD53SimTool.h"
#include "src/FEI4SimTool.h"
#include "src/FEI3SimTool.h"

using namespace RadDam;
DECLARE_ALGORITHM_FACTORY(PixelDigitization)
DECLARE_TOOL_FACTORY(EnergyDepositionTool)
DECLARE_TOOL_FACTORY(RadDamageUtil)
DECLARE_TOOL_FACTORY(EfieldInterpolator)
DECLARE_TOOL_FACTORY(PixelDigitizationTool)
DECLARE_TOOL_FACTORY(PixelNoisyCellGenerator)
DECLARE_TOOL_FACTORY(PixelChargeSmearer)
DECLARE_TOOL_FACTORY(PixelDiodeCrossTalkGenerator)
DECLARE_TOOL_FACTORY(PixelRandomDisabledCellGenerator)
DECLARE_TOOL_FACTORY(SensorSimPlanarTool)
DECLARE_TOOL_FACTORY(SensorSim3DTool)
DECLARE_TOOL_FACTORY(RD53SimTool)
DECLARE_TOOL_FACTORY(FEI4SimTool)
DECLARE_TOOL_FACTORY(FEI3SimTool)
DECLARE_SERVICE_FACTORY(ChargeCollProbSvc)

DECLARE_FACTORY_ENTRIES(PixelDigitization) {
  DECLARE_ALGORITHM(PixelDigitization)
  DECLARE_TOOL(EnergyDepositionTool)
  DECLARE_TOOL(RadDamageUtil)
  DECLARE_TOOL(EfieldInterpolator)
  DECLARE_TOOL(PixelDigitizationTool)
  DECLARE_TOOL(PixelNoisyCellGenerator)
  DECLARE_TOOL(PixelChargeSmearer)
  DECLARE_TOOL(PixelDiodeCrossTalkGenerator)
  DECLARE_TOOL(PixelRandomDisabledCellGenerator)
  DECLARE_TOOL(SensorSimPlanarTool)
  DECLARE_TOOL(SensorSim3DTool)
  DECLARE_TOOL(RD53SimTool)
  DECLARE_TOOL(FEI4SimTool)
  DECLARE_TOOL(FEI3SimTool)
  DECLARE_SERVICE(ChargeCollProbSvc)
}


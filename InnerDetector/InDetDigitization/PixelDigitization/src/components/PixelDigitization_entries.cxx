#include "GaudiKernel/DeclareFactoryEntries.h"

#include "src/PixelDigitization.h"
#include "src/PixelDigitizationTool.h"
#include "src/PixelNoisyCellGenerator.h"
#include "src/PixelGangedMerger.h"
#include "src/PixelChargeSmearer.h"
#include "src/PixelDiodeCrossTalkGenerator.h"
#include "src/PixelRandomDisabledCellGenerator.h"
#include "src/TimeSvc.h"
#include "src/PixelBarrelChargeTool.h"
#include "src/PixelBarrelBichselChargeTool.h"
#include "src/PixelECChargeTool.h"
#include "src/PixelECBichselChargeTool.h"
#include "src/DBMChargeTool.h"
#include "src/IblPlanarChargeTool.h"
#include "src/IblPlanarBichselChargeTool.h"
#include "src/Ibl3DChargeTool.h"
#include "src/Ibl3DBichselChargeTool.h"
#include "src/ChargeCollProbSvc.h"
#include "src/BichselSimTool.h"
#include "src/FEI4SimTool.h"
#include "src/FEI3SimTool.h"

DECLARE_ALGORITHM_FACTORY(PixelDigitization)
DECLARE_TOOL_FACTORY(BichselSimTool)
DECLARE_TOOL_FACTORY(PixelDigitizationTool)
DECLARE_TOOL_FACTORY(PixelNoisyCellGenerator)
DECLARE_TOOL_FACTORY(PixelGangedMerger)
DECLARE_TOOL_FACTORY(PixelChargeSmearer)
DECLARE_TOOL_FACTORY(PixelDiodeCrossTalkGenerator)
DECLARE_TOOL_FACTORY(PixelRandomDisabledCellGenerator)
DECLARE_TOOL_FACTORY(PixelBarrelChargeTool)
DECLARE_TOOL_FACTORY(PixelBarrelBichselChargeTool)
DECLARE_TOOL_FACTORY(PixelECChargeTool)
DECLARE_TOOL_FACTORY(PixelECBichselChargeTool)
DECLARE_TOOL_FACTORY(DBMChargeTool)
DECLARE_TOOL_FACTORY(IblPlanarChargeTool)
DECLARE_TOOL_FACTORY(IblPlanarBichselChargeTool)
DECLARE_TOOL_FACTORY(Ibl3DChargeTool)
DECLARE_TOOL_FACTORY(Ibl3DBichselChargeTool)
DECLARE_TOOL_FACTORY(FEI4SimTool)
DECLARE_TOOL_FACTORY(FEI3SimTool)
//DECLARE_TOOL_FACTORY(SubChargesTool)
DECLARE_SERVICE_FACTORY(ChargeCollProbSvc)
DECLARE_SERVICE_FACTORY(TimeSvc)

DECLARE_FACTORY_ENTRIES(PixelDigitization) {
  DECLARE_ALGORITHM(PixelDigitization)
  DECLARE_TOOL(BichselSimTool)
  DECLARE_TOOL(PixelDigitizationTool)
  DECLARE_TOOL(PixelNoisyCellGenerator)
  DECLARE_TOOL(PixelGangedMerger)
  DECLARE_TOOL(PixelChargeSmearer)
  DECLARE_TOOL(PixelDiodeCrossTalkGenerator)
  DECLARE_TOOL(PixelRandomDisabledCellGenerator)
  DECLARE_TOOL(PixelBarrelChargeTool)
  DECLARE_TOOL(PixelBarrelBichselChargeTool)
  DECLARE_TOOL(PixelECChargeTool)
  DECLARE_TOOL(PixelECBichselChargeTool)
  DECLARE_TOOL(DBMChargeTool)
  DECLARE_TOOL(IblPlanarChargeTool)
  DECLARE_TOOL(IblPlanarBichselChargeTool)
  DECLARE_TOOL(Ibl3DChargeTool)
  DECLARE_TOOL(Ibl3DBichselChargeTool)
  DECLARE_TOOL(FEI4SimTool)
  DECLARE_TOOL(FEI3SimTool)
 // DECLARE_TOOL(SubChargesTool)
  DECLARE_SERVICE(ChargeCollProbSvc)
  DECLARE_SERVICE(TimeSvc)
}


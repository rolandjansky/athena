#include "LArMonTools/LArFEBMon.h"
#include "LArMonTools/LArRawChannelMonTool.h"
#include "LArMonTools/LArCollisionTimeMonTool.h"
#include "LArMonTools/LArCosmicsMonTool.h"
#include "LArMonTools/LArCoverage.h"
#include "LArMonTools/LArDigitMon.h"
#include "LArMonTools/LArRODMonTool.h"
#include "LArMonTools/LArAffectedRegions.h"
#include "LArMonTools/LArHVCorrectionMonTool.h"
#include "LArMonTools/LArNoisyROMon.h"
#include "LArMonTools/LArNoiseCorrelationMon.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(LArFEBMon)
DECLARE_TOOL_FACTORY(LArRawChannelMonTool)
DECLARE_TOOL_FACTORY(LArCollisionTimeMonTool)
  DECLARE_TOOL_FACTORY(LArCosmicsMonTool)
DECLARE_TOOL_FACTORY(LArCoverage)
DECLARE_TOOL_FACTORY(LArDigitMon)
DECLARE_TOOL_FACTORY(LArNoiseCorrelationMon)
DECLARE_TOOL_FACTORY(LArRODMonTool)
DECLARE_TOOL_FACTORY(LArAffectedRegions)
DECLARE_TOOL_FACTORY(LArHVCorrectionMonTool)
DECLARE_TOOL_FACTORY(LArNoisyROMon)
DECLARE_TOOL_FACTORY(LArNoiseCorrelationMon)

DECLARE_FACTORY_ENTRIES(LArMonTools) {
  DECLARE_ALGTOOL(LArFEBMon)
  DECLARE_ALGTOOL(LArRawChannelMonTool)
  DECLARE_ALGTOOL(LArCollisionTimeMonTool)
  DECLARE_ALGTOOL(LArCosmicsMonTool)
  DECLARE_ALGTOOL(LArCoverage)
  DECLARE_ALGTOOL(LArDigitMon)
  DECLARE_ALGTOOL(LArNoiseCorrelationMon)
  DECLARE_ALGTOOL(LArRODMonTool)
  DECLARE_ALGTOOL(LArAffectedRegions)
  DECLARE_ALGTOOL(LArHVCorrectionMonTool)
  DECLARE_ALGTOOL(LArNoisyROMon)
  DECLARE_ALGTOOL(LArNoiseCorrelationMon)
}

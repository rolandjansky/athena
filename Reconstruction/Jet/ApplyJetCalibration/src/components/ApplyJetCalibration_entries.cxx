#include "ApplyJetCalibration/ApplyJetCalibrationAthena.h"
#include "../ApplyJetCalib.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( ApplyJetCalibrationAthena )
DECLARE_TOOL_FACTORY( ApplyJetCalib )

DECLARE_FACTORY_ENTRIES( ApplyJetCalibration) {
  DECLARE_TOOL( ApplyJetCalibrationAthena);
  DECLARE_TOOL( ApplyJetCalib);
}

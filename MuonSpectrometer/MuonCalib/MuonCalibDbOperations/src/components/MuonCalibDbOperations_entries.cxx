#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibDbOperations/CalibrationDbIOTool.h"
#include "MuonCalibDbOperations/CoolInserter.h"
#include "MuonCalibDbOperations/MuonCalibDbCalibrationSource.h"
#include "MuonCalibDbOperations/MuonCalibDefaultCalibrationSource.h"


using namespace MuonCalib;

DECLARE_TOOL_FACTORY ( CalibrationDbIOTool )
DECLARE_TOOL_FACTORY ( MuonCalibDbCalibrationSource )
DECLARE_TOOL_FACTORY ( MuonCalibDefaultCalibrationSource )

DECLARE_FACTORY_ENTRIES(MuonCalibDbOperations) {
  DECLARE_TOOL(CalibrationDbIOTool)
  DECLARE_TOOL(MuonCalibDbCalibrationSource)
  DECLARE_TOOL(MuonCalibDefaultCalibrationSource)
}

DECLARE_ALGORITHM_FACTORY ( CoolInserter )

DECLARE_FACTORY_ENTRIES( CoolInserter ) {
 DECLARE_TOOL(CoolInserter)
}

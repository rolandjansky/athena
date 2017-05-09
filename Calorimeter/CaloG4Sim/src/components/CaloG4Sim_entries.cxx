#include "GaudiKernel/DeclareFactoryEntries.h"

#include "CaloG4Sim/CalibrationDefaultProcessingTool.h"

DECLARE_TOOL_FACTORY( G4UA::CaloG4::CalibrationDefaultProcessingTool )

DECLARE_FACTORY_ENTRIES( CaloG4 ) {
  DECLARE_TOOL( G4UA::CaloG4::CalibrationDefaultProcessingTool )

}

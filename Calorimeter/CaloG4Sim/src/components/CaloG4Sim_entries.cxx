#include "GaudiKernel/DeclareFactoryEntries.h"

#include "CaloG4Sim/CalibrationDefaultProcessing.h"
#include "CaloG4Sim/CalibrationDefaultProcessingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CaloG4 , CalibrationDefaultProcessing )
DECLARE_TOOL_FACTORY( G4UA::CaloG4::CalibrationDefaultProcessingTool )

DECLARE_FACTORY_ENTRIES( CaloG4 ) {
  DECLARE_NAMESPACE_TOOL( CaloG4 , CalibrationDefaultProcessing )
  DECLARE_TOOL( G4UA::CaloG4::CalibrationDefaultProcessingTool )

}

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "CaloG4Sim/CalibrationDefaultProcessing.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CaloG4 , CalibrationDefaultProcessing )

DECLARE_FACTORY_ENTRIES( CaloG4 ) {
  DECLARE_NAMESPACE_TOOL( CaloG4 , CalibrationDefaultProcessing )

}

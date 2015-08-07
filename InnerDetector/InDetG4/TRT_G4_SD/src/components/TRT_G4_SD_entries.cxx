#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TRTSensitiveDetectorTool.h"

DECLARE_TOOL_FACTORY( TRTSensitiveDetectorTool )

DECLARE_FACTORY_ENTRIES( TRT_G4_SD ) {
  DECLARE_TOOL( TRTSensitiveDetectorTool )
}

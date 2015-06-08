#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../CSCSensitiveDetectorTool.h"
#include "../CSCSensitiveDetectorCosmicsTool.h"
#include "../GenericMuonSensitiveDetectorTool.h"
#include "../MDTSensitiveDetectorTool.h"
#include "../MDTSensitiveDetectorCosmicsTool.h"
#include "../MicromegasSensitiveDetectorTool.h"
#include "../RPCSensitiveDetectorTool.h"
#include "../RPCSensitiveDetectorCosmicsTool.h"
#include "../TGCSensitiveDetectorTool.h"
#include "../TGCSensitiveDetectorCosmicsTool.h"
#include "../sTGCSensitiveDetectorTool.h"

DECLARE_TOOL_FACTORY( CSCSensitiveDetectorTool )
DECLARE_TOOL_FACTORY( CSCSensitiveDetectorCosmicsTool )
DECLARE_TOOL_FACTORY( GenericMuonSensitiveDetectorTool )
DECLARE_TOOL_FACTORY( MDTSensitiveDetectorTool )
DECLARE_TOOL_FACTORY( MDTSensitiveDetectorCosmicsTool )
DECLARE_TOOL_FACTORY( MicromegasSensitiveDetectorTool )
DECLARE_TOOL_FACTORY( RPCSensitiveDetectorTool )
DECLARE_TOOL_FACTORY( RPCSensitiveDetectorCosmicsTool )
DECLARE_TOOL_FACTORY( TGCSensitiveDetectorTool )
DECLARE_TOOL_FACTORY( TGCSensitiveDetectorCosmicsTool )
DECLARE_TOOL_FACTORY( sTGCSensitiveDetectorTool )

DECLARE_FACTORY_ENTRIES( MuonG4SD ) {
  DECLARE_TOOL( CSCSensitiveDetectorTool )
  DECLARE_TOOL( CSCSensitiveDetectorCosmicsTool )
  DECLARE_TOOL( GenericMuonSensitiveDetectorTool )
  DECLARE_TOOL( MDTSensitiveDetectorTool )
  DECLARE_TOOL( MDTSensitiveDetectorCosmicsTool )
  DECLARE_TOOL( MicromegasSensitiveDetectorTool )
  DECLARE_TOOL( RPCSensitiveDetectorTool )
  DECLARE_TOOL( RPCSensitiveDetectorCosmicsTool )
  DECLARE_TOOL( TGCSensitiveDetectorTool )
  DECLARE_TOOL( TGCSensitiveDetectorCosmicsTool )
  DECLARE_TOOL( sTGCSensitiveDetectorTool )
}

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4AtlasTools/PhysicsListToolBase.h"
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/UserActionBase.h"
#include "G4AtlasTools/CylindricalEnvelope.h"
#include "G4AtlasTools/PolyconicalEnvelope.h"
#include "G4AtlasTools/EnvelopeTool.h"
#include "G4AtlasTools/G4AtlasDetectorConstructionTool.h"
#include "G4AtlasTools/GeoDetectorTool.h"
#include "G4AtlasTools/RegionCreator.h"
#include "G4AtlasTools/AddPhysicsDecayTool.h"
#include "../SensitiveDetectorMasterTool.h"
#include "../FastSimulationMasterTool.h"

DECLARE_TOOL_FACTORY( PhysicsListToolBase )
DECLARE_TOOL_FACTORY( DetectorGeometryBase )
DECLARE_TOOL_FACTORY( UserActionBase )
DECLARE_TOOL_FACTORY( CylindricalEnvelope )
DECLARE_TOOL_FACTORY( PolyconicalEnvelope )
DECLARE_TOOL_FACTORY( EnvelopeTool )
DECLARE_TOOL_FACTORY( G4AtlasDetectorConstructionTool )
DECLARE_TOOL_FACTORY( GeoDetectorTool )
DECLARE_TOOL_FACTORY( RegionCreator )
DECLARE_TOOL_FACTORY( SensitiveDetectorMasterTool )
DECLARE_TOOL_FACTORY( FastSimulationMasterTool )
DECLARE_TOOL_FACTORY( AddPhysicsDecayTool )

DECLARE_FACTORY_ENTRIES( G4AtlasTools ) {
  DECLARE_TOOL( PhysicsListToolBase )
  DECLARE_TOOL( DetectorGeometryBase )
  DECLARE_TOOL( UserActionBase )
  DECLARE_TOOL( CylindricalEnvelope )
  DECLARE_TOOL( PolyconicalEnvelope )
  DECLARE_TOOL( EnvelopeTool )
  DECLARE_TOOL( G4AtlasDetectorConstructionTool )
  DECLARE_TOOL( GeoDetectorTool )
  DECLARE_TOOL( RegionCreator )
  DECLARE_TOOL( SensitiveDetectorMasterTool )
  DECLARE_TOOL( FastSimulationMasterTool )
  DECLARE_TOOL( AddPhysicsDecayTool )
}

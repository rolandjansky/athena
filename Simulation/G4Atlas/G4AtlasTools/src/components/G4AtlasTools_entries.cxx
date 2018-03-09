#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4AtlasTools/PhysicsListToolBase.h"
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/CylindricalEnvelope.h"
#include "G4AtlasTools/BoxEnvelope.h"
#include "G4AtlasTools/PolyconicalEnvelope.h"
#include "G4AtlasTools/EnvelopeTool.h"
#include "G4AtlasTools/G4AtlasDetectorConstructionTool.h"
#include "G4AtlasTools/RegionCreator.h"
#include "../UserLimitsTool.h"
#include "G4AtlasTools/AddPhysicsDecayTool.h"
#include "../SensitiveDetectorMasterTool.h"
#include "../FastSimulationMasterTool.h"
#include "G4AtlasTools/GlobalFieldManagerTool.h"
#include "G4AtlasTools/DetectorFieldManagerTool.h"
#include "../MaterialDescriptionTool.h"

DECLARE_TOOL_FACTORY( PhysicsListToolBase )
DECLARE_TOOL_FACTORY( DetectorGeometryBase )
DECLARE_TOOL_FACTORY( CylindricalEnvelope )
DECLARE_TOOL_FACTORY( BoxEnvelope )
DECLARE_TOOL_FACTORY( PolyconicalEnvelope )
DECLARE_TOOL_FACTORY( EnvelopeTool )
DECLARE_TOOL_FACTORY( G4AtlasDetectorConstructionTool )
DECLARE_TOOL_FACTORY( RegionCreator )
DECLARE_TOOL_FACTORY( UserLimitsTool )
DECLARE_TOOL_FACTORY( SensitiveDetectorMasterTool )
DECLARE_TOOL_FACTORY( FastSimulationMasterTool )
DECLARE_TOOL_FACTORY( AddPhysicsDecayTool )
DECLARE_TOOL_FACTORY( GlobalFieldManagerTool )
DECLARE_TOOL_FACTORY( DetectorFieldManagerTool )
DECLARE_TOOL_FACTORY( MaterialDescriptionTool )

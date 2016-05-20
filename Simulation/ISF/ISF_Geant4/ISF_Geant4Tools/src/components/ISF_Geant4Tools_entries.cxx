#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TransportTool.h"
#include "../TrackProcessorUserActionPassBack.h"
#include "../TrackProcessorUserActionPassBackTool.h"
#include "../TrackProcessorUserActionFullG4.h"
#include "../TrackProcessorUserActionFullG4Tool.h"
#include "../MCTruthUserAction.h"
#include "../MCTruthUserActionTool.h"
#include "../PhysicsValidationUserAction.h"
#include "../PhysicsValidationUserActionTool.h"
#include "../G4RunManagerHelper.h"

//From Gaudi v25r3 onwards it is very important that the
//G4TransportTool is first in the list of tools to be created when
//arranged alphabetically, as this now determines the order in which
//genConf is run.
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4       , G4TransportTool            )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4       , TrackProcessorUserActionPassBack )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA::iGeant4 , TrackProcessorUserActionPassBackTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4       , TrackProcessorUserActionFullG4   )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA::iGeant4 , TrackProcessorUserActionFullG4Tool   )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4       , MCTruthUserAction        )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA::iGeant4 , MCTruthUserActionTool        )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4       , PhysicsValidationUserAction        )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA::iGeant4 , PhysicsValidationUserActionTool        )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4       , G4RunManagerHelper )

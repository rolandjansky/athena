#include "../TransportTool.h"
#include "../TrackProcessorUserActionPassBackTool.h"
#include "../TrackProcessorUserActionFullG4Tool.h"
#include "../MCTruthUserActionTool.h"
#include "../PhysicsValidationUserActionTool.h"
#include "../G4RunManagerHelper.h"

//From Gaudi v25r3 onwards it is very important that the
//G4TransportTool is first in the list of tools to be created when
//arranged alphabetically, as this now determines the order in which
//genConf is run.
DECLARE_COMPONENT( iGeant4::G4TransportTool )
DECLARE_COMPONENT( G4UA::iGeant4::TrackProcessorUserActionPassBackTool )
DECLARE_COMPONENT( G4UA::iGeant4::TrackProcessorUserActionFullG4Tool )
DECLARE_COMPONENT( G4UA::iGeant4::MCTruthUserActionTool )
DECLARE_COMPONENT( G4UA::iGeant4::PhysicsValidationUserActionTool )
DECLARE_COMPONENT( iGeant4::G4RunManagerHelper )


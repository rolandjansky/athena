#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkG4UserActions/GeantFollowerHelper.h"
#include "TrkG4UserActions/GeantFollowerMSHelper.h"
#include "TrkG4UserActions/MaterialStepRecorder.h"
#include "TrkG4UserActions/EnergyLossRecorder.h"
#include "TrkG4UserActions/GeantFollower.h"
#include "TrkG4UserActions/GeantFollowerMS.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk , GeantFollowerHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk , GeantFollowerMSHelper )
DECLARE_TOOL_FACTORY( MaterialStepRecorder )
DECLARE_TOOL_FACTORY( EnergyLossRecorder )
DECLARE_TOOL_FACTORY( GeantFollower )
DECLARE_TOOL_FACTORY( GeantFollowerMS )

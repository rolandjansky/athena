#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../LArTBH6TriggerTimeTool.h"
#include "../LArTBH6BeamInfo.h"
#include "../LArGeoH62004EventAction.h"
#include "../LArGeoH62004SteppingAction.h"
#include "../RadLenNtuple.h"
// SDs
#include "../LArG4H62004ActiveSDTool.h"
#include "../LArG4H62004DeadSDTool.h"
#include "../LArG4H62004EMECSDTool.h"
#include "../LArG4H62004FCALSDTool.h"
#include "../LArG4H62004HECSDTool.h"
#include "../LArG4H62004InactiveSDTool.h"
#include "../LArG4H6BeamSDTool.h"
#include "../LArG4H6WarmTCSDTool.h"
#include "../LArG4TBECBeamChSDTool.h"
#include "../TBECBackOuterBarretteTool.h"
#include "../TBECInnerModuleTool.h"
#include "../TBECOuterModuleTool.h"

// UA tools
DECLARE_TOOL_FACTORY( LArTBH6TriggerTimeTool )
DECLARE_TOOL_FACTORY( LArGeoH62004EventAction )
DECLARE_TOOL_FACTORY( LArGeoH62004SteppingAction )
DECLARE_TOOL_FACTORY( RadLenNtuple )

// SD tools
DECLARE_TOOL_FACTORY( LArG4H62004ActiveSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004DeadSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004EMECSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004FCALSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004HECSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004InactiveSDTool )
DECLARE_TOOL_FACTORY( LArG4H6BeamSDTool )
DECLARE_TOOL_FACTORY( LArG4H6WarmTCSDTool )
DECLARE_TOOL_FACTORY( LArG4TBECBeamChSDTool )
DECLARE_TOOL_FACTORY( TBECBackOuterBarretteTool )
DECLARE_TOOL_FACTORY( TBECInnerModuleTool )
DECLARE_TOOL_FACTORY( TBECOuterModuleTool )

DECLARE_ALGORITHM_FACTORY(LArTBH6BeamInfo)

DECLARE_FACTORY_ENTRIES(LArG4TBH6Tools) {
   DECLARE_ALGORITHM(LArTBH6BeamInfo)
   DECLARE_TOOL( LArTBH6TriggerTimeTool )
   DECLARE_TOOL( LArGeoH62004EventAction )
   DECLARE_TOOL( LArGeoH62004SteppingAction  )
   DECLARE_TOOL( RadLenNtuple )
   DECLARE_TOOL( LArG4H62004ActiveSDTool )
   DECLARE_TOOL( LArG4H62004DeadSDTool )
   DECLARE_TOOL( LArG4H62004EMECSDTool )
   DECLARE_TOOL( LArG4H62004FCALSDTool )
   DECLARE_TOOL( LArG4H62004HECSDTool )
   DECLARE_TOOL( LArG4H62004InactiveSDTool )
   DECLARE_TOOL( LArG4H6BeamSDTool )
   DECLARE_TOOL( LArG4H6WarmTCSDTool )
   DECLARE_TOOL( LArG4TBECBeamChSDTool )
   DECLARE_TOOL( TBECBackOuterBarretteTool )
   DECLARE_TOOL( TBECInnerModuleTool )
   DECLARE_TOOL( TBECOuterModuleTool )
}


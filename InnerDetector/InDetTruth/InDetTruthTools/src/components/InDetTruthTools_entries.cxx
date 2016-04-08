#include "InDetTruthTools/PRD_MultiTruthBuilder.h"
#include "InDetTruthTools/PRD_TruthTrajectorySorterID.h"
#include "InDetTruthTools/PRD_TruthTrajectoryManipulatorID.h"
#include "InDetTruthTools/PRD_TruthTrajectorySelectorID.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY(InDet, PRD_MultiTruthBuilder)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, PRD_TruthTrajectorySorterID)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, PRD_TruthTrajectoryManipulatorID)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, PRD_TruthTrajectorySelectorID)

DECLARE_FACTORY_ENTRIES(InDetTruthTools) {
DECLARE_NAMESPACE_TOOL(InDet, PRD_MultiTruthBuilder)
DECLARE_NAMESPACE_TOOL(InDet, PRD_TruthTrajectorySorterID)
DECLARE_NAMESPACE_TOOL(InDet, PRD_TruthTrajectoryManipulatorID)
DECLARE_NAMESPACE_TOOL(InDet, PRD_TruthTrajectorySelectorID)
}

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigFTKFastSimTruth/TrigFTKFastSimTruth.h"
#include "TrigFTKFastSimTruth/TrigFTKSectorMatchTool.h"
#include "TrigFTKFastSimTruth/TrigFTKTrackBuilderTool.h"


DECLARE_ALGORITHM_FACTORY( TrigFTKFastSimTruth )

DECLARE_TOOL_FACTORY(TrigFTKSectorMatchTool)
DECLARE_TOOL_FACTORY(TrigFTKTrackBuilderTool)


DECLARE_FACTORY_ENTRIES( TrigFTKFastSimTruth )
{
    DECLARE_ALGORITHM( TrigFTKFastSimTruth );

    DECLARE_TOOL(TrigFTKSectorMatchTool)
    DECLARE_TOOL(TrigFTKTrackBuilderTool)
}

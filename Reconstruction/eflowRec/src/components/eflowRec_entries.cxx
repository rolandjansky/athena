#include "eflowRec/eflowBuilder.h"
#include "eflowRec/eflowPreparation.h"
#include "eflowRec/eflowCaloObjectBuilder.h"
#include "eflowRec/eflowObjectBuilder.h"
#include "eflowRec/eflowCellLevelSubtractionTool.h"
#include "eflowRec/eflowRecoverSplitShowersTool.h"
#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowMomentCalculatorTool.h"
#include "eflowRec/eflowClusterCollectionTool.h"
#include "eflowRec/eflowTrackCaloExtensionTool.h"
#include "eflowRec/eflowTrackCaloDummyExtensionTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowCellEOverPTool_mc12_JetETMiss.h"
#include "eflowRec/eflowCellEOverPTool_mc12_LC.h"
#include "eflowRec/eflowObjectCreatorTool.h"
#include "eflowRec/eflowOverlapRemoval.h"
#include "eflowRec/eflowVertexInformationSetter.h"
#include "eflowRec/PFLeptonSelector.h"
#include "eflowRec/PFTrackSelector.h"
#include "eflowRec/PFClusterSelector.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( eflowBuilder )
DECLARE_ALGORITHM_FACTORY( eflowPreparation )
DECLARE_ALGORITHM_FACTORY( eflowCaloObjectBuilder )
DECLARE_ALGORITHM_FACTORY( eflowObjectBuilder )
DECLARE_ALGORITHM_FACTORY( eflowOverlapRemoval )
DECLARE_ALGORITHM_FACTORY( eflowVertexInformationSetter )
DECLARE_ALGORITHM_FACTORY( PFLeptonSelector )
DECLARE_ALGORITHM_FACTORY( PFClusterSelector )
DECLARE_ALGORITHM_FACTORY( PFTrackSelector )
DECLARE_TOOL_FACTORY( eflowRecoverSplitShowersTool )
DECLARE_TOOL_FACTORY( eflowCellLevelSubtractionTool )
DECLARE_TOOL_FACTORY( eflowLCCalibTool )
DECLARE_TOOL_FACTORY( eflowMomentCalculatorTool )
DECLARE_TOOL_FACTORY( eflowClusterCollectionTool )
DECLARE_TOOL_FACTORY( eflowTrackCaloExtensionTool )
DECLARE_TOOL_FACTORY( eflowTrackCaloDummyExtensionTool )
DECLARE_TOOL_FACTORY( PFTrackClusterMatchingTool )
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12_JetETMiss)
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12_LC)
DECLARE_TOOL_FACTORY( eflowObjectCreatorTool )

DECLARE_FACTORY_ENTRIES(eflowRec) {
  DECLARE_ALGORITHM( eflowBuilder )
    DECLARE_ALGORITHM( eflowPreparation )
    DECLARE_ALGORITHM( eflowCaloObjectBuilder )
    DECLARE_ALGORITHM( eflowObjectBuilder )
    DECLARE_ALGORITHM (eflowOverlapRemoval )
    DECLARE_ALGORITHM ( eflowVertexInformationSetter )
    DECLARE_ALGORITHM( PFLeptonSelector )
    DECLARE_ALGORITHM( PFClusterSelector )
    DECLARE_ALGORITHM( PFTrackSelector )
    DECLARE_TOOL ( eflowRecoverSplitShowersTool )
    DECLARE_TOOL ( eflowCellLevelSubtractionTool )
    DECLARE_TOOL ( eflowMomentCalculatorTool )
    DECLARE_TOOL ( eflowClusterCollectionTool )
    DECLARE_TOOL ( eflowLCCalibTool )
    DECLARE_TOOL ( eflowTrackCaloExtensionTool )
    DECLARE_TOOL ( eflowTrackCaloDummyExtensionTool )
    DECLARE_TOOL ( PFTrackClusterMatchingTool )
    DECLARE_TOOL (eflowCellEOverPTool_mc12_JetETMiss)
    DECLARE_TOOL ( eflowCellEOverPTool_mc12_LC)
    DECLARE_TOOL (eflowObjectCreatorTool )
}

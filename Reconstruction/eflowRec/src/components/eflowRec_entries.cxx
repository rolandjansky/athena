#include "eflowRec/eflowTrackCaloExtensionTool.h"
#include "eflowRec/eflowTrackCaloDummyExtensionTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowCellEOverPTool_mc12_JetETMiss.h"
#include "eflowRec/eflowCellEOverPTool_mc12_LC.h"
#include "eflowRec/eflowOverlapRemoval.h"
#include "eflowRec/PFLeptonSelector.h"
#include "eflowRec/PFTrackSelector.h"
#include "eflowRec/PFClusterSelector.h"
#include "eflowRec/PFAlgorithm.h"
#include "eflowRec/PFCellLevelSubtractionTool.h"
#include "eflowRec/PFRecoverSplitShowersTool.h"
#include "eflowRec/PFMomentCalculatorTool.h"
#include "eflowRec/PFClusterCollectionTool.h"
#include "eflowRec/PFLCCalibTool.h"
#include "eflowRec/PFOChargedCreatorAlgorithm.h"
#include "eflowRec/PFONeutralCreatorAlgorithm.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( eflowOverlapRemoval )
DECLARE_ALGORITHM_FACTORY( PFLeptonSelector )
DECLARE_ALGORITHM_FACTORY( PFClusterSelector )
DECLARE_ALGORITHM_FACTORY( PFTrackSelector )
DECLARE_ALGORITHM_FACTORY( PFAlgorithm )
DECLARE_ALGORITHM_FACTORY( PFOChargedCreatorAlgorithm )
DECLARE_ALGORITHM_FACTORY( PFONeutralCreatorAlgorithm )
DECLARE_TOOL_FACTORY( PFCellLevelSubtractionTool )
DECLARE_TOOL_FACTORY( PFRecoverSplitShowersTool )
DECLARE_TOOL_FACTORY( PFMomentCalculatorTool )
DECLARE_TOOL_FACTORY( PFClusterCollectionTool )
DECLARE_TOOL_FACTORY( PFLCCalibTool )
DECLARE_TOOL_FACTORY( eflowTrackCaloExtensionTool )
DECLARE_TOOL_FACTORY( eflowTrackCaloDummyExtensionTool )
DECLARE_TOOL_FACTORY( PFTrackClusterMatchingTool )
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12_JetETMiss)
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12_LC)

DECLARE_FACTORY_ENTRIES(eflowRec) {
    DECLARE_ALGORITHM (eflowOverlapRemoval )
    DECLARE_ALGORITHM( PFLeptonSelector )
    DECLARE_ALGORITHM( PFClusterSelector )
    DECLARE_ALGORITHM( PFTrackSelector )
    DECLARE_ALGORITHM( PFAlgorithm )
    DECLARE_ALGORITHM( PFOChargedCreatorAlgorithm )
    DECLARE_ALGORITHM( PFONeutralCreatorAlgorithm )
    DECLARE_TOOL( PFCellLevelSubtractionTool )
    DECLARE_TOOL( PFRecoverSplitShowersTool )
    DECLARE_TOOL( PFMomentCalculatorTool )
    DECLARE_TOOL( PFClusterCollectionTool )
    DECLARE_TOOL( PFLCCalibTool )
    DECLARE_TOOL ( eflowTrackCaloExtensionTool )
    DECLARE_TOOL ( eflowTrackCaloDummyExtensionTool )
    DECLARE_TOOL ( PFTrackClusterMatchingTool )
    DECLARE_TOOL (eflowCellEOverPTool_mc12_JetETMiss)
    DECLARE_TOOL ( eflowCellEOverPTool_mc12_LC)
}

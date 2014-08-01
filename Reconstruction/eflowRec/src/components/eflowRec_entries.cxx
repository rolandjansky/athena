#include "eflowRec/eflowBuilder.h"
#include "eflowRec/eflowCaloObjectBuilder.h"
#include "eflowRec/eflowObjectBuilder_Tools.h"
#include "eflowRec/eflowTauProcessingTools.h"
#include "eflowRec/eflowCellLevelSubtractionTool.h"
#include "eflowRec/eflowTauTool.h"
#include "eflowRec/eflowTauPi0TaggerTool.h"
#include "eflowRec/eflowTauLCCalibTool.h"
#include "eflowRec/eflowRecoverSplitShowersTool.h"
#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowMomentCalculatorTool.h"
#include "eflowRec/eflowClusterCollectionTool.h"
#include "eflowRec/eflowTrackToCaloTrackExtrapolatorTool.h"
#include "eflowRec/eflowCellEOverPTool.h"
#include "eflowRec/eflowCellEOverPTool_mc12.h"
#include "eflowRec/eflowCellEOverPTool_mc12_JetETMiss.h"
#include "eflowRec/eflowCellEOverPTool_mc12_LC.h"
#include "eflowRec/eflowObjectCreatorTool.h"
#include "eflowRec/eflowOverlapRemoval.h"
#include "eflowRec/eflowVertexInformationSetter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( eflowBuilder )
DECLARE_ALGORITHM_FACTORY( eflowCaloObjectBuilder )
DECLARE_ALGORITHM_FACTORY( eflowObjectBuilder_Tools )
DECLARE_ALGORITHM_FACTORY( eflowTauProcessingTools )
DECLARE_ALGORITHM_FACTORY( eflowOverlapRemoval )
DECLARE_ALGORITHM_FACTORY( eflowVertexInformationSetter )
DECLARE_TOOL_FACTORY( eflowRecoverSplitShowersTool )
DECLARE_TOOL_FACTORY( eflowCellLevelSubtractionTool )
DECLARE_TOOL_FACTORY( eflowTauTool )
DECLARE_TOOL_FACTORY( eflowTauPi0TaggerTool )
DECLARE_TOOL_FACTORY( eflowTauLCCalibTool )
DECLARE_TOOL_FACTORY( eflowLCCalibTool )
DECLARE_TOOL_FACTORY( eflowMomentCalculatorTool )
DECLARE_TOOL_FACTORY( eflowClusterCollectionTool )
DECLARE_TOOL_FACTORY( eflowTrackToCaloTrackExtrapolatorTool )
DECLARE_TOOL_FACTORY( eflowCellEOverPTool)
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12)
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12_JetETMiss)
DECLARE_TOOL_FACTORY( eflowCellEOverPTool_mc12_LC)
DECLARE_TOOL_FACTORY( eflowObjectCreatorTool )

DECLARE_FACTORY_ENTRIES(eflowRec) {
  DECLARE_ALGORITHM( eflowBuilder )
    DECLARE_ALGORITHM( eflowCaloObjectBuilder )
    DECLARE_ALGORITHM( eflowObjectBuilder_Tools )
    DECLARE_ALGORITHM( eflowTauProcessingTools )
     DECLARE_ALGORITHM (eflowOverlapRemoval )
    DECLARE_ALGORITHM ( eflowVertexInformationSetter )
    DECLARE_TOOL ( eflowRecoverSplitShowersTool )
    DECLARE_TOOL ( eflowCellLevelSubtractionTool )
    DECLARE_TOOL ( eflowTauTool )
    DECLARE_TOOL ( eflowTauPi0TaggerTool )
    DECLARE_TOOL ( eflowTauLCCalibTool )
    DECLARE_TOOL ( eflowMomentCalculatorTool )
    DECLARE_TOOL ( eflowClusterCollectionTool )
    DECLARE_TOOL ( eflowLCCalibTool )
    DECLARE_TOOL (eflowTrackToCaloTrackExtrapolatorTool )
    DECLARE_TOOL (eflowCellEOverPTool)
    DECLARE_TOOL (eflowCellEOverPTool_mc12)
    DECLARE_TOOL (eflowCellEOverPTool_mc12_JetETMiss)
    DECLARE_TOOL ( eflowCellEOverPTool_mc12_LC)
    DECLARE_TOOL (eflowObjectCreatorTool )
}

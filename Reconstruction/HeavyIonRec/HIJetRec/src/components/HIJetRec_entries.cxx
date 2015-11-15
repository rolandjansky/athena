
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../HIJetCellSubtractorTool.h"
#include "../HIClusterMaker.h"
#include "../HIClusterSubtraction.h"
#include "../HISubtractedCellMakerTool.h"
#include "HIJetRec/HIEventShapeJetIteration.h"
#include "HIJetRec/HIJetClusterSubtractorTool.h"
#include "HIJetRec/HIJetConstituentSubtractionTool.h"
#include "HIJetRec/HIJetDRAssociationTool.h"
#include "HIJetRec/HIJetMaxOverMeanTool.h"
#include "HIJetRec/HIJetDiscriminatorTool.h"
#include "HIJetRec/HIJetSignificanceTool.h"
#include "HIJetRec/HIUEModulatorTool.h"


DECLARE_ALGORITHM_FACTORY( HIClusterMaker )
DECLARE_TOOL_FACTORY( HIClusterSubtraction )
DECLARE_TOOL_FACTORY( HIEventShapeJetIteration )
DECLARE_TOOL_FACTORY(HIJetConstituentSubtractionTool)
DECLARE_TOOL_FACTORY(HIJetCellSubtractorTool)
DECLARE_TOOL_FACTORY(HIJetClusterSubtractorTool)
DECLARE_TOOL_FACTORY(HIJetDRAssociationTool)
DECLARE_TOOL_FACTORY(HIJetMaxOverMeanTool)
DECLARE_TOOL_FACTORY(HIJetDiscriminatorTool)
DECLARE_TOOL_FACTORY(HIJetSignificanceTool)
DECLARE_TOOL_FACTORY(HISubtractedCellMakerTool)
DECLARE_TOOL_FACTORY(HIUEModulatorTool)

DECLARE_FACTORY_ENTRIES( HIJetRec ) {
  DECLARE_ALGORITHM( HIClusterMaker )
    DECLARE_TOOL( HIClusterSubtraction )
    DECLARE_TOOL( HIEventShapeJetIteration )
    DECLARE_TOOL(HIJetConstituentSubtractionTool)
    DECLARE_TOOL(HIJetCellSubtractorTool)
    DECLARE_TOOL(HIJetClusterSubtractorTool)
    DECLARE_TOOL(HIJetDRAssociationTool)
    DECLARE_TOOL(HIJetMaxOverMeanTool)
    DECLARE_TOOL(HIJetDiscriminatorTool)
    DECLARE_TOOL(HISubtractedCellMakerTool)
    DECLARE_TOOL(HIUEModulatorTool)
}

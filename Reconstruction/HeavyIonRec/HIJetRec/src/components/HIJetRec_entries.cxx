
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HIJetRec/HIJetConstituentSubtractionTool.h"
#include "HIJetRec/HIJetCellSubtractorTool.h"
#include "HIJetRec/HIJetDRAssociationTool.h"
#include "HIJetRec/HIJetMaxOverMeanTool.h"
#include "HIJetRec/HIJetDiscriminatorTool.h"
#include "HIJetRec/HIClusterMaker.h"
#include "HIJetRec/HIClusterSubtraction.h"
#include "HIJetRec/HIEventShapeJetIteration.h"

DECLARE_ALGORITHM_FACTORY( HIClusterMaker )
DECLARE_ALGORITHM_FACTORY( HIClusterSubtraction )
DECLARE_ALGORITHM_FACTORY( HIEventShapeJetIteration )
DECLARE_TOOL_FACTORY(HIJetConstituentSubtractionTool)
DECLARE_TOOL_FACTORY(HIJetCellSubtractorTool)
DECLARE_TOOL_FACTORY(HIJetDRAssociationTool)
DECLARE_TOOL_FACTORY(HIJetMaxOverMeanTool)
DECLARE_TOOL_FACTORY(HIJetDiscriminatorTool)

DECLARE_FACTORY_ENTRIES( HIJetRec ) {
  DECLARE_ALGORITHM( HIClusterMaker )
    DECLARE_ALGORITHM( HIClusterSubtraction )
    DECLARE_ALGORITHM( HIEventShapeJetIteration )
    DECLARE_TOOL(HIJetConstituentSubtractionTool)
    DECLARE_TOOL(HIJetCellSubtractorTool)
    DECLARE_TOOL(HIJetDRAssociationTool)
    DECLARE_TOOL(HIJetMaxOverMeanTool)
    DECLARE_TOOL(HIJetDiscriminatorTool)
}

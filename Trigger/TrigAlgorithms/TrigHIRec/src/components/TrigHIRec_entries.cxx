#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigHIClusterMaker.h"
#include "../TrigHIEventShapeMaker.h"
#include "../TrigHIEventShapeJetIteration.h"

DECLARE_ALGORITHM_FACTORY( TrigHIClusterMaker )
DECLARE_ALGORITHM_FACTORY( TrigHIEventShapeMaker )
DECLARE_TOOL_FACTORY( TrigHIEventShapeJetIteration )

DECLARE_FACTORY_ENTRIES( TrigHIHypo ) {
    DECLARE_ALGORITHM( TrigHIClusterMaker );
    DECLARE_ALGORITHM( TrigHIEventShapeMaker );
    DECLARE_TOOL( TrigHIEventShapeJetIteration );
}


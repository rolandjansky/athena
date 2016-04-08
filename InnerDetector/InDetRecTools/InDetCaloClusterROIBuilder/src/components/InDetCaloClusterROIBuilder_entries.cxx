#include "../CaloClusterROI_Builder.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_TOOL_FACTORY (CaloClusterROI_Builder)

DECLARE_FACTORY_ENTRIES(InDetCaloClusterROIBuilder) {
    DECLARE_TOOL     ( CaloClusterROI_Builder )
}

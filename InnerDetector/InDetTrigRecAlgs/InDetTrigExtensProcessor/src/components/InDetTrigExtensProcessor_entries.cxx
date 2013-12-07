#include "InDetTrigExtensProcessor/InDetTrigExtensProcessor.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetTrigExtensProcessor )

DECLARE_FACTORY_ENTRIES(InDetTrigExtensProcessor) {
    DECLARE_NAMESPACE_ALGORITHM( InDet, InDetTrigExtensProcessor)
}





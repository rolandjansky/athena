#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetEtaDependentCutsTool/InDetEtaDependentCutsTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetEtaDependentCutsTool )

DECLARE_FACTORY_ENTRIES( InDetEtaDependentCutsTool )
{
    DECLARE_NAMESPACE_TOOL( InDet, InDetEtaDependentCutsTool )
}

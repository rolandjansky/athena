#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetEtaDependentCuts/InDetEtaDependentCutsSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( InDet, InDetEtaDependentCutsSvc )

DECLARE_FACTORY_ENTRIES( InDetEtaDependentCuts )
{
    DECLARE_NAMESPACE_SERVICE( InDet, InDetEtaDependentCutsSvc )
}

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../FPEAuditor.h"

DECLARE_FACTORY_ENTRIES(AthenaAuditors) {
    DECLARE_AUDITOR(FPEAuditor)
}
DECLARE_AUDITOR_FACTORY(FPEAuditor)

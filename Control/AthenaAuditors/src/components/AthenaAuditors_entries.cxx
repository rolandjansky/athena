#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../FPEAuditor.h"
#include "../CoWAuditor.h"
#include "../AthMemoryAuditor.h"

DECLARE_FACTORY_ENTRIES(AthenaAuditors) {
    DECLARE_AUDITOR(FPEAuditor)
    DECLARE_AUDITOR(CoWAuditor)
    DECLARE_AUDITOR(AthMemoryAuditor)
}
DECLARE_AUDITOR_FACTORY(FPEAuditor)
DECLARE_AUDITOR_FACTORY(CoWAuditor)
DECLARE_AUDITOR_FACTORY(AthMemoryAuditor)

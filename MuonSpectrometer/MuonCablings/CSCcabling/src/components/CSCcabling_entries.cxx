#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CSCcabling/CSCcablingSvc.h"

DECLARE_SERVICE_FACTORY(CSCcablingSvc)

DECLARE_FACTORY_ENTRIES(CSCcabling) {
    DECLARE_SERVICE( CSCcablingSvc )
}

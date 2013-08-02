#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TGCcabling/TGCcablingSvc.h"

DECLARE_SERVICE_FACTORY( TGCcablingSvc )

DECLARE_FACTORY_ENTRIES(TGCcabling) {
    DECLARE_SERVICE( TGCcablingSvc )
}

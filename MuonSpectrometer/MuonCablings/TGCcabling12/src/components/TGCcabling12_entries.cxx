#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TGCcabling12/TGCcabling12Svc.h"

DECLARE_SERVICE_FACTORY( TGCcabling12Svc )

DECLARE_FACTORY_ENTRIES(TGCcabling12) {
    DECLARE_SERVICE( TGCcabling12Svc );
}

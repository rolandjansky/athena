#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"

DECLARE_SERVICE_FACTORY( MuonTGC_CablingSvc )

DECLARE_FACTORY_ENTRIES(MuonTGC_Cabling) {
    DECLARE_SERVICE( MuonTGC_CablingSvc );
}

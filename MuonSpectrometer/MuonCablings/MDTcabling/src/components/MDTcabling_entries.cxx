#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MDTcabling/MDTcablingSvc.h"
#include "MuonCondInterface/IMDTCablingDbTool.h"

DECLARE_SERVICE_FACTORY(MDTcablingSvc)

DECLARE_FACTORY_ENTRIES(MDTcabling) {
    DECLARE_SERVICE( MDTcablingSvc )
}

#include "AFP_LocRecoEv/AFP_SIDLocRecoEvent.h"
#include "AFP_LocRecoEv/AFP_TDLocRecoEvent.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(AFP_SIDLocRecoEvent)
DECLARE_ALGORITHM_FACTORY(AFP_TDLocRecoEvent)

DECLARE_FACTORY_ENTRIES(AFP_LocRecoEv) {
DECLARE_ALGORITHM(AFP_SIDLocRecoEvent)
DECLARE_ALGORITHM(AFP_TDLocRecoEvent)
}

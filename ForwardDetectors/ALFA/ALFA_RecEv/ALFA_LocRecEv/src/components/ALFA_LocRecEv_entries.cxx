#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"
#include "ALFA_LocRecEv/ALFA_LocRecODEvent.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(ALFA_LocRecEvent)
DECLARE_ALGORITHM_FACTORY(ALFA_LocRecODEvent)

DECLARE_FACTORY_ENTRIES(ALFA_LocRecEv) {
DECLARE_ALGORITHM(ALFA_LocRecEvent)
DECLARE_ALGORITHM(ALFA_LocRecODEvent)
}

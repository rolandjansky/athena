#include "ALFA_GloRecEv/ALFA_GloRecEvent.h"
#include "ALFA_GloRecEv/ALFA_GloRecODEvent.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(ALFA_GloRecEvent)
DECLARE_ALGORITHM_FACTORY(ALFA_GloRecODEvent)

DECLARE_FACTORY_ENTRIES(ALFA_GloRecEv) {
DECLARE_ALGORITHM(ALFA_GloRecEvent)
DECLARE_ALGORITHM(ALFA_GloRecODEvent)
}

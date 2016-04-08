#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvent.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(ALFA_LocRecCorrEvent)
DECLARE_ALGORITHM_FACTORY(ALFA_LocRecCorrODEvent)

DECLARE_FACTORY_ENTRIES(ALFA_LocRecCorrEv) {
DECLARE_ALGORITHM(ALFA_LocRecCorrEvent)
DECLARE_ALGORITHM(ALFA_LocRecCorrODEvent)
}

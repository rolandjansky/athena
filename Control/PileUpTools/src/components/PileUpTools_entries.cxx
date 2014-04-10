#include "GaudiKernel/DeclareFactoryEntries.h"

#include "PileUpTools/PileUpMergeSvc.h"

DECLARE_FACTORY_ENTRIES(PileUpTools) {
    DECLARE_SERVICE( PileUpMergeSvc )
}
DECLARE_SERVICE_FACTORY(PileUpMergeSvc) 

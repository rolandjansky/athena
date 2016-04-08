#include "GaudiKernel/DeclareFactoryEntries.h"
#include "src/DetDescrDBEnvelopeSvc.h"

DECLARE_SERVICE_FACTORY( DetDescrDBEnvelopeSvc )

DECLARE_FACTORY_ENTRIES( SubDetectorEnvelopes ) {
  DECLARE_SERVICE( DetDescrDBEnvelopeSvc )
}


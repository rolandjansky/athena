#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SubDetectorEnvelopes/DetDescrDBEnvelopeSvc.h"

DECLARE_SERVICE_FACTORY( DetDescrDBEnvelopeSvc )

DECLARE_FACTORY_ENTRIES( SubDetectorEnvelopes ) {
  DECLARE_SERVICE( DetDescrDBEnvelopeSvc )
}


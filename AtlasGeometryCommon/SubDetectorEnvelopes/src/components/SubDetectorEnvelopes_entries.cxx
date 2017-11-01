#include "GaudiKernel/DeclareFactoryEntries.h"
#include "src/DetDescrDBEnvelopeSvc.h"

DECLARE_COMPONENT( DetDescrDBEnvelopeSvc )

DECLARE_FACTORY_ENTRIES( SubDetectorEnvelopes ) {
  DECLARE_SERVICE( DetDescrDBEnvelopeSvc )
}


//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ByteStreamEmonSvc/ByteStreamEmonInputSvc.h"

DECLARE_SERVICE_FACTORY( ByteStreamEmonInputSvc )

DECLARE_FACTORY_ENTRIES( ByteStreamEmonSvc ) {
    DECLARE_SERVICE    ( ByteStreamEmonInputSvc )
}

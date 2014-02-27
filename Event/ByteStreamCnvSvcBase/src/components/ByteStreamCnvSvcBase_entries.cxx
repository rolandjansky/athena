//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddressProviderSvc.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

DECLARE_SERVICE_FACTORY( ByteStreamCnvSvcBase )
DECLARE_SERVICE_FACTORY( ByteStreamAddressProviderSvc )
DECLARE_SERVICE_FACTORY( ROBDataProviderSvc )

DECLARE_FACTORY_ENTRIES( ByteStreamCnvSvcBase ) {
    DECLARE_SERVICE    ( ByteStreamCnvSvcBase )
    DECLARE_SERVICE    ( ByteStreamAddressProviderSvc )
    DECLARE_SERVICE    ( ROBDataProviderSvc )
}

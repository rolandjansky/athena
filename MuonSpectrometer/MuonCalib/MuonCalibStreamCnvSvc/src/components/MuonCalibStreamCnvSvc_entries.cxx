#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamTestAlg.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamCnvSvc.h"
#include "MuonCalibStreamCnvSvc/EventSelectorMuonCalibStream.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamFileInputSvc.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddressProviderSvc.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamDataProviderSvc.h"
#include "MuonCalibStreamCnvSvc/MdtPRDCollCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/MdtPRDContCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/RpcRDOContCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/TgcRDOContCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/CscRDOContCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/EventInfoMuonCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/MuonCalibRunLumiBlockCoolSvc.h"

DECLARE_ALGORITHM_FACTORY( MuonCalibStreamTestAlg )
DECLARE_SERVICE_FACTORY( MuonCalibStreamCnvSvc )
DECLARE_SERVICE_FACTORY( EventSelectorMuonCalibStream )
DECLARE_SERVICE_FACTORY( MuonCalibStreamFileInputSvc )
DECLARE_SERVICE_FACTORY( MuonCalibStreamAddressProviderSvc )
DECLARE_SERVICE_FACTORY( MuonCalibStreamDataProviderSvc )
DECLARE_SERVICE_FACTORY( MuonCalibRunLumiBlockCoolSvc )
DECLARE_CONVERTER_FACTORY( MdtPRDCollCalibStreamCnv )
DECLARE_CONVERTER_FACTORY( MdtPRDContCalibStreamCnv )
DECLARE_CONVERTER_FACTORY( RpcRDOContCalibStreamCnv )
DECLARE_CONVERTER_FACTORY( TgcRDOContCalibStreamCnv )
DECLARE_CONVERTER_FACTORY( CscRDOContCalibStreamCnv )
DECLARE_CONVERTER_FACTORY( EventInfoMuonCalibStreamCnv )

DECLARE_FACTORY_ENTRIES( MuonCalibStreamCnvSvc ) {
    DECLARE_ALGORITHM  ( MuonCalibStreamTestAlg )
    DECLARE_SERVICE    ( MuonCalibStreamCnvSvc        )
    DECLARE_SERVICE    ( EventSelectorMuonCalibStream )
    DECLARE_SERVICE    ( MuonCalibStreamFileInputSvc )
    DECLARE_SERVICE    ( MuonCalibStreamAddressProviderSvc )
    DECLARE_SERVICE    ( MuonCalibStreamDataProviderSvc )
    DECLARE_SERVICE    ( MuonCalibRunLumiBlockCoolSvc )
    DECLARE_CONVERTER  ( MdtPRDCollCalibStreamCnv )
    DECLARE_CONVERTER  ( MdtPRDContCalibStreamCnv )
    DECLARE_CONVERTER  ( RpcRDOContCalibStreamCnv )
    DECLARE_CONVERTER  ( CscRDOContCalibStreamCnv )
    DECLARE_CONVERTER  ( EventInfoMuonCalibStreamCnv )
}

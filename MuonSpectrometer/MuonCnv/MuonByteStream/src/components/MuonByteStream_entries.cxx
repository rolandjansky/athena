#include "MuonByteStream/MdtRawDataProvider.h"
#include "MuonByteStream/RpcRawDataProvider.h"
#include "MuonByteStream/TgcRawDataProvider.h"
#include "MuonByteStream/CscRawDataProvider.h"
#include "MuonByteStream/MdtCsmContByteStreamCnv.h"
#include "MuonByteStream/CscRdoContByteStreamCnv.h"
#include "MuonByteStream/RpcPadContByteStreamCnv.h"
#include "MuonByteStream/TgcRdoContByteStreamCnv.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, MdtRawDataProvider )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, RpcRawDataProvider )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, TgcRawDataProvider )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, CscRawDataProvider )
DECLARE_CONVERTER_FACTORY (MdtCsmContByteStreamCnv)
DECLARE_CONVERTER_FACTORY (CscRdoContByteStreamCnv)
DECLARE_CONVERTER_FACTORY (RpcPadContByteStreamCnv)
DECLARE_CONVERTER_FACTORY (TgcRdoContByteStreamCnv)


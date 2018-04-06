#include "MuonByteStream/MdtRawDataProvider.h"
#include "MuonByteStream/RpcRawDataProvider.h"
#include "MuonByteStream/TgcRawDataProvider.h"
#include "MuonByteStream/CscRawDataProvider.h"
#include "MuonByteStream/MdtCsmContByteStreamCnv.h"
#include "MuonByteStream/CscRdoContByteStreamCnv.h"
#include "MuonByteStream/RpcPadContByteStreamCnv.h"
#include "MuonByteStream/TgcRdoContByteStreamCnv.h"

DECLARE_COMPONENT( Muon::MdtRawDataProvider )
DECLARE_COMPONENT( Muon::RpcRawDataProvider )
DECLARE_COMPONENT( Muon::TgcRawDataProvider )
DECLARE_COMPONENT( Muon::CscRawDataProvider )
DECLARE_CONVERTER( MdtCsmContByteStreamCnv )
DECLARE_CONVERTER( CscRdoContByteStreamCnv )
DECLARE_CONVERTER( RpcPadContByteStreamCnv )
DECLARE_CONVERTER( TgcRdoContByteStreamCnv )


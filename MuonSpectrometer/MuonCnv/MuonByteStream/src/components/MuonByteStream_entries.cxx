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
DECLARE_CONVERTER_FACTORY( MdtCsmContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( CscRdoContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( RpcPadContByteStreamCnv )
DECLARE_CONVERTER_FACTORY( TgcRdoContByteStreamCnv )


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../RpcRdoToPrepDataTool.h"
#include "../RpcPadContByteStreamTool.h"
#include "../RpcROD_Decoder.h"
#include "../RpcRDO_Decoder.h"
#include "../RPC_RawDataProviderTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RpcRdoToPrepDataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RpcPadContByteStreamTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RpcROD_Decoder )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RPC_RawDataProviderTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RpcRDO_Decoder )

DECLARE_FACTORY_ENTRIES( MuonRPC_CnvTools )
{
    DECLARE_NAMESPACE_TOOL( Muon, RpcRdoToPrepDataTool )
    DECLARE_NAMESPACE_TOOL( Muon, RpcPadContByteStreamTool )
    DECLARE_NAMESPACE_TOOL( Muon, RpcROD_Decoder )
    DECLARE_NAMESPACE_TOOL( Muon, RPC_RawDataProviderTool )
    DECLARE_NAMESPACE_TOOL( Muon, RpcRDO_Decoder )
}


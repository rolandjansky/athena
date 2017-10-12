#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TgcRdoToPrepDataTool.h"
#include "../TgcRdoContByteStreamTool.h"
#include "../TgcPrepDataReplicationTool.h"
#include "../TGC_RodDecoderReadout.h"
#include "../TGC_RodDecoderRawdata.h"
#include "../TGC_RawDataProviderTool.h"
#include "../TgcRDO_Decoder.h"
#include "../TgcPrepDataReplicationAlg.h"
#include "../TgcPrepDataReplicationTool3BCtoAllBC.h"
#include "../TgcPrepDataReplicationToolAllBCto3BC.h"


DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcRdoToPrepDataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcRdoContByteStreamTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcPrepDataReplicationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcPrepDataReplicationTool3BCtoAllBC )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcPrepDataReplicationToolAllBCto3BC )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TGC_RodDecoderReadout )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TGC_RodDecoderRawdata )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TGC_RawDataProviderTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcRDO_Decoder )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, TgcPrepDataReplicationAlg )

DECLARE_FACTORY_ENTRIES( MuonTGC_CnvTools )
{
    DECLARE_NAMESPACE_TOOL( Muon, TgcRdoToPrepDataTool )
    DECLARE_NAMESPACE_TOOL( Muon, TgcRdoContByteStreamTool )
    DECLARE_NAMESPACE_TOOL( Muon, TgcPrepDataReplicationTool )
    DECLARE_NAMESPACE_TOOL( Muon, TGC_RodDecoderReadout )
    DECLARE_NAMESPACE_TOOL( Muon, TGC_RodDecoderRawdata )
    DECLARE_NAMESPACE_TOOL( Muon, TGC_RawDataProviderTool )
    DECLARE_NAMESPACE_TOOL( Muon, TgcRDO_Decoder )
    DECLARE_NAMESPACE_TOOL( Muon, TgcPrepDataReplicationTool3BCtoAllBC )
    DECLARE_NAMESPACE_TOOL( Muon, TgcPrepDataReplicationToolAllBCto3BC )
    DECLARE_NAMESPACE_ALGORITHM( Muon, TgcPrepDataReplicationAlg )
}

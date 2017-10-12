#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MdtRdoToPrepDataTool.h"
#include "../MdtCsmContByteStreamTool.h"
#include "../MDT_RawDataProviderTool.h"
#include "../MdtROD_Decoder.h"
#include "../MdtRDO_Decoder.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MdtRdoToPrepDataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MdtCsmContByteStreamTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MDT_RawDataProviderTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MdtRDO_Decoder )
DECLARE_TOOL_FACTORY( MdtROD_Decoder )

DECLARE_FACTORY_ENTRIES( MuonMDT_CnvTools )
{
    DECLARE_NAMESPACE_TOOL( Muon, MdtRdoToPrepDataTool )
    DECLARE_NAMESPACE_TOOL( Muon, MdtCsmContByteStreamTool )
    DECLARE_NAMESPACE_TOOL( Muon, MDT_RawDataProviderTool )	
    DECLARE_NAMESPACE_TOOL( Muon, MdtRDO_Decoder )
    DECLARE_TOOL( MdtROD_Decoder )
}


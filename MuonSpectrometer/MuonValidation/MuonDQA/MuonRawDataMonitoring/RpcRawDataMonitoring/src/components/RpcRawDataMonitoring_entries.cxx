#include "GaudiKernel/DeclareFactoryEntries.h"
//////////// RPC ///////////////////////////////
#include "RpcRawDataMonitoring/RpcRawDataValAlg.h"
#include "RpcRawDataMonitoring/RpcLv1RawDataValAlg.h"
#include "RpcRawDataMonitoring/RpcLv1RawDataSectorLogic.h"
#include "RpcRawDataMonitoring/RpcLv1RawDataEfficiency.h"
#include "RpcRawDataMonitoring/RPCStandaloneTracksMon.h"
  
DECLARE_TOOL_FACTORY(RpcRawDataValAlg)
DECLARE_TOOL_FACTORY(RpcLv1RawDataValAlg)
DECLARE_TOOL_FACTORY(RpcLv1RawDataSectorLogic)
DECLARE_TOOL_FACTORY(RpcLv1RawDataEfficiency)
DECLARE_TOOL_FACTORY(RPCStandaloneTracksMon)

    
DECLARE_FACTORY_ENTRIES(RpcRawDataMonitoring)
{
   DECLARE_ALGTOOL(RpcRawDataValAlg)   
   DECLARE_ALGTOOL(RpcLv1RawDataValAlg)
   DECLARE_ALGTOOL(RpcLv1RawDataSectorLogic)
   DECLARE_ALGTOOL(RpcLv1RawDataEfficiency)
   DECLARE_ALGTOOL(RPCStandaloneTracksMon)
} 
 

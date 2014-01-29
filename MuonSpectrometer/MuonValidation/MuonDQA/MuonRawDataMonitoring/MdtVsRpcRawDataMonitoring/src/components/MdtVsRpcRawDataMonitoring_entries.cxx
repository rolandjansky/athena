#include "GaudiKernel/DeclareFactoryEntries.h"
//////////// MdtVsRpc /////////////
#include "MdtVsRpcRawDataMonitoring/MdtVsRpcRawDataValAlg.h" 
  
DECLARE_TOOL_FACTORY(MdtVsRpcRawDataValAlg)
    
DECLARE_FACTORY_ENTRIES(MdtVsRpcRawDataMonitoring)
{
   DECLARE_ALGTOOL(MdtVsRpcRawDataValAlg)
} 
 

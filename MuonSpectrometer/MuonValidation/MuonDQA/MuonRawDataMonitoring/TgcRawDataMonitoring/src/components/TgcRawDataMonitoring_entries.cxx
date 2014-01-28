#include "GaudiKernel/DeclareFactoryEntries.h"
//////////// TGC ///////////////////////////////
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
 
DECLARE_TOOL_FACTORY(TgcRawDataValAlg)
DECLARE_TOOL_FACTORY(TgcLv1RawDataValAlg)
   
DECLARE_FACTORY_ENTRIES(TgcRawDataMonitoring)
{
   DECLARE_ALGTOOL(TgcRawDataValAlg)
   DECLARE_ALGTOOL(TgcLv1RawDataValAlg)
} 
 

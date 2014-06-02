#include "TRT_Monitoring/TRT_Monitoring_Tool.h"
#include "TRT_Monitoring/TRT_Hits_Monitoring_Tool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TRT_Monitoring_Tool)
#ifdef ONLINE
DECLARE_TOOL_FACTORY(TRT_Hits_Monitoring_Tool)
#endif

DECLARE_FACTORY_ENTRIES(TRT_Monitoring) 
{
    DECLARE_ALGTOOL(TRT_Monitoring_Tool);
#ifdef ONLINE
    DECLARE_ALGTOOL(TRT_Hits_Monitoring_Tool);
#endif
}

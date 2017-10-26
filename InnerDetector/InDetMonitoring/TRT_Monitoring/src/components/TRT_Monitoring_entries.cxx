#include "TRT_Monitoring/TRT_Monitoring_Tool.h"
#include "TRT_Monitoring/TRT_Hits_Monitoring_Tool.h"

DECLARE_COMPONENT( TRT_Monitoring_Tool )
#ifdef ONLINE
DECLARE_COMPONENT( TRT_Hits_Monitoring_Tool )
#endif

#ifdef ONLINE
#endif


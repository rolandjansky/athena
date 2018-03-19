#include "GaudiKernel/DeclareFactoryEntries.h"
#include "src/TRT_ConditionsSummarySvc.h"
#include "src/TRT_ConditionsTestSvc.h"
#include "src/TRT_AlignDbSvc.h"
#include "src/TRT_CalDbSvc.h"
#include "src/TRT_StrawAlignDbSvc.h"
#include "src/TRT_DCS_ConditionsSvc.h"
#include "src/TRT_HWMappingSvc.h"
#include "src/TRT_StrawNeighbourSvc.h"
#include "src/TRT_StrawStatusSummarySvc.h"
#ifndef SIMULATIONBASE
#include "src/TRT_ByteStream_ConditionsSvc.h"
#endif
#include "src/TRT_DAQ_ConditionsSvc.h"

DECLARE_SERVICE_FACTORY( TRT_ConditionsSummarySvc )
DECLARE_SERVICE_FACTORY( TRT_ConditionsTestSvc )
DECLARE_SERVICE_FACTORY( TRT_AlignDbSvc )
DECLARE_SERVICE_FACTORY( TRT_CalDbSvc )
DECLARE_SERVICE_FACTORY( TRT_StrawAlignDbSvc )
DECLARE_SERVICE_FACTORY( TRT_DCS_ConditionsSvc )
DECLARE_SERVICE_FACTORY( TRT_HWMappingSvc )
DECLARE_SERVICE_FACTORY( TRT_StrawNeighbourSvc )
DECLARE_SERVICE_FACTORY( TRT_StrawStatusSummarySvc )
#ifndef SIMULATIONBASE
DECLARE_SERVICE_FACTORY( TRT_ByteStream_ConditionsSvc )
#endif
DECLARE_SERVICE_FACTORY( TRT_DAQ_ConditionsSvc )

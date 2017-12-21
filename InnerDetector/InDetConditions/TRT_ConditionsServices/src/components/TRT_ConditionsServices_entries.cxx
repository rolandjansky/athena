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

DECLARE_COMPONENT( TRT_ConditionsSummarySvc )
DECLARE_COMPONENT( TRT_ConditionsTestSvc )
DECLARE_COMPONENT( TRT_AlignDbSvc )
DECLARE_COMPONENT( TRT_CalDbSvc )
DECLARE_COMPONENT( TRT_StrawAlignDbSvc )
DECLARE_COMPONENT( TRT_DCS_ConditionsSvc )
DECLARE_COMPONENT( TRT_HWMappingSvc )
DECLARE_COMPONENT( TRT_StrawNeighbourSvc )
DECLARE_COMPONENT( TRT_StrawStatusSummarySvc )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( TRT_ByteStream_ConditionsSvc )
#endif
DECLARE_COMPONENT( TRT_DAQ_ConditionsSvc )


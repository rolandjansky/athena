#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/Lvl1Converter.h"
#include "TrigSteering/LvlTopoConverter.h"
#include "TrigSteering/Lvl1FromFile.h"
#include "TrigSteering/Lvl2Converter.h"
#include "TrigSteering/LoopbackConverterFromPersistency.h"
#include "TrigSteering/Lvl2ConverterFromPersistency.h"
#include "TrigSteering/ResultBuilder.h"
#include "TrigSteering/RandomScaler.h"
//#include "TrigSteering/PeriodicScaler.h"
#include "TrigSteering/ScalerSvc.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"
#include "TrigSteering/HLTResultAccessTool.h"
#include "TrigSteering/EventInfoAccessTool.h"
#include "TrigSteering/Lvl1ConsistencyChecker.h"

#include "../DefaultStreamingStrategy.h"
#include "../HeavyIonStreamingStrategy.h"

#include "../DefaultExecutionOrderStrategy.h"
#include "../OptimalExecutionOrderStrategy.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( HLT, TrigSteer )

DECLARE_NAMESPACE_SERVICE_FACTORY( HLT, ScalerSvc )

DECLARE_NAMESPACE_TOOL_FACTORY( HLT, ResultBuilder )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, RandomScaler )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, Lvl1ResultAccessTool )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, HLTResultAccessTool )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, EventInfoAccessTool )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, Lvl1Converter )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, LvlTopoConverter )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, Lvl1FromFile )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, Lvl2Converter )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, LoopbackConverterFromPersistency )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, Lvl2ConverterFromPersistency )
DECLARE_TOOL_FACTORY( Lvl1ConsistencyChecker )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, DefaultStreamingStrategy )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, HeavyIonStreamingStrategy )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, DefaultExecutionOrderStrategy )
DECLARE_NAMESPACE_TOOL_FACTORY( HLT, OptimalExecutionOrderStrategy )


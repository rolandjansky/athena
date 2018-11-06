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


DECLARE_COMPONENT( HLT::TrigSteer )

DECLARE_COMPONENT( HLT::ScalerSvc )

DECLARE_COMPONENT( HLT::ResultBuilder )
DECLARE_COMPONENT( HLT::RandomScaler )
DECLARE_COMPONENT( HLT::Lvl1ResultAccessTool )
DECLARE_COMPONENT( HLT::HLTResultAccessTool )
DECLARE_COMPONENT( HLT::EventInfoAccessTool )
DECLARE_COMPONENT( HLT::Lvl1Converter )
DECLARE_COMPONENT( HLT::LvlTopoConverter )
DECLARE_COMPONENT( HLT::Lvl1FromFile )
DECLARE_COMPONENT( HLT::Lvl2Converter )
DECLARE_COMPONENT( HLT::LoopbackConverterFromPersistency )
DECLARE_COMPONENT( HLT::Lvl2ConverterFromPersistency )
DECLARE_COMPONENT( Lvl1ConsistencyChecker )
DECLARE_COMPONENT( HLT::DefaultStreamingStrategy )
DECLARE_COMPONENT( HLT::HeavyIonStreamingStrategy )
DECLARE_COMPONENT( HLT::DefaultExecutionOrderStrategy )
DECLARE_COMPONENT( HLT::OptimalExecutionOrderStrategy )


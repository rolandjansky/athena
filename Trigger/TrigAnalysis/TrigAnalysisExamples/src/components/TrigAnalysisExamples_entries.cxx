#include "../TDTExample.h"
#include "../TagAndProbeExAlg.h"
#include "../JetMetExAlg.h"
#include "../TrigMatchExAlg.h"
#include "../TriggerAnalysisTutorial.h"
#include "../TrigExpressStreamAlg.h"
#include "../TrigEmulatorExAlg.h"
#include "../BunchCrossingExample.h"
#include "../TriggerAnalysisHelper.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig, TDTExample )
DECLARE_ALGORITHM_FACTORY(TagAndProbeExAlg)
DECLARE_ALGORITHM_FACTORY(JetMetExAlg)
DECLARE_ALGORITHM_FACTORY(TrigMatchExAlg)
DECLARE_ALGORITHM_FACTORY(TriggerAnalysisTutorial)
DECLARE_ALGORITHM_FACTORY(TrigExpressStreamAlg)
DECLARE_ALGORITHM_FACTORY(TrigEmulatorExAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trig, BunchCrossingExample )
DECLARE_NAMESPACE_TOOL_FACTORY(Trig, TriggerAnalysisHelper)


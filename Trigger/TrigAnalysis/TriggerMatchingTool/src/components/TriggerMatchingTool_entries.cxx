#include "TriggerMatchingTool/MatchingTool.h"
#include "TriggerMatchingTool/IParticleRetrievalTool.h"
#include "TriggerMatchingTool/MatchFromCompositeTool.h"
#include "TriggerMatchingTool/R3MatchingTool.h"
#include "TriggerMatchingTool/R3IParticleRetrievalTool.h"
#include "TriggerMatchingTool/DRScoringTool.h"
#include "TriggerMatchingTool/TypedScoringTool.h"
#include "TriggerMatchingTool/EgammaDRScoringTool.h"

DECLARE_COMPONENT(Trig::MatchingTool)

DECLARE_COMPONENT(Trig::MatchingTool)
DECLARE_COMPONENT(Trig::IParticleRetrievalTool)
DECLARE_COMPONENT(Trig::MatchFromCompositeTool)
DECLARE_COMPONENT(Trig::R3MatchingTool)
DECLARE_COMPONENT(Trig::R3IParticleRetrievalTool)
DECLARE_COMPONENT(Trig::DRScoringTool)
DECLARE_COMPONENT(Trig::TypedScoringTool)
DECLARE_COMPONENT(Trig::EgammaDRScoringTool)

#include "../TestMatchingToolAlg.h"
DECLARE_COMPONENT(TestMatchingToolAlg)

#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"

#include "../MuonEfficiencyCorrections_TestAlg.h"
#include "../MuonEfficiencyCorrectionsProvider.h"
#include "../TestTrigSF.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonEfficiencyScaleFactors )

DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonTriggerScaleFactors )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, MuonEfficiencyCorrections_TestAlg )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, MuonEfficiencyCorrectionsProvider )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trig, TestTrigSF )


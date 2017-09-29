
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"

#include "../MuonEfficiencyCorrections_TestAlg.h"
#include "../MuonEfficiencyCorrectionsProvider.h"
#include "../TestTrigSF.h"
#include "../TestTrigSF.h"
#include "../MuonTriggerSF_TestAlg.h"


DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonEfficiencyScaleFactors )

DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonTriggerScaleFactors )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, MuonEfficiencyCorrections_TestAlg )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, MuonEfficiencyCorrectionsProvider )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trig, TestTrigSF )

DECLARE_FACTORY_ENTRIES( MuonEfficiencyCorrections ) {

   DECLARE_NAMESPACE_TOOL( CP, MuonEfficiencyScaleFactors )

   DECLARE_NAMESPACE_TOOL( CP, MuonTriggerScaleFactors )

   DECLARE_NAMESPACE_ALGORITHM( CP, MuonEfficiencyCorrections_TestAlg )

   DECLARE_NAMESPACE_ALGORITHM( CP, MuonEfficiencyCorrectionsProvider )

   DECLARE_NAMESPACE_ALGORITHM( Trig, TestTrigSF )
}


#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"

#include "../MuonEfficiencyCorrections_TestAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonEfficiencyScaleFactors )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, MuonEfficiencyCorrections_TestAlg )

DECLARE_FACTORY_ENTRIES( MuonEfficiencyCorrections ) {

   DECLARE_NAMESPACE_TOOL( CP, MuonEfficiencyScaleFactors )

   DECLARE_NAMESPACE_ALGORITHM( CP, MuonEfficiencyCorrections_TestAlg )

}

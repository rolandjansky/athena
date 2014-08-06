#include "GaudiKernel/DeclareFactoryEntries.h"

#include "src/TrigEffJpsiTools.h"
#include "src/MuonRoiWithExtendedBunches.h"


DECLARE_TOOL_FACTORY(TrigEffJpsiTools)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(MuComm, MuonRoiWithExtendedBunches)

DECLARE_FACTORY_ENTRIES(TrigEffJpsiTools) {
  DECLARE_TOOL(TrigEffJpsiTools);
  DECLARE_NAMESPACE_ALGORITHM(MuComm, TrigEffJpsiTools);
}

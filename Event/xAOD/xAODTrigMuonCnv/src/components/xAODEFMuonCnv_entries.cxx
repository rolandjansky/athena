#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigMuonEFInfoToMuonCnvAlg.h"
#include "../TrigMuonEFInfoToMuonCnvTool.h"
#include "../TrigMuonEFInfoCnvTestAlg.h"

DECLARE_TOOL_FACTORY(TrigMuonEFInfoToMuonCnvTool)

DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, TrigMuonEFInfoToMuonCnvAlg)

DECLARE_ALGORITHM_FACTORY(TrigMuonEFInfoCnvTestAlg)

DECLARE_FACTORY_ENTRIES(xAODTrigMuonCnv) {
  DECLARE_TOOL( TrigMuonEFInfoToMuonCnvTool )

  DECLARE_NAMESPACE_ALGORITHM(xAODMaker, TrigMuonEFInfoToMuonCnvAlg )
  DECLARE_ALGORITHM( TrigMuonEFInfoCnvTestAlg )
}

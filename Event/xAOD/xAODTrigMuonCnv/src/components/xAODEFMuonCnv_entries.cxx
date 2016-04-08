#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigMuonEFInfoToMuonCnvAlg.h"
#include "../TrigMuonEFInfoToMuonCnvTool.h"
#include "../TrigMuonEFInfoCnvTestAlg.h"

#include "../IsoMuonFeatureCnvTool.h"
#include "../IsoMuonFeatureContainerCnvTool.h"
#include "../IsoMuonFeatureCnvAlg.h"
#include "../CombinedMuonFeatureCnvTool.h"
#include "../CombinedMuonFeatureContainerCnvTool.h"
#include "../CombinedMuonFeatureCnvAlg.h"

DECLARE_TOOL_FACTORY(TrigMuonEFInfoToMuonCnvTool)
DECLARE_ALGORITHM_FACTORY(TrigMuonEFInfoCnvTestAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, TrigMuonEFInfoToMuonCnvAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, IsoMuonFeatureCnvTool)
DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, IsoMuonFeatureContainerCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, IsoMuonFeatureCnvAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, CombinedMuonFeatureCnvTool)
DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, CombinedMuonFeatureContainerCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, CombinedMuonFeatureCnvAlg)

DECLARE_FACTORY_ENTRIES(xAODTrigMuonCnv)
{
   DECLARE_TOOL(TrigMuonEFInfoToMuonCnvTool)
   DECLARE_NAMESPACE_ALGORITHM(xAODMaker, TrigMuonEFInfoToMuonCnvAlg)
   DECLARE_ALGORITHM(TrigMuonEFInfoCnvTestAlg)

   DECLARE_NAMESPACE_TOOL(xAODMaker, IsoMuonFeatureCnvTool)
   DECLARE_NAMESPACE_ALGORITHM(xAODMaker, IsoMuonFeatureCnvAlg)
   DECLARE_NAMESPACE_TOOL(xAODMaker, IsoMuonFeatureContainerCnvTool)
   DECLARE_NAMESPACE_TOOL(xAODMaker, CombinedMuonFeatureCnvTool)
   DECLARE_NAMESPACE_ALGORITHM(xAODMaker, CombinedMuonFeatureCnvAlg)
   DECLARE_NAMESPACE_TOOL(xAODMaker, CombinedMuonFeatureContainerCnvTool)
}

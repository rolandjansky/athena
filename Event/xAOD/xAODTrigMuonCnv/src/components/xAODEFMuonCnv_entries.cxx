#include "../TrigMuonEFInfoToMuonCnvAlg.h"
#include "../TrigMuonEFInfoToMuonCnvTool.h"
#include "../TrigMuonEFInfoCnvTestAlg.h"

#include "../IsoMuonFeatureCnvTool.h"
#include "../IsoMuonFeatureContainerCnvTool.h"
#include "../IsoMuonFeatureCnvAlg.h"
#include "../CombinedMuonFeatureCnvTool.h"
#include "../CombinedMuonFeatureContainerCnvTool.h"
#include "../CombinedMuonFeatureCnvAlg.h"

#include "../MuonFeatureCnvAlg.h"
#include "../MuonFeatureCnvTool.h"
#include "../MuonFeatureCnvTestAlg.h"

DECLARE_TOOL_FACTORY(TrigMuonEFInfoToMuonCnvTool)
DECLARE_ALGORITHM_FACTORY(TrigMuonEFInfoCnvTestAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, TrigMuonEFInfoToMuonCnvAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, IsoMuonFeatureCnvTool)
DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, IsoMuonFeatureContainerCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, IsoMuonFeatureCnvAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, CombinedMuonFeatureCnvTool)
DECLARE_NAMESPACE_TOOL_FACTORY(xAODMaker, CombinedMuonFeatureContainerCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, CombinedMuonFeatureCnvAlg)

DECLARE_TOOL_FACTORY(MuonFeatureCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(xAODMaker, MuonFeatureCnvAlg)
DECLARE_ALGORITHM_FACTORY(MuonFeatureCnvTestAlg)


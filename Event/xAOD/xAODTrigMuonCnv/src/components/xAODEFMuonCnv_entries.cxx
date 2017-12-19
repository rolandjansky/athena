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

DECLARE_COMPONENT( TrigMuonEFInfoToMuonCnvTool )
DECLARE_COMPONENT( TrigMuonEFInfoCnvTestAlg )
DECLARE_COMPONENT( xAODMaker::TrigMuonEFInfoToMuonCnvAlg )

DECLARE_COMPONENT( xAODMaker::IsoMuonFeatureCnvTool )
DECLARE_COMPONENT( xAODMaker::IsoMuonFeatureContainerCnvTool )
DECLARE_COMPONENT( xAODMaker::IsoMuonFeatureCnvAlg )

DECLARE_COMPONENT( xAODMaker::CombinedMuonFeatureCnvTool )
DECLARE_COMPONENT( xAODMaker::CombinedMuonFeatureContainerCnvTool )
DECLARE_COMPONENT( xAODMaker::CombinedMuonFeatureCnvAlg )

DECLARE_COMPONENT( MuonFeatureCnvTool )
DECLARE_COMPONENT( xAODMaker::MuonFeatureCnvAlg )
DECLARE_COMPONENT( MuonFeatureCnvTestAlg )


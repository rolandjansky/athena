#ifndef XAOD_ANALYSIS
#include "../MuonRoICnvTool.h"
#include "../MuonRoICnvAlg.h"
#include "../JetRoICnvTool.h"
#include "../JetRoICnvAlg.h"
#include "../EmTauRoICnvTool.h"
#include "../EmTauRoICnvAlg.h"
#include "../EnergySumRoICnvTool.h"
#include "../EnergySumRoICnvAlg.h"
#include "../JetEtRoICnvTool.h"
#include "../JetEtRoICnvAlg.h"

#include "../TrigDecisionCnvTool.h"
#include "../TrigDecisionCnvAlg.h"

#include "../BunchConfCnvAlg.h"

#include "../TrigNavigationCnvTool.h"
#include "../TrigNavigationCnvAlg.h"

#include "../TrigPassBitsCnvTool.h"
#endif

#include "xAODTriggerCnv/TriggerMenuMetaDataTool.h"

#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( xAODMaker::MuonRoICnvTool )
DECLARE_COMPONENT( xAODMaker::MuonRoICnvAlg )
DECLARE_COMPONENT( xAODMaker::JetRoICnvTool )
DECLARE_COMPONENT( xAODMaker::JetRoICnvAlg )
DECLARE_COMPONENT( xAODMaker::EmTauRoICnvTool )
DECLARE_COMPONENT( xAODMaker::EmTauRoICnvAlg )
DECLARE_COMPONENT( xAODMaker::EnergySumRoICnvTool )
DECLARE_COMPONENT( xAODMaker::EnergySumRoICnvAlg )
DECLARE_COMPONENT( xAODMaker::JetEtRoICnvTool )
DECLARE_COMPONENT( xAODMaker::JetEtRoICnvAlg )

DECLARE_COMPONENT( xAODMaker::TrigDecisionCnvTool )
DECLARE_COMPONENT( xAODMaker::TrigDecisionCnvAlg )

DECLARE_COMPONENT( xAODMaker::BunchConfCnvAlg )

DECLARE_COMPONENT( xAODMaker::TrigNavigationCnvTool )
DECLARE_COMPONENT( xAODMaker::TrigNavigationCnvAlg )

DECLARE_COMPONENT( xAODMaker::TrigPassBitsCnvTool )
#endif

DECLARE_COMPONENT( xAODMaker::TriggerMenuMetaDataTool )


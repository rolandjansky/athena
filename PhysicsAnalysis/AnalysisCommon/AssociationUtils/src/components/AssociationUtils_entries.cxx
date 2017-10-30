#include "AssociationUtils/OverlapRemovalTool.h"
#include "AssociationUtils/DeltaROverlapTool.h"
#include "AssociationUtils/EleEleOverlapTool.h"
#include "AssociationUtils/EleJetOverlapTool.h"
#include "AssociationUtils/EleMuSharedTrkOverlapTool.h"
#include "AssociationUtils/MuJetOverlapTool.h"
#include "AssociationUtils/AltMuJetOverlapTool.h"
#include "AssociationUtils/ObjLinkOverlapTool.h"
#include "AssociationUtils/TauLooseEleOverlapTool.h"
#include "AssociationUtils/TauLooseMuOverlapTool.h"
#include "AssociationUtils/TauJetOverlapTool.h"
#include "AssociationUtils/TauAntiTauJetOverlapTool.h"

#include "../OverlapRemovalTestAlg.h"
#include "../ORToolBoxTestAlg.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
DECLARE_TOOL_FACTORY( OverlapRemovalTool )
#pragma GCC diagnostic pop
DECLARE_TOOL_FACTORY( ORUtils::OverlapRemovalTool )
DECLARE_TOOL_FACTORY( ORUtils::DeltaROverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::EleEleOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::EleJetOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::EleMuSharedTrkOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::MuJetOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::AltMuJetOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::ObjLinkOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::TauLooseEleOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::TauLooseMuOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::TauJetOverlapTool )
DECLARE_TOOL_FACTORY( ORUtils::TauAntiTauJetOverlapTool )

DECLARE_ALGORITHM_FACTORY( OverlapRemovalTestAlg )
DECLARE_ALGORITHM_FACTORY( ORToolBoxTestAlg )

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic pop


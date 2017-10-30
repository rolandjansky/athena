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
DECLARE_COMPONENT( OverlapRemovalTool )
#pragma GCC diagnostic pop
DECLARE_COMPONENT( ORUtils::OverlapRemovalTool )
DECLARE_COMPONENT( ORUtils::DeltaROverlapTool )
DECLARE_COMPONENT( ORUtils::EleEleOverlapTool )
DECLARE_COMPONENT( ORUtils::EleJetOverlapTool )
DECLARE_COMPONENT( ORUtils::EleMuSharedTrkOverlapTool )
DECLARE_COMPONENT( ORUtils::MuJetOverlapTool )
DECLARE_COMPONENT( ORUtils::AltMuJetOverlapTool )
DECLARE_COMPONENT( ORUtils::ObjLinkOverlapTool )
DECLARE_COMPONENT( ORUtils::TauLooseEleOverlapTool )
DECLARE_COMPONENT( ORUtils::TauLooseMuOverlapTool )
DECLARE_COMPONENT( ORUtils::TauJetOverlapTool )
DECLARE_COMPONENT( ORUtils::TauAntiTauJetOverlapTool )

DECLARE_COMPONENT( OverlapRemovalTestAlg )
DECLARE_COMPONENT( ORToolBoxTestAlg )

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic pop


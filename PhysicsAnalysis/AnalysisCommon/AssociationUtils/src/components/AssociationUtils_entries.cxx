// Gaudi/Athena includes
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local includes
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
#include "../OverlapRemovalGenUseAlg.h"
#include "../ORToolBoxTestAlg.h"

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
DECLARE_ALGORITHM_FACTORY( OverlapRemovalGenUseAlg )
DECLARE_ALGORITHM_FACTORY( ORToolBoxTestAlg )

DECLARE_FACTORY_ENTRIES( AssociationUtils ) 
{
  DECLARE_TOOL( ORUtils::OverlapRemovalTool )
  DECLARE_TOOL( ORUtils::DeltaROverlapTool )
  DECLARE_TOOL( ORUtils::EleEleOverlapTool )
  DECLARE_TOOL( ORUtils::EleJetOverlapTool )
  DECLARE_TOOL( ORUtils::EleMuSharedTrkOverlapTool )
  DECLARE_TOOL( ORUtils::MuJetOverlapTool )
  DECLARE_TOOL( ORUtils::AltMuJetOverlapTool )
  DECLARE_TOOL( ORUtils::ObjLinkOverlapTool )
  DECLARE_TOOL( ORUtils::TauLooseEleOverlapTool )
  DECLARE_TOOL( ORUtils::TauLooseMuOverlapTool )
  DECLARE_TOOL( ORUtils::TauJetOverlapTool )
  DECLARE_TOOL( ORUtils::TauAntiTauJetOverlapTool )
  DECLARE_ALGORITHM( OverlapRemovalTestAlg )
  DECLARE_ALGORITHM( OverlapRemovalGenUseAlg )
  DECLARE_ALGORITHM( ORToolBoxTestAlg )
}

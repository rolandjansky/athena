#include "../HLTSeeding.h"
#include "../HLTSeedingNoCtpForTesting.h"
#include "../CTPUnpackingToolBase.h"
#include "../CTPUnpackingTool.h"
#include "../CTPUnpackingEmulationTool.h"
#include "../eFexEMRoIThresholdsTool.h"
#include "../eFexTauRoIThresholdsTool.h"
#include "../jFexTauRoIThresholdsTool.h"
#include "../cTauRoIThresholdsTool.h"
#include "../jFexSRJetRoIThresholdsTool.h"
#include "../jFexLRJetRoIThresholdsTool.h"
#include "../gFexSRJetRoIThresholdsTool.h"
#include "../gFexLRJetRoIThresholdsTool.h"
#include "../EMRoIsUnpackingTool.h"
#include "../METRoIsUnpackingTool.h"
#include "../FSRoIsUnpackingTool.h"
#include "../JRoIsUnpackingTool.h"
#include "../TAURoIsUnpackingTool.h"
#include "../RoIsUnpackingToolBase.h"
#include "../RoIsUnpackingToolPhase1.h"
#include "../RoIsUnpackingEmulationTool.h"
#include "../MURoIsUnpackingTool.h"
#include "../PrescalingTool.h"
#include "../PrescalingEmulationTool.h"
#include "../L1TriggerResultMaker.h"
#include "../L1DataConsistencyChecker.h"

// Algorithms
DECLARE_COMPONENT( HLTSeeding )
DECLARE_COMPONENT( HLTSeedingNoCtpForTesting )
DECLARE_COMPONENT( L1TriggerResultMaker )

// Prescaling tools
DECLARE_COMPONENT( PrescalingTool )
DECLARE_COMPONENT( PrescalingEmulationTool )

// Seeding tools: CTP
DECLARE_COMPONENT( CTPUnpackingToolBase )
DECLARE_COMPONENT( CTPUnpackingTool )
DECLARE_COMPONENT( CTPUnpackingEmulationTool )

// Seeding tools: Muon
DECLARE_COMPONENT( MURoIsUnpackingTool ) // legacy
DECLARE_COMPONENT( MuonRoIsUnpackingTool ) // phase-1

// Seeding: tools L1Calo
DECLARE_COMPONENT( eFexEMRoIsUnpackingTool )
DECLARE_COMPONENT( eFexEMRoIThresholdsTool )
DECLARE_COMPONENT( eFexTauRoIsUnpackingTool )
DECLARE_COMPONENT( eFexTauRoIThresholdsTool )
DECLARE_COMPONENT( jFexTauRoIsUnpackingTool )
DECLARE_COMPONENT( jFexTauRoIThresholdsTool )
DECLARE_COMPONENT( cTauRoIsUnpackingTool )
DECLARE_COMPONENT( cTauRoIThresholdsTool )
DECLARE_COMPONENT( jFexSRJetRoIsUnpackingTool )
DECLARE_COMPONENT( jFexSRJetRoIThresholdsTool )
DECLARE_COMPONENT( jFexLRJetRoIsUnpackingTool )
DECLARE_COMPONENT( jFexLRJetRoIThresholdsTool )
DECLARE_COMPONENT( gFexSRJetRoIsUnpackingTool )
DECLARE_COMPONENT( gFexSRJetRoIThresholdsTool )
DECLARE_COMPONENT( gFexLRJetRoIsUnpackingTool )
DECLARE_COMPONENT( gFexLRJetRoIThresholdsTool )

// Seeding tools: Legacy L1Calo
DECLARE_COMPONENT( EMRoIsUnpackingTool )
DECLARE_COMPONENT( TAURoIsUnpackingTool )
DECLARE_COMPONENT( JRoIsUnpackingTool )
DECLARE_COMPONENT( METRoIsUnpackingTool )

// Seeding tools: Common
DECLARE_COMPONENT( RoIsUnpackingToolBase )
DECLARE_COMPONENT( RoIsUnpackingEmulationTool )
DECLARE_COMPONENT( FSRoIsUnpackingTool )
DECLARE_COMPONENT( L1DataConsistencyChecker )

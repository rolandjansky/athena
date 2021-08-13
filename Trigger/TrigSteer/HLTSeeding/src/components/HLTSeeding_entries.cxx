#include "../HLTSeeding.h"
#include "../HLTSeedingNoCtpForTesting.h"
#include "../CTPUnpackingToolBase.h"
#include "../CTPUnpackingTool.h"
#include "../CTPUnpackingEmulationTool.h"
#include "../eFexEMRoIsUnpackingTool.h"
#include "../eFexEMRoIThresholdsTool.h"
#include "../eFexTauRoIsUnpackingTool.h"
#include "../eFexTauRoIThresholdsTool.h"
#include "../jFexTauRoIsUnpackingTool.h"
#include "../jFexTauRoIThresholdsTool.h"
#include "../EMRoIsUnpackingTool.h"
#include "../METRoIsUnpackingTool.h"
#include "../FSRoIsUnpackingTool.h"
#include "../JRoIsUnpackingTool.h"
#include "../TAURoIsUnpackingTool.h"
#include "../RoIsUnpackingToolBase.h"
#include "../RoIsUnpackingEmulationTool.h"
#include "../MURoIsUnpackingTool.h"
#include "../PrescalingTool.h"
#include "../PrescalingEmulationTool.h"
#include "../L1TriggerResultMaker.h"

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
DECLARE_COMPONENT( MURoIsUnpackingTool )

// Seeding: tools L1Calo
DECLARE_COMPONENT( eFexEMRoIsUnpackingTool )
DECLARE_COMPONENT( eFexEMRoIThresholdsTool )
DECLARE_COMPONENT( eFexTauRoIsUnpackingTool )
DECLARE_COMPONENT( eFexTauRoIThresholdsTool )
DECLARE_COMPONENT( jFexTauRoIsUnpackingTool )
DECLARE_COMPONENT( jFexTauRoIThresholdsTool )

// Seeding tools: Legacy L1Calo
DECLARE_COMPONENT( EMRoIsUnpackingTool )
DECLARE_COMPONENT( TAURoIsUnpackingTool )
DECLARE_COMPONENT( JRoIsUnpackingTool )
DECLARE_COMPONENT( METRoIsUnpackingTool )

// Seeding tools: Common
DECLARE_COMPONENT( RoIsUnpackingToolBase )
DECLARE_COMPONENT( RoIsUnpackingEmulationTool )
DECLARE_COMPONENT( FSRoIsUnpackingTool )

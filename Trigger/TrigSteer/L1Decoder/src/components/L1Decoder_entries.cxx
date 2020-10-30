#include "../L1CaloDecoder.h"
#include "../FakeRoI.h"
#include "../L1Decoder.h"
#include "../L1TestDecoder.h"
#include "../FakeCTP.h"
#include "L1Decoder/CTPUnpackingToolBase.h"
#include "L1Decoder/CTPUnpackingTool.h"
#include "../CTPUnpackingEmulationTool.h"
#include "../EMRoIsUnpackingTool.h"
#include "../METRoIsUnpackingTool.h"
#include "../FSRoIsUnpackingTool.h"
#include "../JRoIsUnpackingTool.h"
#include "../TAURoIsUnpackingTool.h"
#include "../RoIsUnpackingToolBase.h"
#include "../RoIsUnpackingEmulationTool.h"
#include "../MURoIsUnpackingTool.h"
#include "../RerunRoIsUnpackingTool.h"
#include "../PrescalingTool.h"
#include "../PrescalingEmulationTool.h"
#include "../CreateFullScanRoI.h"
#include "../L1TriggerResultMaker.h"

DECLARE_COMPONENT( L1CaloDecoder )
DECLARE_COMPONENT( FakeRoI )
DECLARE_COMPONENT( FakeCTP )
DECLARE_COMPONENT( L1Decoder )
DECLARE_COMPONENT( L1TestDecoder )
DECLARE_COMPONENT( CTPUnpackingToolBase )
DECLARE_COMPONENT( CTPUnpackingTool )
DECLARE_COMPONENT( CTPUnpackingEmulationTool )
DECLARE_COMPONENT( EMRoIsUnpackingTool )
DECLARE_COMPONENT( METRoIsUnpackingTool )
DECLARE_COMPONENT( FSRoIsUnpackingTool )
DECLARE_COMPONENT( RoIsUnpackingToolBase )
DECLARE_COMPONENT( RoIsUnpackingEmulationTool )
DECLARE_COMPONENT( MURoIsUnpackingTool )
DECLARE_COMPONENT( JRoIsUnpackingTool )
DECLARE_COMPONENT( TAURoIsUnpackingTool )
DECLARE_COMPONENT( PrescalingTool )
DECLARE_COMPONENT( PrescalingEmulationTool )
DECLARE_COMPONENT( RerunRoIsUnpackingTool )
DECLARE_COMPONENT( CreateFullScanRoI )
DECLARE_COMPONENT( L1TriggerResultMaker )

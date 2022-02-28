// Run 3
#include "../CpmMonitorAlgorithm.h"
#include "../CpmSimMonitorAlgorithm.h"
#include "../PprMonitorAlgorithm.h"
#include "../JepJemMonitorAlgorithm.h"
#include "../JepCmxMonitorAlgorithm.h"
#include "../OverviewMonitorAlgorithm.h"

// Run 2
#include "../CPMon.h"
#include "../CPSimMon.h"
#include "../JEPJEMMon.h"
#include "../JEPCMXMon.h"
#include "../JEPSimMon.h"
#include "../PPrMon.h"
#include "../MistimedStreamMon.h"
#include "../PPrStabilityMon.h"
#include "../PPrSpareMon.h"
#include "../PPMSimBSMon.h"
#include "../RODMon.h"
#include "../OverviewMon.h"
#include "../TagProbeEfficiencyMon.h"

// Run 1
#include "../TrigT1CaloGlobalMonTool.h"

// Run 3
DECLARE_COMPONENT( CpmMonitorAlgorithm )
DECLARE_COMPONENT( CpmSimMonitorAlgorithm )
DECLARE_COMPONENT( PprMonitorAlgorithm )
DECLARE_COMPONENT( JepJemMonitorAlgorithm )
DECLARE_COMPONENT( JepCmxMonitorAlgorithm )
DECLARE_COMPONENT( OverviewMonitorAlgorithm )

// Run 2
DECLARE_COMPONENT( LVL1::OverviewMon )
DECLARE_COMPONENT( LVL1::CPMon )
DECLARE_COMPONENT( LVL1::CPSimMon )
DECLARE_COMPONENT( LVL1::JEPJEMMon )
DECLARE_COMPONENT( LVL1::JEPCMXMon )
DECLARE_COMPONENT( LVL1::JEPSimMon )

DECLARE_COMPONENT( LVL1::PPrMon )
DECLARE_COMPONENT( LVL1::MistimedStreamMon )
DECLARE_COMPONENT( LVL1::PPrStabilityMon )
DECLARE_COMPONENT( LVL1::PPrSpareMon )
DECLARE_COMPONENT( LVL1::PPMSimBSMon )
DECLARE_COMPONENT( LVL1::RODMon )

DECLARE_COMPONENT( LVL1::TagProbeEfficiencyMon )

// Run 1
DECLARE_COMPONENT( LVL1::TrigT1CaloGlobalMonTool )

#include "../PrimaryDPDPrescaler.h"
#include "PrimaryDPDMaker/ThinContainers.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "PrimaryDPDMaker/BCIDFilterTool.h"
#include "PrimaryDPDMaker/CollisionFilterAlg.h"
#include "PrimaryDPDMaker/muonTrkTrackThinTool.h"
#include "PrimaryDPDMaker/CaloCellThinningTool.h"

using namespace DerivationFramework;

DECLARE_ALGORITHM_FACTORY( PrimaryDPDPrescaler )
DECLARE_ALGORITHM_FACTORY( ThinContainers )
DECLARE_ALGORITHM_FACTORY( CollisionFilterAlg )
DECLARE_ALGORITHM_FACTORY( muonTrkTrackThinTool )
DECLARE_TOOL_FACTORY( MBTSTimeFilterTool )
DECLARE_TOOL_FACTORY( CaloTimeFilterTool )
DECLARE_TOOL_FACTORY( BCIDFilterTool )
DECLARE_TOOL_FACTORY( CaloCellThinningTool )


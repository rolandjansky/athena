#include "../PrimaryDPDPrescaler.h"
#include "PrimaryDPDMaker/ThinContainers.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "PrimaryDPDMaker/BCIDFilterTool.h"
#include "PrimaryDPDMaker/CollisionFilterAlg.h"
#include "PrimaryDPDMaker/muonTrkTrackThinTool.h"
#include "PrimaryDPDMaker/CaloCellThinningTool.h"

using namespace DerivationFramework;

DECLARE_COMPONENT( PrimaryDPDPrescaler )
DECLARE_COMPONENT( ThinContainers )
DECLARE_COMPONENT( CollisionFilterAlg )
DECLARE_COMPONENT( muonTrkTrackThinTool )
DECLARE_COMPONENT( MBTSTimeFilterTool )
DECLARE_COMPONENT( CaloTimeFilterTool )
DECLARE_COMPONENT( BCIDFilterTool )
DECLARE_COMPONENT( CaloCellThinningTool )


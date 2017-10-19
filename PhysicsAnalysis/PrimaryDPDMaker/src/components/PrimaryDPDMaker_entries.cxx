#include "GaudiKernel/DeclareFactoryEntries.h"

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

DECLARE_FACTORY_ENTRIES( PrimaryDPDMaker )
{
  DECLARE_ALGORITHM( PrimaryDPDPrescaler );
  DECLARE_ALGORITHM( ThinContainers );
  DECLARE_ALGORITHM( CollisionFilterAlg );
  DECLARE_ALGORITHM( muonTrkTrackThinTool );
  DECLARE_TOOL( MBTSTimeFilterTool );
  DECLARE_TOOL( CaloTimeFilterTool );
  DECLARE_TOOL( BCIDFilterTool );
  DECLARE_TOOL( CaloCellThinningTool );
}

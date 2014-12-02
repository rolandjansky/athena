#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../PrimaryDPDPrescaler.h"
#include "PrimaryDPDMaker/ThinContainers.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "PrimaryDPDMaker/CollisionFilterAlg.h"

DECLARE_ALGORITHM_FACTORY( PrimaryDPDPrescaler )
DECLARE_ALGORITHM_FACTORY( ThinContainers )
DECLARE_ALGORITHM_FACTORY( CollisionFilterAlg )
DECLARE_TOOL_FACTORY( MBTSTimeFilterTool )
DECLARE_TOOL_FACTORY( CaloTimeFilterTool )

DECLARE_FACTORY_ENTRIES( PrimaryDPDMaker )
{
  DECLARE_ALGORITHM( PrimaryDPDPrescaler );
  DECLARE_ALGORITHM( ThinContainers );
  DECLARE_ALGORITHM( CollisionFilterAlg );
  DECLARE_TOOL( MBTSTimeFilterTool );
  DECLARE_TOOL( CaloTimeFilterTool );
}

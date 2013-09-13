#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MuonClusterizationAlg.h"
#include "../MuonClusterizationTool.h"

using namespace Muon;

DECLARE_ALGORITHM_FACTORY( MuonClusterizationAlg )
DECLARE_TOOL_FACTORY( MuonClusterizationTool )
  
DECLARE_FACTORY_ENTRIES( MuonClusterization ) 
{
  DECLARE_ALGORITHM( MuonClusterizationAlg )
  DECLARE_TOOL( MuonClusterizationTool )
}

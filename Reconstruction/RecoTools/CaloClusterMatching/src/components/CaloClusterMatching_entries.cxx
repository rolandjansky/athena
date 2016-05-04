#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../CaloClusterMatchingTool.h"
#include "../CaloClusterMatchLinkAlg.h"

using namespace ClusterMatching;

DECLARE_TOOL_FACTORY(CaloClusterMatchingTool)

DECLARE_ALGORITHM_FACTORY(CaloClusterMatchLinkAlg)

DECLARE_FACTORY_ENTRIES(CaloClusterMatching) {
  DECLARE_TOOL(CaloClusterMatchingTool)

  DECLARE_ALGORITHM(CaloClusterMatchLinkAlg)
}

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../RPC_ResidualPullCalculator.h"
#include "../TGC_ResidualPullCalculator.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RPC_ResidualPullCalculator )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TGC_ResidualPullCalculator )

DECLARE_FACTORY_ENTRIES( MuonResidualPullCalculators )
{
  DECLARE_NAMESPACE_TOOL( Muon, RPC_ResidualPullCalculator )
  DECLARE_NAMESPACE_TOOL( Muon, TGC_ResidualPullCalculator )
}


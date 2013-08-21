#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../ResidualPullCalculator.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ResidualPullCalculator )

DECLARE_FACTORY_ENTRIES( TrkResidualPullCalculator )
{
	DECLARE_NAMESPACE_TOOL( Trk, ResidualPullCalculator )
}


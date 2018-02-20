#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DerivationFrameworkTileCal/MbtsToVectorsTool.h"
#include "DerivationFrameworkTileCal/TileCellsMuonDecorator.h"
#include "DerivationFrameworkTileCal/TileCellsDecorator.h"

#include "../TrackTools.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( MbtsToVectorsTool )
DECLARE_TOOL_FACTORY( TileCellsMuonDecorator )
DECLARE_NAMESPACE_TOOL_FACTORY(DerivationFramework, TileCellsDecorator)
DECLARE_TOOL_FACTORY( TrackTools )

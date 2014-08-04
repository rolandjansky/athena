#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonPrintingTool.h"
#include "../MuonScatteringAngleSignificanceTool.h"
#include "../MuonMomentumBalanceSignificanceTool.h"
#include "../MuonMeanMDTdADCFillerTool.h"

using namespace Rec;

DECLARE_TOOL_FACTORY( MuonPrintingTool )
DECLARE_TOOL_FACTORY( MuonScatteringAngleSignificanceTool )
DECLARE_TOOL_FACTORY( MuonMomentumBalanceSignificanceTool )
DECLARE_TOOL_FACTORY( MuonMeanMDTdADCFillerTool )

DECLARE_FACTORY_ENTRIES( MuonCombinedEvaluationTools )
{
        DECLARE_TOOL( MuonPrintingTool )
        DECLARE_TOOL( MuonScatteringAngleSignificanceTool )
        DECLARE_TOOL( MuonMomentumBalanceSignificanceTool )
        DECLARE_TOOL( uonMeanMDTdADCFillerTool )
}

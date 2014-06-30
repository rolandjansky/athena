#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "../MuonSegmentConverterTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonEDMHelperTool )
DECLARE_TOOL_FACTORY( MuonEDMPrinterTool )
DECLARE_TOOL_FACTORY( MuonSegmentConverterTool )

DECLARE_FACTORY_ENTRIES( MuonRecHelperTools ) 
{
  DECLARE_TOOL( MuonEDMHelperTool );
  DECLARE_TOOL( MuonEDMPrinterTool );
  DECLARE_TOOL( MuonSegmentConverterTool );
}

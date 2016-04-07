#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"

#include "TrigTauEmulation/EmTauSelectionTool.h"
#include "TrigTauEmulation/MuonRoISelectionTool.h"
#include "TrigTauEmulation/EnergySumSelectionTool.h"
#include "TrigTauEmulation/JetRoISelectionTool.h"
#include "TrigTauEmulation/HltTauSelectionTool.h"
#include "TrigTauEmulation/FastTrackSelectionTool.h"


#include "../TTETestAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( TrigTauEmul, Level1EmulationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TrigTauEmul, HltEmulationTool )
DECLARE_TOOL_FACTORY( EmTauSelectionTool )
DECLARE_TOOL_FACTORY( MuonRoISelectionTool )
DECLARE_TOOL_FACTORY( JetRoISelectionTool )
DECLARE_TOOL_FACTORY( EnergySumSelectionTool )
DECLARE_TOOL_FACTORY( HltTauSelectionTool )
DECLARE_ALGORITHM_FACTORY( TTETestAlg )
DECLARE_TOOL_FACTORY( EmTauSelectionTool )
DECLARE_TOOL_FACTORY( FastTrackSelectionTool )

DECLARE_FACTORY_ENTRIES( TrigTauEmulation ) 
{
  DECLARE_NAMESPACE_TOOL( TrigTauEmul, Level1EmulationTool );
  DECLARE_NAMESPACE_TOOL( TrigTauEmul, HltEmulationTool );
  DECLARE_TOOL( EmTauSelectionTool );
  DECLARE_TOOL( MuonRoISelectionTool );
  DECLARE_TOOL( JetRoISelectionTool );
  DECLARE_TOOL( EnergySumSelectionTool );
  DECLARE_TOOL( HltTauSelectionTool );
  DECLARE_ALGORITHM( TTETestAlg );
  DECLARE_TOOL( EmTauSelectionTool );
  DECLARE_TOOL( FastTrackSelectionTool );

}

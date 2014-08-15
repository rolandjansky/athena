
#include "TrigEgammaMonitoring/HLTEgammaMonTool.h"
#include "TrigEgammaMonitoring/HLTEgammaNavMonTool.h"
#include "TrigEgammaMonitoring/HLTEgammaNavSigTEMonTool.h"
#include "TrigEgammaMonitoring/HLTEgammaDumpTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HLTEgammaMonTool )
DECLARE_TOOL_FACTORY(HLTEgammaNavMonTool )
DECLARE_TOOL_FACTORY(HLTEgammaNavSigTEMonTool )
DECLARE_TOOL_FACTORY(HLTEgammaDumpTool )

DECLARE_FACTORY_ENTRIES(TrigEgammaMonitoring) {
  DECLARE_ALGTOOL( HLTEgammaMonTool )
  DECLARE_ALGTOOL( HLTEgammaNavMonTool )
  DECLARE_ALGTOOL( HLTEgammaNavSigTEMonTool )
  DECLARE_ALGTOOL( HLTEgammaDumpTool )
}

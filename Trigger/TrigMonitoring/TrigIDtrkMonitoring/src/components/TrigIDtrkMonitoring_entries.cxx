#include "TrigIDtrkMonitoring/HLTIDtrkMonTool.h"
#include "TrigIDtrkMonitoring/HLTIDtrkDumpTool.h"
#include "TrigIDtrkMonitoring/HLTIDZeeTagProbe.h"
#include "TrigIDtrkMonitoring/TIDAMonTool.h"
#include "TrigIDtrkMonitoring/HLTIDpvtxTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HLTIDtrkMonTool )
DECLARE_TOOL_FACTORY(HLTIDtrkDumpTool )
DECLARE_TOOL_FACTORY(HLTIDZeeTagProbe )
DECLARE_TOOL_FACTORY(TIDAMonTool )
DECLARE_TOOL_FACTORY(HLTIDpvtxTool )

DECLARE_FACTORY_ENTRIES(TrigIDtrkMonitoring) {

  DECLARE_ALGTOOL( HLTIDtrkMonTool )
  DECLARE_ALGTOOL( HLTIDtrkDumpTool )
  DECLARE_ALGTOOL( HLTIDZeeTagProbe )
  DECLARE_ALGTOOL( TIDAMonTool )
  DECLARE_ALGTOOL( HLTIDpvtxTool )

}

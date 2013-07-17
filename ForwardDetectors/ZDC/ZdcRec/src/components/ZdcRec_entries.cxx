#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ZdcRec/ZdcRec.h"
#include "ZdcRec/ZdcRecChannelTool.h"
#include "ZdcRec/ZdcRecNoiseTool.h"
#include "ZdcRec/ZdcSignalSinc.h"

DECLARE_ALGORITHM_FACTORY 	(ZdcRec)
DECLARE_TOOL_FACTORY 		(ZdcRecChannelTool)
DECLARE_TOOL_FACTORY 		(ZdcRecNoiseTool)

DECLARE_FACTORY_ENTRIES (ZdcRec) {
  	 DECLARE_TOOL      (ZdcRecChannelTool)
  	 DECLARE_TOOL      (ZdcRecNoiseTool)
	 DECLARE_ALGORITHM (ZdcRec)
}


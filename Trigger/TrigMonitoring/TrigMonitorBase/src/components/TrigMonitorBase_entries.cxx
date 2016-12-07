#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigHLTBaseTreeTool.h"
#include "../TrigGenericMonitoringTool.h"
#include <boost/preprocessor/punctuation/comma.hpp>

DECLARE_TOOL_FACTORY( TrigHLTBaseTreeTool )
DECLARE_TOOL_FACTORY( TrigGenericMonitoringToolST )
DECLARE_TOOL_FACTORY( TrigGenericMonitoringToolMT )

DECLARE_FACTORY_ENTRIES(TrigMonitorBase) {
  DECLARE_ALGTOOL( TrigHLTBaseTreeTool )
  DECLARE_ALGTOOL( TrigGenericMonitoringToolST )
  DECLARE_ALGTOOL( TrigGenericMonitoringToolMT )
}


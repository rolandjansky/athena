#include "../HiveAlgA.h"
#include "../HiveAlgB.h"
#include "../HiveAlgC.h"
#include "../HiveAlgD.h"
#include "../HiveAlgE.h"
#include "../HiveAlgF.h"
#include "../HiveAlgG.h"
#include "../HiveTool.h"
#include "../HiveExSvc.h"

#include "../ThreadInitTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( HiveAlgA )
DECLARE_ALGORITHM_FACTORY( HiveAlgB )
DECLARE_ALGORITHM_FACTORY( HiveAlgC )
DECLARE_ALGORITHM_FACTORY( HiveAlgD )
DECLARE_ALGORITHM_FACTORY( HiveAlgE )
DECLARE_ALGORITHM_FACTORY( HiveAlgF )
DECLARE_ALGORITHM_FACTORY( HiveAlgG )

DECLARE_TOOL_FACTORY( ThreadInitTool )
DECLARE_TOOL_FACTORY( HiveTool )

DECLARE_SERVICE_FACTORY( HiveExSvc )

DECLARE_FACTORY_ENTRIES(AthExHive) {
  DECLARE_ALGORITHM( HiveAlgA )
  DECLARE_ALGORITHM( HiveAlgB )
  DECLARE_ALGORITHM( HiveAlgC )
  DECLARE_ALGORITHM( HiveAlgD )
  DECLARE_ALGORITHM( HiveAlgE )
  DECLARE_ALGORITHM( HiveAlgF )
  DECLARE_ALGORITHM( HiveAlgG )
  DECLARE_TOOL( ThreadInitTool )
  DECLARE_TOOL( HiveTool )
  DECLARE_SERVICE( HiveExSvc )
}

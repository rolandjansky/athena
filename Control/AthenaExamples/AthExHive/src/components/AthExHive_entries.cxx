#include "../HiveAlgA.h"
#include "../HiveAlgB.h"
#include "../HiveAlgC.h"
#include "../HiveAlgD.h"
#include "../HiveAlgE.h"
#include "../HiveAlgF.h"
#include "../HiveAlgG.h"
#include "../HiveAlgL1.h"
#include "../HiveAlgL2.h"
#include "../HiveAlgL3.h"
#include "../HiveAlgM.h"
#include "../HiveAlgV.h"
#include "../HiveTool.h"
#include "../HiveExSvc.h"
#include "../ASCIICondDbSvc.h"

#ifdef REENTRANT_GAUDI
  #include "../HiveAlgR.h"
#endif

#include "../AlgA.h"
#include "../AlgB.h"
#include "../AlgC.h"
#include "../AlgD.h"
#include "../AlgT.h"
#include "../CondAlgX.h"
#include "../CondAlgY.h"

#include "../ThreadInitTool.h"


DECLARE_ALGORITHM_FACTORY( HiveAlgA )
DECLARE_ALGORITHM_FACTORY( HiveAlgB )
DECLARE_ALGORITHM_FACTORY( HiveAlgC )
DECLARE_ALGORITHM_FACTORY( HiveAlgD )
DECLARE_ALGORITHM_FACTORY( HiveAlgE )
DECLARE_ALGORITHM_FACTORY( HiveAlgF )
DECLARE_ALGORITHM_FACTORY( HiveAlgG )
DECLARE_ALGORITHM_FACTORY( HiveAlgL1 )
DECLARE_ALGORITHM_FACTORY( HiveAlgL2 )
DECLARE_ALGORITHM_FACTORY( HiveAlgL3 )
DECLARE_ALGORITHM_FACTORY( HiveAlgM )
DECLARE_ALGORITHM_FACTORY( HiveAlgV )

#ifdef REENTRANT_GAUDI
  DECLARE_ALGORITHM_FACTORY( HiveAlgR )
#endif

DECLARE_ALGORITHM_FACTORY( AlgA )
DECLARE_ALGORITHM_FACTORY( AlgB )
DECLARE_ALGORITHM_FACTORY( AlgC )
DECLARE_ALGORITHM_FACTORY( AlgD )
DECLARE_ALGORITHM_FACTORY( AlgT )
DECLARE_ALGORITHM_FACTORY( CondAlgX )
DECLARE_ALGORITHM_FACTORY( CondAlgY )

DECLARE_TOOL_FACTORY( ThreadInitTool )
DECLARE_TOOL_FACTORY( HiveTool )

DECLARE_SERVICE_FACTORY( HiveExSvc )
DECLARE_SERVICE_FACTORY( ASCIICondDbSvc )


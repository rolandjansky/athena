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


DECLARE_COMPONENT( HiveAlgA )
DECLARE_COMPONENT( HiveAlgB )
DECLARE_COMPONENT( HiveAlgC )
DECLARE_COMPONENT( HiveAlgD )
DECLARE_COMPONENT( HiveAlgE )
DECLARE_COMPONENT( HiveAlgF )
DECLARE_COMPONENT( HiveAlgG )
DECLARE_COMPONENT( HiveAlgL1 )
DECLARE_COMPONENT( HiveAlgL2 )
DECLARE_COMPONENT( HiveAlgL3 )
DECLARE_COMPONENT( HiveAlgM )
DECLARE_COMPONENT( HiveAlgV )

#ifdef REENTRANT_GAUDI
  DECLARE_COMPONENT( HiveAlgR )
#endif

DECLARE_COMPONENT( AlgA )
DECLARE_COMPONENT( AlgB )
DECLARE_COMPONENT( AlgC )
DECLARE_COMPONENT( AlgD )
DECLARE_COMPONENT( AlgT )
DECLARE_COMPONENT( CondAlgX )
DECLARE_COMPONENT( CondAlgY )

DECLARE_COMPONENT( ThreadInitTool )
DECLARE_COMPONENT( HiveTool )

DECLARE_COMPONENT( HiveExSvc )
DECLARE_COMPONENT( ASCIICondDbSvc )


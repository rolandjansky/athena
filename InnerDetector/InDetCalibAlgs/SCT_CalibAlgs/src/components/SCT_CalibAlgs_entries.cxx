#include "SCT_CalibAlgs/SCTCalib.h"
#include "SCT_CalibAlgs/SCTCalibWriteSvc.h"
#include "../SCT_CalibHitmapSvc.h"
#include "../SCT_CalibLbSvc.h"
#include "../SCT_CalibBsErrorSvc.h"
#include "../SCT_CalibModuleListSvc.h"
#include "../SCT_CalibEventInfo.h"



DECLARE_ALGORITHM_FACTORY( SCTCalib )
DECLARE_SERVICE_FACTORY( SCT_CalibBsErrorSvc )
DECLARE_SERVICE_FACTORY( SCTCalibWriteSvc )
DECLARE_SERVICE_FACTORY(SCT_CalibHitmapSvc)
DECLARE_SERVICE_FACTORY( SCT_CalibLbSvc )
DECLARE_SERVICE_FACTORY( SCT_CalibModuleListSvc )
DECLARE_SERVICE_FACTORY( SCT_CalibEventInfo )

  


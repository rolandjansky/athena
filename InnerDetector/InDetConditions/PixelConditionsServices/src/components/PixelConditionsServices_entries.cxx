#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../PixelOfflineCalibSvc.h"
#include "../PixelCalibSvc.h"
#include "../PixelCalibServiceTest.h"
#include "../SpecialPixelMapSvc.h"
#include "../PixelMapTestAlg.h"
#include "../PixelDCSSvc.h"
#include "../PixelDCSTestSvc.h"
#include "../PixelConditionsSummarySvc.h"
#include "../PixelSiliconConditionsSvc.h"
#include "../PixelByteStreamErrorsSvc.h"
#include "../PixelTDAQSvc.h"
#include "../PixelRunConditionsDbSvc.h"
#include "../PixelOfflineCalibServiceTest.h"
//#include "../PixelConnCoolDbSvc.h"

DECLARE_SERVICE_FACTORY( PixelOfflineCalibSvc )
DECLARE_SERVICE_FACTORY( PixelCalibSvc )
DECLARE_ALGORITHM_FACTORY( PixelOfflineCalibServiceTest )
DECLARE_ALGORITHM_FACTORY( PixelCalibServiceTest )
DECLARE_SERVICE_FACTORY( SpecialPixelMapSvc )
DECLARE_ALGORITHM_FACTORY( PixelMapTestAlg )
DECLARE_SERVICE_FACTORY( PixelDCSSvc )
DECLARE_ALGORITHM_FACTORY( PixelDCSTestSvc )
DECLARE_SERVICE_FACTORY( PixelConditionsSummarySvc )
DECLARE_SERVICE_FACTORY( PixelSiliconConditionsSvc )
DECLARE_SERVICE_FACTORY( PixelByteStreamErrorsSvc )
DECLARE_SERVICE_FACTORY( PixelTDAQSvc )
DECLARE_SERVICE_FACTORY( PixelRunConditionsDbSvc )
  //DECLARE_SERVICE_FACTORY( PixelConnCoolDbSvc )

DECLARE_FACTORY_ENTRIES( PixelConditionsServices )
{
  DECLARE_SERVICE( PixelOfflineCalibSvc )
  DECLARE_SERVICE( PixelCalibSvc )
  DECLARE_ALGORITHM( PixelOfflineCalibServiceTest )
  DECLARE_ALGORITHM( PixelCalibServiceTest )
  DECLARE_SERVICE( SpecialPixelMapSvc )
  DECLARE_ALGORITHM( PixelMapTestAlg )
  DECLARE_ALGORITHM( PixelRunConditionsDbTest )
  DECLARE_SERVICE( PixelDCSSvc )
  DECLARE_ALGORITHM( PixelDCSTestSvc )
  DECLARE_SERVICE( PixelConditionsSummarySvc )
  DECLARE_SERVICE( PixelSiliconConditionsSvc )
  DECLARE_SERVICE( PixelByteStreamErrorsSvc )
  DECLARE_SERVICE( PixelTDAQSvc )
  DECLARE_SERVICE( PixelRunConditionsDbSvc )
    //DECLARE_SERVICE( PixelConnCoolDbSvc )    
}


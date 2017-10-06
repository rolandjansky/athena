


#include "TrigConfigSvc/L1TopoConfigSvc.h"
#include "TrigConfigSvc/LVL1ConfigSvc.h"
#include "TrigConfigSvc/DSConfigSvc.h"
#include "TrigConfigSvc/HLTConfigSvc.h"
#include "TrigConfigSvc/HLTJobOptionsSvc.h"
#include "TrigConfigSvc/TrigConfigSvc.h"
#include "TrigConfigSvc/TrigConfDataIOVChanger.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( TrigConf, TrigConfDataIOVChanger )

DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, L1TopoConfigSvc     )
DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, LVL1ConfigSvc     )
DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, HLTConfigSvc      )
DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, HLTJobOptionsSvc  )
DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, DSConfigSvc       )
DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, TrigConfigSvc     )


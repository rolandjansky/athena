#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../FastCaloSimSvc.h"
#include "../FastCaloSimV2ParamSvc.h"
#include "../FastCaloSimSvcV2.h"
#include "../FastCaloSimSvcPU.h"
#include "../DNNCaloSimSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimV2ParamSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimSvcV2 )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimSvcPU )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , DNNCaloSimSvc )

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT1TGCRecRoiSvc/TGCRecRoiSvc.h"


DECLARE_NAMESPACE_SERVICE_FACTORY  ( LVL1TGC, TGCRecRoiSvc )

DECLARE_FACTORY_ENTRIES(TrigT1TGCRecRoiSvc) {
  DECLARE_NAMESPACE_SERVICE   ( LVL1TGC, TGCRecRoiSvc )
}

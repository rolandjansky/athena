#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ValgrindSvc.h"
#include "../ValgrindAuditor.h"

DECLARE_SERVICE_FACTORY( ValgrindSvc )
DECLARE_AUDITOR_FACTORY( ValgrindAuditor )
  
DECLARE_FACTORY_ENTRIES(Valkyrie) {
  DECLARE_SERVICE( ValgrindSvc )
  DECLARE_AUDITOR( ValgrindAuditor )
}

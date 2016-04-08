#include "SGAudSvc/SGAudSvc.h"
//#include "../PerfMonAuditor.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_SERVICE_FACTORY( SGAudSvc  )
//DECLARE_AUDITOR_FACTORY( PerfMonAuditor )

DECLARE_FACTORY_ENTRIES( SGAudSvc ) {
  DECLARE_SERVICE( SGAudSvc )
//  DECLARE_AUDITOR( PerfMonAuditor )

}

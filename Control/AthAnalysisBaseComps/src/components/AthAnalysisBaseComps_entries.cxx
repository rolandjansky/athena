
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../AthROOTErrorHandlerSvc.h"

DECLARE_SERVICE_FACTORY(AthROOTErrorHandlerSvc)


DECLARE_FACTORY_ENTRIES(AthAnalysisBaseComps) {
 
   DECLARE_SERVICE(AthROOTErrorHandlerSvc)

}

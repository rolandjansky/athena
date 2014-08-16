#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_HepMC_Services/HepMC_TruthSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , HepMC_TruthSvc )

DECLARE_FACTORY_ENTRIES( ISF_HepMC_Services ) {
  DECLARE_NAMESPACE_SERVICE( ISF ,  HepMC_TruthSvc )
}   


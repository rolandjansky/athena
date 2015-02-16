#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../Geant4SimSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( iGeant4 , Geant4SimSvc )

DECLARE_FACTORY_ENTRIES( ISF_Geant4Services) {
  DECLARE_NAMESPACE_SERVICE( iGeant4 ,  Geant4SimSvc )
}

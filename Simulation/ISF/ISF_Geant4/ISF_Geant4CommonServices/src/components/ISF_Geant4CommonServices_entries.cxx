#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../G4PolyconeGeoIDSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , G4PolyconeGeoIDSvc )

DECLARE_FACTORY_ENTRIES( ISF_Geant4Services) {
  DECLARE_NAMESPACE_SERVICE( ISF ,  G4PolyconeGeoIDSvc )
}
